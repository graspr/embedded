import socket
from thread import *

sock = None
SOCK_HOST = ''
SOCK_PORT = 8080
SOCK_BACKLOG = 3
sock_connection = None
sock_address = None

#Function for handling connections. This will be used to create threads
def clientthread(conn):
    #Sending message to connected client
    conn.send('Welcome to the server. Type something and hit enter\n') #send only takes string
     
    #infinite loop so that function do not terminate and thread do not end.
    while True:
         
        #Receiving from client
        data = conn.recv(1024)
        reply = 'OK...' + data
        if not data: 
            break
     
        conn.sendall(reply)
     
    print('Ended connection with {}'.format(conn))
    #came out of loop
    conn.close()

def setup_socket():
    global sock
    # Create a TCP/IP socket
    try:
        print('Creating socket...'),
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error:
        print 'Failed to create socket'
        sys.exit()
    print('Created.')

    # Bind the socket to the port
    server_address = (SOCK_HOST, SOCK_PORT)
    print('starting up on {} port {}'.format(*server_address))
    try:
        print('Attempting to bind socket...'),
        sock.bind(server_address)
    except socket.error , msg:
        print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
        sys.exit()
    print('Successful bind.')
    # Listen for incoming connections
    print('Now listening for connections on socket.')
    sock.listen(SOCK_BACKLOG)


if __name__ == "__main__":
    setup_socket()
    #now keep talking with the client
    while True:
        #wait to accept a connection - blocking call
        sock_connection, sock_address = sock.accept()
        print('Connected with ' + sock_address[0] + ':' + str(sock_address[1]))
         
        #start new thread takes 1st argument as a function name to be run, second is the tuple of arguments to the function.
        start_new_thread(clientthread, (sock_connection, ))