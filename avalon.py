import os
from flask import Flask, redirect, url_for,request
from flask_dance.contrib.google import make_google_blueprint, google
from dash import Dash
from qpython import qconnection
import pandas as pd
import numpy as np

q = qconnection.QConnection(host = 'localhost', port = 7778, pandas = True)
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
    return redirect('http://ivoryhuo.com/avalon/')
    # return redirect('localhost:7777/avalon/')

import json
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output, State


fontSize=20

app = Dash(
        __name__, 
        server=server,
        routes_pathname_prefix='/avalon/',
        meta_tags=[ {"name": "viewport", "content": "width=device-width, initial-scale=1"} ],
        )

app.layout = html.Div( [
    html.Div([
        dcc.Dropdown(
            id='input_option',
            options=[
                {'label': 'vote success', 'value': 'vote[`success]'},#投成功票
                {'label': 'vote fail', 'value': 'vote[`fail]'},#投失败票
                {'label': 'show game progress', 'value': 'records'},#游戏进度
                {'label': 'assassinate *', 'value': 'assassinate'},#行刺
                {'label': 'show people on this table', 'value': 'string value id2names'},#玩家们
                {'label': 'newgame', 'value': 'newgame`'},#开局
                {'label': 'show my profile', 'value': 'who`'},#我的身份
                {'label': 'join', 'value': 'join`'},#加入
                {'label': 'I\'m *', 'value': 'im'},#我是
                {'label': 'set number of people *', 'value': 'set_nb_people'},#设定游戏人数 
                {'label': '!!!reset players', 'value': 'refresh_players`'},
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

@app.callback(
        [ Output('cache_history', 'children'), Output('input_option', 'value'),Output('input_cmd', 'value'), ], 
        [Input('submit_button','n_clicks')],
        [
            State('input_option', 'value'),
            State('input_cmd', 'value'),
            State('cache_history', 'children')]
        )
def update_output_div(click, input_opt, input_cmd, existe_value):
    if not google.authorized:
        return json.dumps(['not logged in, please go to click [ivoryhuo.com](http://ivoryhuo.com) to login']),'',''
    try:
        resp = google.get("/oauth2/v1/userinfo")
        assert resp.ok, resp.text
    except:
        return json.dumps(['google session expired, please click [ivoryhuo.com](http://ivoryhuo.com) to relogin']),'',''
    if not input_opt:
        return existe_value,'',''
    qres=''
    command='python["'+resp.json()['id']+'";"'+input_opt+('[\\"'+input_cmd.replace(" ", "")+'\\"]' if input_cmd else '')+'"]'
    print('command: '+command)
    qres=q.sendSync(command)
    print("qres is")
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
