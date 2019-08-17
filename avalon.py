import dash
import json
import dash_core_components as dcc
import dash_html_components as html
from dash.dependencies import Input, Output, State
from flask import request

rowNb=12
app = dash.Dash(__name__)
app.layout = html.Div([
    html.Div(id='cache_history', style={'display': 'none'}),
    dcc.Textarea(
        id='history',
        readOnly=True,
        className='textarea',
        rows=rowNb,
        ),
    html.Div(),
    dcc.Input(id='input_cmd', type='text', placeholder='Enter your command here.',value='', debounce=True, className='input'),
    html.Button(id='submit_button', n_clicks=0, children='Submit', className='button'),
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
    return [json.dumps(res+[input_value]),'x']


@app.callback(
        Output('history', 'value'), 
        [Input('cache_history','children')],
        )
def update_output_div(existe_value):
    if existe_value:
        return '\n'.join(json.loads(existe_value))
    return 'Interaction history'

if __name__ == '__main__':
    app.run_server(debug=True)
