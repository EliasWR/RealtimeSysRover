from flask import Flask, render_template
import netifaces

#INTERFACE = '{98FF9398-32A3-4162-8A1F-4D66F8420747}' # GUID for wifi-interface på Martin's bigboi
INTERFACE = ('{1FD34547-92DE-402B-8482-6D7AE772796A}') # GUID for wifi-interface på Elias's bigboi
#{1FD34547-92DE-402B-8482-6D7AE772796A}
#{ceab951d-d811-4e9b-99e4-ba7e22117f90}
#{7abed608-0a5d-4dc1-832f-07af8b689a14}
#{caa36097-329f-49b6-b7af-2c5b232d02db}



def get_ip_address():
    return netifaces.ifaddresses(INTERFACE)[netifaces.AF_INET][0]['addr']


app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')

if __name__ == '__main__':
    app.run(debug=True, host=get_ip_address(), port=5000)
