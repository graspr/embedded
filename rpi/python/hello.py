import tornado.httpserver
import tornado.websocket
import tornado.ioloop
import tornado.web
import tornado.options
from tornado.log import enable_pretty_logging

tornado.options.parse_command_line()
enable_pretty_logging()
 
ws = None

class IndexHandler(tornado.web.RequestHandler):
    def get(self):
        print 'WTF!'
        self.render("index.html")

class WSHandler(tornado.websocket.WebSocketHandler):
    def open(self):
        ws = self
        print 'new connection'
        self.write_message("Hello World")
      
    def on_message(self, message):
        print 'message received %s' % message
        ws.write_message('Echo! ' + message);
 
    def on_close(self):
        ws = None
        print 'connection closed'
 
 
application = tornado.web.Application([
    (r'/ws', WSHandler),
    (r'/', IndexHandler),
])
 
 
if __name__ == "__main__":
    # http_server = tornado.httpserver.HTTPServer(application)
    application.listen(80)
    tornado.ioloop.IOLoop.instance().start()
