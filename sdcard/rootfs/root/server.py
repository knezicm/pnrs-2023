from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib.parse
import subprocess

VALID_FREQ = ['8', '32', '48', '96']
DEFAULT_FREQ = '8'
VALID_WORD_LENGTH = ['16', '20', '24', '32']
DEFAULT_WORD_LENGTH = '24'

I2C_VOLUMEN_L = "i2cset -y 0 0x1a 0x04 "
I2C_VOLUMEN_R = "i2cset -y 0 0x1a 0x06 "
VOL_CON = ["0x30","0x42","0x54","0x66","0x79","0x7a","0x7b","0x7c","0x7d","0x7f"]

SOURCE_IN = ["i2cset -y 0 0x1a 0x00 0x97\ni2cset -y 0 0x1a 0x02 0x97\ni2cset -y 0 0x1a 0x08 0x14",
             "i2cset -y 0 0x1a 0x00 0x17\ni2cset -y 0 0x1a 0x02 0x17\ni2cset -y 0 0x1a 0x08 0x12"] 

# Send the HTML form to the client
response_message = '''
           <!DOCTYPE html>
                <html>
                <head>
                </head>
                <body>
                    <h1>Audio configuration</h1>
                    <form method="POST">
                        <label for="l_volume">Left channel volume</label><br>
                        <input type="range" id="l_volume" name="l_volume" min="0" max="9"><br>
                        <label for="r_volume">Right channel volume</label><br>
                        <input type="range" id="r_volume" name="r_volume" min="0" max="9"><br><br>
                        <input type="submit" value="Submit"><br><br>
                        <a href="https://ibb.co/XkkFF6j"><img src="https://i.ibb.co/rff55qF/IMG-0075.jpg" alt="IMG-0075" border="0"></a>
                    </form>

                </body>
                </html>
            '''

class RequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # Set response status code
        self.send_response(200)
        self.end_headers()


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
        l_vol = form_data.get('l_volume', [''])[0]
        r_vol = form_data.get('r_volume', [''])[0]
        

        left_vol = I2C_VOLUMEN_L + VOL_CON[int(l_vol)]
        right_vol = I2C_VOLUMEN_R + VOL_CON[int(r_vol)]

        with open('/root/audio_config.sh', 'w') as file:
            file.write(left_vol + '\n')
            file.write(right_vol + '\n')
            file.close()
            
        subprocess.call(['sh','/root/audio_config.sh'])

        # Send a response message
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

run_server()