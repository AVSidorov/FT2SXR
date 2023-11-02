import matplotlib
matplotlib.use('Qt5Agg')
matplotlib.rcParams['path.simplify'] = True
matplotlib.rcParams['path.simplify_threshold'] = 1.0
matplotlib.rcParams['agg.path.chunksize'] = 50000
matplotlib.rcParams["legend.loc"] = 'upper right'
import sys
from PyQt5 import QtWidgets, QtGui, QtCore
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg, NavigationToolbar2QT
from matplotlib.figure import Figure
import matplotlib.pyplot as plt
import numpy as np
from ui.PLOTTER.PlotterUIDesign import Ui_Plotter
from ui.PLOTTER.reader import Reader
# from PlotterUIDesign import Ui_Plotter
# from reader import Reader
from os import path
import gc
from silx.math import medfilt1d
from silx.math.fit import savitsky_golay
import scipy.signal as signal
from numba import njit
import os
import time
import configparser


class PlotterWidget(QtWidgets.QMainWindow, Ui_Plotter):
    def __init__(self, parent=None, data_file=None, x_unit='ms'):
        curdir = os.getcwd()
        os.chdir(os.path.join(curdir, 'ui', 'PLOTTER'))
        super().__init__(parent=parent)
        self.setupUi(self)
        config = configparser.ConfigParser()
        config.read('settings.ini')
        width = QtWidgets.QApplication.desktop().screenGeometry().width()
        height = QtWidgets.QApplication.desktop().screenGeometry().height()
        # if int(config['plotter']['position_x']) < width and int(config['plotter']['position_y']) < height:
        self.move(int(config['plotter']['position_x']), int(config['plotter']['position_y']))
        del config
        os.chdir(curdir)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        gc.collect(generation=2)
        self.setWindowTitle('SXR plotter')
        self.dir = data_file
        self.x_unit = x_unit
        if self.x_unit not in ('samples', 'ms'):
            self.x_unit = 'samples'
        self.reader = Reader()

        self.statusbar = self.statusBar()
        self.statusbar.show()

        self._main = QtWidgets.QWidget()
        self.setCentralWidget(self._main)
        Hlayout = QtWidgets.QHBoxLayout(self._main)

        self.static_canvas = FigureCanvasQTAgg(Figure(tight_layout=True))
        self.addToolBar(NavigationToolbar2QT(self.static_canvas, self))

        Hlayout.addWidget(self.static_canvas)
        Hlayout.addWidget(self.shot_groupBox)

        self.static_canvas.figure.dpi = 80.0
        self.x_ax_comboBox.setCurrentText(x_unit)
        self.make_plot(data_file=self.dir, x_unit=x_unit, new=True)

        self.x_ax_comboBox.currentIndexChanged.connect(self.change_ax)
        self.select_shot_pushButton.clicked.connect(self.select_shot)
        self.count_rate_pushButton.clicked.connect(self.count_rate)
        self.rms_pushButton.clicked.connect(self.rms)

        gc.collect(generation=2)

    def make_plot(self, data_file=None, x_unit='samples', new=True):
        self.static_canvas.figure.clear('all')
        gc.collect(generation=2)
        if new is True:
            if data_file is not None:
                self.reader.clear()
                self.reader.read(data_file)

        if not (isinstance(self.reader.meta, type(None)) or isinstance(self.reader.data, type(None))):
            if len(self.reader.meta) == len(self.reader.data):
                n_plots = len(self.reader.meta)
                if new is True:
                    lable = str(self.reader.meta[0][0])
                    samples = str(round(int(self.reader.meta[0][2]) / 1e6, 2)) + 'Msps'
                    rate = str(int(int(self.reader.meta[0][3]) / 1e6)) + 'MHz'
                    time_ms = str(int(int(self.reader.meta[0][2]) / int(self.reader.meta[0][3]) * 1e3)) + 'ms'
                    self.name_val_label.setText(lable)
                    self.samples_val_label.setText(samples)
                    self.rate_val_label.setText(rate)
                    self.channels_val_label.setText(str(n_plots))
                    self.time_val_label.setText(time_ms)
                    self.date_label.setText(time.ctime(self.reader.stat.st_ctime))
                    self.count_rate_comboBox.clear()
                    self.rms_comboBox.clear()
                    for i in range(n_plots - 1, -1, -1):
                        self.count_rate_comboBox.addItem(
                            f'Graph №{self.reader.meta[i - 1][1]} ({self.reader.meta[i - 1][4]})')
                        self.rms_comboBox.addItem(f'Graph №{self.reader.meta[i - 1][1]} ({self.reader.meta[i - 1][4]})')

                for i in range(1, n_plots + 1):
                    globals()[f"ax{i}"] = self.static_canvas.figure.add_subplot(n_plots, 1, i)
                    eval(f'ax{i}').set_title(f'Graph №{self.reader.meta[i - 1][1]} ({self.reader.meta[i - 1][4]})')
                    if x_unit == 'samples':
                        eval(f'ax{i}').plot(self.reader.data[i - 1])
                        eval(f'ax{i}').set_xlabel('Number of sample')
                        eval(f'ax{i}').set_ylabel('Signal, bits')
                    elif x_unit == 'ms':
                        # samples = int(self.reader.meta[0][2])
                        rate = int(self.reader.meta[0][3])
                        eval(f'ax{i}').plot(np.linspace(0, len(self.reader.data[i - 1]) / rate * 1000,
                                                        len(self.reader.data[i - 1]), dtype=np.float32),
                                            self.reader.data[i - 1])
                        eval(f'ax{i}').set_xlabel('Time, ms')
                        eval(f'ax{i}').set_ylabel('Signal, bits')

                self.static_canvas.draw()
                self.statusbar.showMessage('Plotted', 10000)
            else:
                self.statusbar.showMessage('Can\'t plot', 10000)
        else:
            self.statusbar.showMessage('Can\'t plot', 10000)

    def change_ax(self):
        self.x_unit = self.x_ax_comboBox.currentText().lower().strip()
        self.make_plot(x_unit=self.x_unit, new=False)

    def select_shot(self):
        data_file = QtWidgets.QFileDialog.getOpenFileName(self,
                                                          "Select one or more files to open",
                                                          ".",
                                                          "SXR Files (*.h5 *.bin)")[0]
        if data_file != '':
            if path.isabs(data_file):
                self.dir = data_file
                self.make_plot(data_file=self.dir)

    def rms(self):
        if not (isinstance(self.reader.meta, type(None)) or isinstance(self.reader.data, type(None))):
            if len(self.reader.meta) == len(self.reader.data):
                lable = str(self.reader.meta[0][0])
                signal_index = self.rms_comboBox.currentIndex()

                time_ms = int(int(self.reader.meta[0][2]) / int(self.reader.meta[0][3]) * 1e3)
                rate = int(self.reader.meta[0][3])
                ms_per_smpl = 1 / rate * 1e3

                rms_time_ms = self.rms_window_doubleSpinBox.value()
                rms_time_smpls = int(rms_time_ms * rate / 1e3)
                rms = []
                times = []

                for i in range(int(time_ms / rms_time_ms)):
                    rms.append(np.std(self.reader.data[signal_index][i * rms_time_smpls:(i + 1) * rms_time_smpls]))
                    times.append((i + 0.5) * rms_time_smpls * ms_per_smpl)

                plt.close()
                plt.title(f'RMS in {self.reader.meta[signal_index][4]} (file {lable})')
                plt.xlabel('Time, ms')
                plt.grid()
                plt.plot(times, rms)
                plt.show()
                gc.collect(generation=2)

    def count_rate(self):
        if not (isinstance(self.reader.meta, type(None)) or isinstance(self.reader.data, type(None))):
            if len(self.reader.meta) == len(self.reader.data):
                lable = str(self.reader.meta[0][0])
                signal_index = self.count_rate_comboBox.currentIndex()

                time_ms = int(int(self.reader.meta[0][2]) / int(self.reader.meta[0][3]) * 1e3)
                rate = int(self.reader.meta[0][3])
                ms_per_smpl = 1 / rate * 1e3

                count_time_ms = self.count_rate_window_doubleSpinBox.value()
                count_time_smpls = int(count_time_ms * rate / 1e3)

                sig = medfilt1d(self.reader.data[signal_index], kernel_size=5)
                sig = savitsky_golay(sig, npoints=40)
                maxs = signal.find_peaks(sig, distance=10)[0]
                prominences, mins, _ = signal.peak_prominences(sig, maxs)

                threshold = self.threshold_spinBox.value()

                @njit
                def _check_and_count():
                    real_maxs = []
                    # real_mins = []
                    for i in range(len(maxs)):
                        if prominences[i] > threshold:
                            real_maxs.append(maxs[i])
                            # real_mins.append(mins[i])
                    real_maxs = np.array(real_maxs)
                    # real_mins = np.array(real_mins)

                    counts = [0.0 for i in range(int(time_ms / count_time_ms))]
                    times = [(i + 0.5) * count_time_smpls * ms_per_smpl for i in range(int(time_ms / count_time_ms))]

                    for i in real_maxs:
                        counts[int(i*ms_per_smpl/count_time_ms)] += 1.0
                    for i in range(len(counts)):
                        counts[i] = counts[i] / (count_time_ms * 1000.0)
                    # for i in range(int(time_ms / count_time_ms)):
                        # counts.append(np.sum(
                        #     np.in1d(list(range(i * count_time_smpls, (i + 1) * count_time_smpls)), real_maxs)) / (
                        #                           count_time_ms * 1e3))
                        # times.append((i + 0.5) * count_time_smpls * ms_per_smpl)
                    return counts, times

                counts, times = _check_and_count()

                plt.close()
                plt.title(f'Count rate in {self.reader.meta[signal_index][4]} (file {lable})')
                plt.ylabel('Millions per second')
                plt.xlabel('Time, ms')
                plt.grid()
                plt.plot(times, counts)
                plt.show()
                gc.collect(generation=2)

    # def hideEvent(self, a0: QtGui.QHideEvent) -> None:
    #     self.static_canvas.figure.clear('all')
    #     gc.collect(generation=2)
    #     self.close()

    def closeEvent(self, a0: QtGui.QCloseEvent) -> None:
        curdir = os.getcwd()
        os.chdir(os.path.join(curdir, 'ui', 'PLOTTER'))
        config = configparser.ConfigParser()
        config.read('settings.ini')
        config['plotter']['position_x'] = str(self.x())
        config['plotter']['position_y'] = str(self.y())
        with open('settings.ini', 'w') as f:
            config.write(f)
        os.chdir(curdir)
        self.static_canvas.figure.clear('all')
        gc.collect(generation=2)


def main(data_file=None, x_unit='ms'):
    try:
        # Включите в блок try/except, если вы также нацелены на Mac/Linux
        from PyQt5.QtWinExtras import QtWin  # !!!
        myappid = 'FT2SXRPLOTTER'  # !!!
        QtWin.setCurrentProcessExplicitAppUserModelID(myappid)  # !!!
    except ImportError:
        pass

    app = QtWidgets.QApplication(sys.argv)
    app.setWindowIcon(QtGui.QIcon(os.path.join(os.getcwd(), 'style', 'icons', 'icons8-quadrant-light-48.png')))
    mv = PlotterWidget(data_file=data_file, x_unit=x_unit)
    mv.setWindowIcon(QtGui.QIcon(os.path.join(os.getcwd(), 'style', 'icons', 'icons8-quadrant-light-48.png')))
    mv.showMaximized()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main(data_file='')
