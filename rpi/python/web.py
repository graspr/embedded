import socket
from thread import *
import sys
import time
import mux

sock = None
SOCK_HOST = ''
SOCK_PORT = 8081
SOCK_BACKLOG = 3
sock_connection = None
sock_address = None

#Function for handling connections. This will be used to create threads
def clientthread(conn):
    #Sending message to connected client
    # conn.send('START OF RUN:\n')
    # conn.send('{},channel 14,15,16\n'.format(int(time.time()*1000))) #send only takes string
    t = int(time.time()*1000)
    diff = 0
    i = 0
    COUNTS = 100
    mux.switch_to_channel(16)
    #infinite loop so that function do not terminate and thread does not end.
    while True:
        if (i == COUNTS):
            diff = int(time.time()*1000) - t
            rps = ((COUNTS * 1.0)/(diff/1000.0)) * 16
            wps = rps/16
            reply = "TIMEDELTA IS: %s milliseconds, CHANNEL_READS/SEC = %s, WRITES_TO_CLIENT/SEC = %s" % (diff, rps, wps)
            t = int(time.time()*1000)
            i = 0
            print(reply)
            continue
        i = i+1
        try:
            
            # data = ",".join([mux.read(1), mux.read(2), mux.read(3), mux.read(4),\
            #                   mux.read(5), mux.read(6), mux.read(7),\
            #                   mux.read(8), \
            #                   mux.read(9), mux.read(10), mux.read(11),\
            #                   mux.read(12), mux.read(13), mux.read(14), \
            #                   mux.read(15), mux.read(16)])
            data = ",".join(['1', '2', '3', '4',\
                              '5', '6', '7',\
                              '8', \
                              '9', '10', '11',\
                              '12', '13', '14', \
                              '16', mux.read(16)])
            print 'DATA: %s' % data
            conn.send('%s\n' % data) #35.198873636 reads/sec
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

def run(PORT=None):
    try:
        if (PORT):
            PORT = int(PORT)
        setup_socket(PORT)
        #now keep talking with the client
        while True:
            #wait to accept a connection - blocking call
            sock_connection, sock_address = sock.accept()
            print('Connection acquired!')
            sys.stdout.write('Connected with ' + sock_address[0] + ':' + str(sock_address[1]))
             
            clientthread(sock_connection)
    except Exception:
        import traceback
        sys.stderr.write( traceback.format_exc())
        sys.stderr.flush()

if __name__ == "__main__":
    run('8181')
