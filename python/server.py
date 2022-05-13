
## Copyright (c) 2021 Thakee Nathees.
## Distributed Under The MIT License

## An http server to respond javascript module imports
## with the content type as "application/javascript".
## The default pythons handler (`> python -m http.server 8000`)
## respond javascript files with the content-type "text/plain"
## which is not allowed for modular imports.
##
## Reference: https://stackoverflow.com/a/60638762/17541440

import http.server
import socketserver

HOST = "localhost"
PORT = 8000
  
def main():
  Handler = http.server.SimpleHTTPRequestHandler
  Handler.extensions_map.update({
    ".js": "application/javascript",
  })
  
  httpd = socketserver.TCPServer((HOST, PORT), Handler)
  httpd.serve_forever()

if __name__ == '__main__':
  main()
