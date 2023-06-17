from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib.parse

VALID_FREQ = ['8', '32', '48', '96']
DEFAULT_FREQ = '48'
VALID_WORD_LENGTH = ['16', '20', '24', '32']
DEFAULT_WORD_LENGTH = '24'

class RequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # Set response status code
        self.send_response(200)
        self.end_headers()

        # Send the HTML form to the client
        response_message = '''
        <html>
        <head>
        <title>Audio configuration</title>
        </head>
        <body>
        <h1>Audio configuration</h1>
        <form method="POST">
        <label for="freq">Enter the sample rate in kHz (8kHz, 32kHz, 48kHz, 96kHz) :</label><br>
        <input type="text" id="freq" name="freq"><br><br>
        <label for="word_length">Enter the word Length: 16/20/24/32 bit :</label><br>
        <input type="text" id="word_length" name="word_length"><br><br>
        <input type="submit" value="Submit">
        </form>
        </body>
        </html>
        '''

        self.wfile.write(response_message.encode('utf-8'))

    def do_POST(self):
        # Set response status code
        self.send_response(200)
        self.end_headers()

        # Get the length of the incoming request body
        content_length = int(self.headers['Content-Length'])

        # Read the request body data
        post_data = self.rfile.read(content_length).decode('utf-8')

        # Parse the form data
        form_data = urllib.parse.parse_qs(post_data)
        freq = form_data.get('freq', [''])[0]
        word_length = form_data.get('word_length', [''])[0]

        # Check for validity
        if (freq not in VALID_FREQ):
            freq = DEFAULT_FREQ

        # Check for validity
        if (word_length not in VALID_WORD_LENGTH):
            word_length = DEFAULT_WORD_LENGTH

        # Write the data to a file
        with open('audio_config.txt', 'w') as file:
            file.write(freq + '\n')
            file.write(word_length + '\n')

        # Send a response message
        response_message = 'Configuration was saved'
        self.wfile.write(response_message.encode('utf-8'))

        #For one time use
        #quit()

# Host IP
IP = '192.168.23.100'

def run_server(port):
    server_address = (IP, port) # Now it's set up as http://localhost:8000/
    httpd = HTTPServer(server_address, RequestHandler)
    print(f'Starting server on port {port}...')
    httpd.serve_forever()

run_server(8000)
