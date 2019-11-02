import dash
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output, State
import json
import flask

import pandas as pd
import numpy as np
import re
import socket



HOST = '127.0.0.1'    # The remote host
PORT = 20001          # The same port as used by the server
serverCpp=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serverCpp.connect((HOST, PORT))

app = dash.Dash(
    __name__,
    external_stylesheets=['https://codepen.io/chriddyp/pen/bWLwgP.css'],
    meta_tags=[ {"name": "viewport", "content": "width=device-width, initial-scale=1"} ],
)
fontSize=20
login_form = html.Div([
    html.Form([
        dcc.Input(placeholder='username', name='username', type='text'),
        dcc.Input(placeholder='password', name='password', type='password'),
        html.Button('Login', type='submit')
    ], action='/login', method='post')
])

url_bar_and_content_div = html.Div([
    dcc.Location(id='url', refresh=False),
    html.Div(id='page-content')
])

avalon_form = html.Div( [
    html.Div([
        dcc.Dropdown(
            id='input_option',
            options=[
                {'label': 'vote success',              'value': 'vote,true'      },#投成功票
                {'label': 'vote fail',                 'value': 'vote,false'     },#投失败票
                {'label': 'show game progress',        'value': 'progress'       },#游戏进度
                {'label': 'assassinate *',             'value': 'assassinate'    },#行刺
                {'label': 'show people on this table', 'value': 'people_on_table'},#玩家们
                {'label': 'newgame',                   'value': 'newgame'        },#开局
                {'label': 'show my profile',           'value': 'who'            },#我的身份
                {'label': 'join',                      'value': 'join'           },#加入
                {'label': 'set number of people *',    'value': 'set_nb_people'  },#设定游戏人数 
                ],
            value='',
            style={'fontSize': fontSize},
            className='fullScreen',
            ),
        dcc.Input(id='input_cmd', type='text', placeholder='Enter your text here.',value='', debounce=True, className='input_cmd', style={'fontSize': fontSize},),
        html.Button(id='submit_button', n_clicks=0, children='Submit', className='button', style={'fontSize': fontSize}),]),
    html.Div(id='cache_history', style={'display': 'none'}, children="[\"Interaction history\"]"),
    html.Div(id='history'),
    ])

@app.server.route('/login', methods=['POST'])
def route_login():
    data = flask.request.form
    username = re.sub(' ', '', data.get('username'))
    password = data.get('password')

    if not username or not password=="pass123":
        flask.abort(401)

    # actual implementation should verify the password.
    # Recommended to only keep a hash in database and use something like
    # bcrypt to encrypt the password and check the hashed results.

    # Return a redirect with
    rep = flask.redirect('/avalon')

    # Here we just store the given username in a cookie.
    # Actual session cookies should be signed or use a JWT token.
    rep.set_cookie('custom_auth_session', username)
    return rep

# create a logout route
@app.server.route('/logout', methods=['POST'])
def route_logout():
    # Redirect back to the index and remove the session cookie.
    rep = flask.redirect(_app_route)
    rep.set_cookie('custom_auth_session', '', expires=0)
    return rep

def serve_layout():
    if flask.has_request_context():
        return url_bar_and_content_div
    return html.Div([
        url_bar_and_content_div,
        avalon_form,
    ])

app.layout = serve_layout

# Index callbacks
@app.callback(Output('page-content', 'children'),
              [Input('url', 'pathname')])
def display_page(pathname):
    if pathname == "/avalon":
        return avalon_form
    elif pathname == "/login":
        return login_form

@app.callback(
        [ Output('cache_history', 'children'), Output('input_option', 'value'),Output('input_cmd', 'value'), ], 
        [Input('submit_button','n_clicks')],
        [
            State('input_option', 'value'),
            State('input_cmd', 'value'),
            State('cache_history', 'children')]
        )
def update_output_div(click, input_opt, input_cmd, existe_value):
    session_cookie = flask.request.cookies.get('custom_auth_session')
    if not session_cookie:
        return json.dumps(['not logged in, please go to click [ivoryhuo.com/login](http://ivoryhuo.com/login) to login']),'',''
        # return ['not logged in, please go to click [127.0.0.1:8050/login](http://127.0.0.1:8050/login) to login'],'',''
    print(session_cookie)
    if not input_opt:
        return existe_value,'',''
    command=session_cookie+','+input_opt+( ','+ input_cmd.replace(" ", "") if input_cmd else '')
    print('command: '+command)
    serverCpp.sendall(command.encode('utf-8'))
    qres = serverCpp.recv(1024)
    print("qres is ")
    print(qres)
    if isinstance(qres, pd.core.frame.DataFrame):
        str_df = qres.select_dtypes([np.object])
        str_df = str_df.stack().str.decode('utf-8').unstack()
        for col in str_df:
            qres[col] = str_df[col]
        qres=qres.to_string(index=False)
    elif isinstance(qres, list):
        qres='\n'.join([i.decode('UTF-8') for i in qres])
    else:
        qres=qres.decode('UTF-8')
    res=json.loads(existe_value)
    return [json.dumps((["*"+input_opt+" "+input_cmd+"*","```bash\n"+qres+"\n```"]+res)),'','']

@app.callback(
        Output('history', 'children'), 
        [Input('cache_history','children')],
        )
def update_output_div(existe_value):
    res = '\n'.join(json.loads(existe_value))
    print(res)
    return dcc.Markdown(res)

server = app.server
