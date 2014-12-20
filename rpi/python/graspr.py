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

def signal_handler(signal, frame):
    """
    For when we hit CTRL+C!
    """
    print(('End of run: {!s}'.format(time.time())))
    sys.exit(0)

def application_setup():
    signal.signal(signal.SIGINT, signal_handler) #handler for keyboard interrupt

if __name__ == "__main__":
    print('Setting up')
    application_setup()
    spi.setup()
    print(('Start of run: {!s}'.format(time.time())))
    print('Channel 14,Channel 15,Channel 16')

    if len(sys.argv) > 1:
        PORT = sys.argv[1]
    web.run(PORT)
