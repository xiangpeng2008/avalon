from dash import Dash
import json
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output, State
# from flask import request,Flask
from flask import request
# from dash_google_auth import GoogleOAuth

# server = Flask(__name__)
# server.config.update({
    # 'GOOGLE_OAUTH_CLIENT_ID': '393849065511-j9mq7bms2e8a13r94onvjdhel6vl8go3.apps.googleusercontent.com',
    # 'GOOGLE_OAUTH_CLIENT_SECRET': 'pUEBVllmHoHGstviLMVGCADs',
    # })


# authorized_emails = []
# additional_scopes = []

rowNb=12
fontSize=20
# app = Dash(__name__, server=server, url_base_pathname='/', meta_tags=[ {"name": "viewport", "content": "width=device-width, initial-scale=1"} ])
app = Dash(__name__, meta_tags=[ {"name": "viewport", "content": "width=device-width, initial-scale=1"} ])
# app.secret_key = 'super23'
# auth = GoogleOAuth(app, authorized_emails, additional_scopes)
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
    print(request.remote_addr)
    if not input_value:
        return existe_value,''
    if not existe_value:
        res=['Interaction history']
    else:
        res=json.loads(existe_value)[1-rowNb:]
    return [json.dumps(res+[input_value]),'']


@app.callback(
        Output('history', 'value'), 
        [Input('cache_history','children')],
        )
def update_output_div(existe_value):
    if existe_value:
        return '\n'.join(json.loads(existe_value))
    return 'Interaction history'

if __name__ == '__main__':
    # app.run_server(debug=True, port = 7777)
    app.run_server(debug=True,host='0.0.0.0',port = 7777)
