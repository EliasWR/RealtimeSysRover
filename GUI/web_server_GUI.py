from flask import Flask, render_template
import netifaces

INTERFACE = '{98FF9398-32A3-4162-8A1F-4D66F8420747}'  # GUID for wifi-interface på Martin's bigboi


def get_ip_address():
    # return "192.168.0.100"
    return netifaces.ifaddresses(INTERFACE)[netifaces.AF_INET][0]['addr']


app = Flask(__name__)


@app.route('/')
def index():
    server_ip = get_ip_address()
    return render_template('index.html', server_ip=server_ip)


if __name__ == '__main__':
    app.run(debug=True, host=get_ip_address(), port=5000)
