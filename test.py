from matplotlib import pyplot as plt
import plotly.graph_objects as go
import h5py
import numpy as np


def show_signal(path):
    f = h5py.File(path, 'r')
    # plt.plot(np.array(f['SXR']['ADC']['channel00'], dtype=np.int16))
    # plt.show()
    fig = go.Figure()
    fig.add_trace(go.Scatter(y=np.array(f['SXR']['ADC']['channel00'], dtype=np.int16)))
    fig.show()
    f.close()


if __name__ == '__main__':
    show_signal(r'D:\home\projects\SXR\FT2SXR\230720\SXR230720_005.h5')