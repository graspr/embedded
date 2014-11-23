"""
Constants related to the Raspberry Pi B+ board
"""

HIGH = 1
LOW = 0
OUTPUT = 1
INPUT = 0

CSB = 10
MISO = 13
MOSI = 12
SCKL = 14

SPI_CHANNEL = 0  #on RPI there are two SPI channels, 0 and 1
SPI_CLOCK_SPEED = int((2**19)) #hz == ~2.62mhz
