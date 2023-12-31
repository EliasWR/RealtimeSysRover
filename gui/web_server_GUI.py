from flask import Flask, render_template
import netifaces

# Add your interface GUID here
INTERFACE = '{98FF9398-32A3-4162-8A1F-4D66F8420747}'  # GUID for wifi-interface on Martin's Computer


# INTERFACE = '{b28e8efd-ddbe-4b84-8074-d3a30bb615f2}' # GUID for wifi-interface on Elias's Computer

def get_ip_address():
    ip_address = 'localhost'
    try:
        ip_address = netifaces.ifaddresses(INTERFACE)[netifaces.AF_INET][0]['addr']
    except ValueError:
        print(f"Interface {INTERFACE} is not valid.")
    except (IndexError, KeyError):
        print(f"No IPv4 address assigned to interface {INTERFACE}.")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
    return ip_address


app = Flask(__name__)


@app.route('/')
def index():
    server_ip = get_ip_address()
    return render_template('index.html', server_ip=server_ip)


if __name__ == '__main__':
    try:
        host_ip = get_ip_address()
        app.run(debug=True, host=host_ip, port=5000)
    except ValueError as e:
        print(f"Error: {e}")
        app.run(debug=True, port=5000)  # Fallback to localhost
