import os
from flask import Flask, redirect, url_for,request
from flask_dance.contrib.google import make_google_blueprint, google
from dash import Dash
from qpython import qconnection

q = qconnection.QConnection(host = 'localhost', port = 7778)
q.open()

server = Flask(__name__)
server.secret_key = os.environ.get("FLASK_SECRET_KEY", "supersekrit")
server.config["GOOGLE_OAUTH_CLIENT_ID"] = os.environ.get("GOOGLE_OAUTH_CLIENT_ID")
server.config["GOOGLE_OAUTH_CLIENT_SECRET"] = os.environ.get("GOOGLE_OAUTH_CLIENT_SECRET")
google_bp = make_google_blueprint(scope=["profile", "email"])
server.register_blueprint(google_bp, url_prefix="/login")

@server.route("/")
def index():
    if not google.authorized:
        return redirect(url_for("google.login"))
    try:
        resp = google.get("/oauth2/v1/userinfo")
        assert resp.ok, resp.text
    except:
        return redirect(url_for("google.login"))
    return redirect('http://ivoryhuo.com/avalon')

app = Dash(
    __name__,
    routes_pathname_prefix='/avalon/'
)

import json
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output, State


rowNb=12
fontSize=20
app = Dash(
        __name__, 
        server=server,
        meta_tags=[ {"name": "viewport", "content": "width=device-width, initial-scale=1"} ],
        )

app.layout = html.Div([
    html.Div(id='cache_history', style={'display': 'none'}),
    dcc.Textarea(
        id='history',
        readOnly=True,
        className='textarea',
        rows=rowNb,
        style={'color': 'blue', 'fontSize': fontSize-3},
        ),
    html.Div(),
    dcc.Input(id='input_cmd', type='text', placeholder='Enter your command here.',value='', debounce=True, className='input', style={'fontSize': fontSize},),
        
    html.Button(id='submit_button', n_clicks=0, children='Submit', className='button', style={'fontSize': fontSize}),
    ])

@app.callback(
        [ Output('cache_history', 'children'), Output('input_cmd', 'value'), ], 
        [Input('submit_button','n_clicks')],
        [
            State('input_cmd', 'value'),
            State('cache_history', 'children')]
        )
def update_output_div(click, input_value, existe_value):
    if not google.authorized:
        return json.dumps(['not logged in, go to ivoryhuo.com']),'ivoryhuo.com'
    resp = google.get("/oauth2/v1/userinfo")
    assert resp.ok, resp.text
    print(resp.json()['id'])
    if not input_value:
        return existe_value,''
    qres=''
    if not existe_value:
        res=['Interaction history']
    else:
        command='python["'+resp.json()['id']+'";"'+input_value+'"]'
        print('command '+command)
        try:
            qres=q.sendSync(command).decode('UTF-8')
            print(qres)
        except:
            qres='unable to connect or illegal command'
        res=json.loads(existe_value)
    return [json.dumps(([input_value,qres]+res)),'']


@app.callback(
        Output('history', 'value'), 
        [Input('cache_history','children')],
        )
def update_output_div(existe_value):
    if existe_value:
        return '\n'.join(json.loads(existe_value))
    return 'Interaction history'

