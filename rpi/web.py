import socket
from thread import *
import sys
import time

sock = None
SOCK_HOST = ''
SOCK_PORT = 8084
SOCK_BACKLOG = 3
sock_connection = None
sock_address = None



def signal_handler(signal, frame):
    """
    For when we hit CTRL+C!
    """
    global sock
    print('Closing socket.')
    sock.close()
    sys.exit(0)

#Function for handling connections. This will be used to create threads
def clientthread(conn, DATA):
    #Sending message to connected client
    conn.send('START OF RUN:\n')
    conn.send('{},channel 14,15,16,DEC(14),DEC(15),DEC(16)\n'.format(int(time.time()*1000))) #send only takes string
     
    #infinite loop so that function do not terminate and thread do not end.
    while True:
        # print 'DATA %s' % DATA
        if (len(DATA) == 0):
            reply = "NO_DATA"
            # time.sleep(0.2)
            continue
        reply = '{}\n'.format(DATA[0])
        DATA.popleft()
        # if not data: 
        #     break
        try:
            # print 'LOL %s' % conn.send(reply)
            pass
        except socket.error:
            print 'SOCKET CLOSED! closing connection'
            conn.close()
            break


     
    print('Ended connection with {}'.format(conn))
    #came out of loop
    conn.close()

def setup_socket(PORT=None):
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
    server_address = (SOCK_HOST, PORT or SOCK_PORT)
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
    signal.signal(signal.SIGINT, signal_handler) #handler for keyboard interrupt

def run(DATA, PORT=None):
    if (PORT):
        PORT = int(PORT)
    setup_socket(PORT)
    #now keep talking with the client
    while True:
        #wait to accept a connection - blocking call
        sock_connection, sock_address = sock.accept()
        sys.stdout.write('Connected with ' + sock_address[0] + ':' + str(sock_address[1]))
         
        #start new thread takes 1st argument as a function name to be run, second is the tuple of arguments to the function.
        start_new_thread(clientthread, (sock_connection, DATA))


if __name__ == "__main__":
    run(None)
