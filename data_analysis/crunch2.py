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
# pylint: disable-msg=C0103


if __name__ == '__main__':
    print 'Starting...'
    f = open('data.log', 'r')
    raw_values = f.readlines()
    raw_values = [int(n) for n in raw_values]
    print raw_values
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