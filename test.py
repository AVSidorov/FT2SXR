import subprocess
from matplotlib import pyplot as plt
import h5py


def show_count_rate(path):
    f = h5py.File(path, 'r')
    if 'processed_data' in list(f['SXR'].keys()):
        plt.plot(list(f['SXR']['processed_data']['count_rate']['channel00']['times']),
                 list(f['SXR']['processed_data']['count_rate']['channel00']['counts']))
        plt.show()
        f.close()
    else:
        f.close()
        subprocess.call([r"D:\home\projects\SXR\FT2SXR\matlab\pulse_count\MakePulseCount\for_testing\MakePulseCount.exe", path])
        f = h5py.File(path, 'r')
        plt.plot(list(f['SXR']['processed_data']['count_rate']['channel00']['times']),
                 list(f['SXR']['processed_data']['count_rate']['channel00']['counts']))
        plt.show()
        f.close()


if __name__ == '__main__':
    path = input()
    show_count_rate(path)
