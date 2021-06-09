import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import wave
from thunderfish.eventdetection import threshold_crossings


def load_wave(filepath):
    try:
        wf = wave.open(filepath, 'r')
        nchannels, sampwidth, rate, nframes, comptype, compname = wf.getparams()
        buffer = wf.readframes(nframes)
        if sampwidth == 1:
            dtype = 'u1'
            data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
        else:
            dtype = 'i%d' % sampwidth
            data = np.frombuffer(buffer, dtype=dtype).reshape(-1, nchannels)
        wf.close()
        return data, float(rate)
    except EOFError:
        return None, None


def test_continuity(pathes):
    data, rate = load_wave(pathes[0])
    if not rate:
        return
    samplerate = rate
    first = True
    nchannels = data.shape[1]
    isis = [[] for k in range(nchannels)]
    isis0 = np.zeros((nchannels, len(pathes)))
    isis1 = np.zeros((nchannels, len(pathes)))
    for k, path in enumerate(pathes):
        if not first:
            data, rate = load_wave(path)
        if not rate:
            break
        if data.shape[1] != nchannels:
            break
        first = False
        print(path)
        for c in range(nchannels):
            nup = np.sum(data[:,c] > 1024*16)
            ndown = np.sum(data[:,c] < -1024*16)
            if nup + ndown < 9*len(data)/10 or min(nup, ndown) < len(data)/10:
                continue
            up, down = threshold_crossings(data[:,c], 0)
            isis[c].extend(np.diff(up))
            isis0[c][k] = up[0]
            isis1[c][k] = len(data) - up[-1]
    fig, axs = plt.subplots(2, nchannels//2)
    axs = axs.ravel()
    fig.subplots_adjust(left=0.06, right=0.98, top=0.94, bottom=0.09)
    intervals = isis1[:,:-1] + isis0[:,1:]
    for c in range(nchannels):
        if len(isis[c]) > 10:
            n1, b1 = np.histogram(isis[c], -0.5 + np.arange(np.min(isis[c])-1, np.max(isis[c])+3))
            n2, b2 = np.histogram(intervals[c], -0.5 + np.arange(np.min(intervals[c])-1, np.max(intervals[c])+3))
            n2 *= np.max(n1)/np.max(n2)/2
            period = np.mean(isis[c])/samplerate
            axs[c].bar(b1[:-1], n1, width=1, color='b')
            axs[c].bar(b2[:-1], n2, width=1, color='g')
            axs[c].text(0.95, 0.9, 'period=%.2fms' % (1000.0*period), transform=axs[c].transAxes, ha='right')
            axs[c].text(0.95, 0.8, 'freq=%.1fHz' % (1/period), transform=axs[c].transAxes, ha='right')
            dp = max(1, np.floor(np.log10(max(b1[-1], b2[-1]) - min(b1[0], b2[0]))))
            axs[c].xaxis.set_major_locator(ticker.MultipleLocator(10**dp))
            axs[c].set_xlabel('Inter-pulse samples')
        axs[c].set_title('channel %d' % c)
    fig.tight_layout()
    plt.show()

        
if __name__ == '__main__':
    test_continuity(sys.argv[1:])


