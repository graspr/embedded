"""
Quick script to get mean mode variance
"""
import os
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab
import scipy
import scipy.fftpack
import pylab
import sys
# pylint: disable-msg=C0103


if __name__ == '__main__':
    print 'Starting...'
    f = open(sys.argv[1], 'r')
    f.readline()
    f.readline()
    f.readline()
    f.readline()
    raw_values = []
    for row in f:
        try:
            cols = row.split(',')
            timestamp = cols[0]
            bin1 = cols[1]
            bin2 = cols[2]
            bin3 = cols[3]
            dec1 = cols[4]
            dec2 = cols[5]
            dec3 = cols[6]
            # binary = ''.join(row.split(','))
            dec = int(bin1, 2)
            raw_values.append(dec)
            # print '.',
        except ValueError as exception:
            pass
            # print 'Wtf: %s, %s' % (binary, exception)

    f.close()
    values = np.array(raw_values)
    mean = np.mean(values)
    variance = np.var(values)
    print 'Done. Number of samples: %s' % len(raw_values)
    print 'Max: %s' % np.amax(values)
    print 'Min: %s' % np.amin(values)

    print 'Mean: %s' % mean
    print 'Standard dev: %s' % np.std(values)
    print 'Variance: %s' % variance

    # sigma = np.sqrt(variance)
    # x = np.linspace(60000,66000,100)
    # plt.plot(x,mlab.normpdf(x,mean,sigma))
    # plt.show()

    FFT = abs(scipy.fft(values))
    t = np.arange(len(values)) / 580.0
    freqs = scipy.fftpack.fftfreq(values.size, t[1]-t[0])

    pylab.subplot(211)
    pylab.plot(t, values)
    pylab.subplot(212)
    pylab.plot(freqs,20*scipy.log10(FFT),'x')
    pylab.show()