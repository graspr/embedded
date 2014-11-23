"""
Entry point for Graspr Embedded App
"""
import signal
import sys
import time
from thread import *
from collections import deque
import web
import spi
import mux

PORT = 8080
DQ_MAX_LENGTH = 10000
DATA = deque(maxlen=DQ_MAX_LENGTH)


def run():
    """
    Main runloop
    """
    while True:
        # yield ",".join(mux.read(i))
        read_14 = mux.read(14)
        read_15 = mux.read(15)
        read_16 = mux.read(16)
        yield "{},{:s},{:s},{:s}".format(int(time.time()*1000), read_14, read_15, read_16)


def signal_handler(signal, frame):
    """
    For when we hit CTRL+C!
    """
    print(('End of run: {!s}'.format(time.time())))
    sys.exit(0)


def server(args):
    global DATA
    runner = run()
    sys.stdout.write('===============')
    sys.stdout.flush()
    for i in runner:
        DATA.append(i)
    signal.signal(signal.SIGINT, signal_handler) #handler for keyboard interrupt

def application_setup():
    signal.signal(signal.SIGINT, signal_handler) #handler for keyboard interrupt

def spawn_in_thread():
    global DATA
    start_new_thread(server,(None,))
    start_new_thread(web.run,(DATA, PORT))

if __name__ == "__main__":
    print('Setting up')
    application_setup()
    spi.setup()
    print(('Start of run: {!s}'.format(time.time())))
    print('Channel 14,Channel 15,Channel 16')

    if len(sys.argv) > 1:
        PORT = sys.argv[1]

    spawn_in_thread()
    
    while True:
        if len(DATA) != DQ_MAX_LENGTH:
            toprint = "{}\r".format(len(DATA))
            sys.stdout.write(toprint)
        else:
            sys.stdout.write('FULL! {}\r'.format(str(int(time.time()))))
        sys.stdout.flush()

