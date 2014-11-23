"""
Raspberry Pi B+ SPI communication code
"""
import wiringpi2 as wpi
from constants import rpi as RPI
from constants import mux as MUX

class SPIError(Exception):
    """
    SPI Error
    """
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return str(self.value)

def setup(SPI_SPEED=RPI.SPI_CLOCK_SPEED):
    """
    Perform initial board and SPI setup
    """
    
    wpi.wiringPiSetup()
    for pin in MUX.MUX_PINS:
        wpi.pinMode(pin, RPI.OUTPUT)

    wpi.pinMode(MUX.EN, RPI.OUTPUT)
    wpi.digitalWrite(MUX.EN, RPI.HIGH) #EN should always be high.
    wpi.pinMode(RPI.CSB, RPI.OUTPUT)
    wpi.wiringPiSPISetup(0, SPI_SPEED)
