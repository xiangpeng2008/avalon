import os
from flask import Flask, redirect, url_for,request
from flask_dance.contrib.google import make_google_blueprint, google
from dash import Dash
from qpython import qconnection
import pandas as pd

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
    # return redirect('http://ivoryhuo.com/avalon/')
    return redirect('localhost:7777/avalon')

import json
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output, State


fontSize=20
# external_stylesheets = ['https://www.w3schools.com/w3css/4/w3.css']

app = Dash(
        __name__, 
        # external_stylesheets=external_stylesheets,
        server=server,
        routes_pathname_prefix='/avalon/',
        meta_tags=[ {"name": "viewport", "content": "width=device-width, initial-scale=1"} ],
        )

app.layout = html.Div( [
    html.Div([
        dcc.Dropdown(
            id='input_cmd',
            options=[
                {'label': 'get records', 'value': 'get records'},
                {'label': 'who', 'value': 'who'},
                {'label': 'refresh_players', 'value': 'refresh_players'},
                {'label': 'set_nb_people *', 'value': 'set_nb_people'},
                {'label': 'I\'m *', 'value': 'im'},
                {'label': 'join', 'value': 'join'},
                {'label': 'newgame', 'value': 'newgame'},
                {'label': 'vote success', 'value': 'vote success'},
                {'label': 'vote fail', 'value': 'vote fail'},
                ],
            value='',
            style={'fontSize': fontSize},
            className='input',
            ),
        html.Button(id='submit_button', n_clicks=0, children='Submit', className='button', style={'fontSize': fontSize}),]),
    html.Div(id='cache_history', style={'display': 'none'}, children="[\"Interaction history\"]"),
    html.Div(id='history'),
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
    if not input_value:
        return existe_value,''
    qres=''
    command='python["'+resp.json()['id']+'";"'+input_value+'"]'
    print('command: '+command)
    # try:
    qres=q.sendSync(command)
    # qres=q.sendSync(command).decode('UTF-8')
    print("qres is")
    print(qres)
# except:
    # qres='unable to connect or illegal command'
    if isinstance(qres, pd.core.frame.DataFrame):
        qres=qres.to_string(index=False)
    else:
        qres=qres.decode('UTF-8')
    
    res=json.loads(existe_value)
    return [json.dumps((["*"+input_value+"*","```py\n"+qres+"\n```"]+res)),'']


@app.callback(
        Output('history', 'children'), 
        [Input('cache_history','children')],
        )
def update_output_div(existe_value):
    res = '\n'.join(json.loads(existe_value))
    print(res)
    return dcc.Markdown(res)
