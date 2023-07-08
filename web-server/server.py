from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib.parse
import threading
import subprocess

VALID_FREQ = ['8', '32', '48', '96']
DEFAULT_FREQ = '8'
VALID_WORD_LENGTH = ['16', '20', '24', '32']
DEFAULT_WORD_LENGTH = '24'

subprocess.call(['sh','/root/pass_through.sh'])

def c_function():
	subprocess.run(["/root/de1_soc_audio_linux"])

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
        <label for="l_volume">Left channel volume</label><br>
        <input type="range" id="l_volume" name="l_volume" min="0" max="9"><br>
        <label for="r_volume">Right channel volume</label><br>
        <input type="range" id="r_volume" name="r_volume" min="0" max="9"><br><br>
        <fieldset style="width:230px">
            <legend>Please select your input source:</legend>
            <input type="radio" id="sourceMic" name="sorce" value="sourceMic" checked/>
            <label for="contactChoice2">Microphone Input</label><br>
            <input type="radio" id="sourceLine" name="sorce" value="sourceLine" />
            <label for="contactChoice3">Line Input</label>
        </fieldset><br>
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
        l_vol = form_data.get('l_volume', [''])[0]
        r_vol = form_data.get('r_volume', [''])[0]
        source = form_data.get('sorce', [''])[0]
        

        # Check for validity
        if (freq not in VALID_FREQ):
            freq = DEFAULT_FREQ

        # Check for validity
        if (word_length not in VALID_WORD_LENGTH):
            word_length = DEFAULT_WORD_LENGTH

        # 
        if(source == 'sourceMic'):
            input_source = '1'
        else:
            input_source = '2'


        # Write the data to a file
        with open('audio_config.txt', 'w') as file:
            file.write(freq + '\n')
            file.write(word_length + '\n')
            file.write(l_vol + '\n')
            file.write(r_vol + '\n')
            file.write(input_source + '\n')

        # Send a response message
        response_message = 'Configuration was saved'
        self.wfile.write(response_message.encode('utf-8'))

        #For one time use
        #quit()

# Host IP
IP = '10.42.0.2'

def run_server():
    port=8000
    server_address = (IP, port) # Now it's set up as http://localhost:8000/
    httpd = HTTPServer(server_address, RequestHandler)
    print(f'Starting server on port {port}...')
    httpd.serve_forever()

thread2 = threading.Thread(target=c_function)
thread2.start()

thread = threading.Thread(target=run_server)
thread.start()
#run_server(8000)
