"""
Constants related to the MUX (ADG 706/707)
"""
CHANNELS = {
    1: ['0', '0', '0', '0'],
    2: ['0', '0', '0', '1'],
    3: ['0', '0', '1', '0'],
    4: ['0', '0', '1', '1'],
    5: ['0', '1', '0', '0'],
    6: ['0', '1', '0', '1'],
    7: ['0', '1', '1', '0'],
    8: ['0', '1', '1', '1'],
    9: ['1', '0', '0', '0'],
    10: ['1', '0', '0', '1'],
    11: ['1', '0', '1', '0'],
    12: ['1', '0', '1', '1'],
    13: ['1', '1', '0', '0'],
    14: ['1', '1', '0', '1'],
    15: ['1', '1', '1', '0'],
    16: ['1', '1', '1', '1'],
}


A0 = 9 #mapping from mux chan to RPI pin numbers
A1 = 7
A2 = 5
A3 = 4
EN = 8
MUX_PINS = [A3, A2, A1, A0]
