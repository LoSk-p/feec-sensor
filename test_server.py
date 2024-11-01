from http.server import SimpleHTTPRequestHandler, HTTPServer

class MyHandler(SimpleHTTPRequestHandler):
    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        print(f"Received POST data: {post_data.decode('utf-8')}")

server_address = ('', 8080)
httpd = HTTPServer(server_address, MyHandler)

print("Starting server on port 8080...")
httpd.serve_forever()