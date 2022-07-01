import sys
from PyQt5 import QtWidgets, QtGui, QtCore
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg, NavigationToolbar2QT
from matplotlib.figure import Figure
import numpy as np
from ui.PlotterUIDesign import Ui_Plotter
from ui.reader import Reader
from os import path
import gc


class PlotterWidget(QtWidgets.QMainWindow, Ui_Plotter):
    def __init__(self, parent=None, data_file=None, x_unit='samples'):
        super().__init__(parent=parent)
        self.setupUi(self)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        gc.collect(generation=2)
        self.dir = data_file
        self.x_unit = x_unit
        if self.x_unit not in ('samples', 'msec'):
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

        self.x_ax_comboBox.currentTextChanged.connect(self.change_ax)
        self.select_shot_pushButton.clicked.connect(self.select_shot)

        self.static_canvas.figure.dpi = 80.0

        self.make_plot(data_file=self.dir)

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

                for i in range(1, n_plots + 1):
                    globals()[f"ax{i}"] = self.static_canvas.figure.add_subplot(n_plots, 1, i)
                    eval(f'ax{i}').set_title(f'ch {self.reader.meta[i - 1][1]}')
                    if x_unit == 'samples':
                        eval(f'ax{i}').plot(self.reader.data[i - 1])
                    elif x_unit == 'ms':
                        # samples = int(self.reader.meta[0][2])
                        rate = int(self.reader.meta[0][3])
                        eval(f'ax{i}').plot(np.linspace(0, len(self.reader.data[i - 1]) / rate * 1000,
                                                        len(self.reader.data[i - 1]), dtype=np.float64),
                                            self.reader.data[i - 1])

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
                self.change_ax()

    def hideEvent(self, a0: QtGui.QHideEvent) -> None:
        self.static_canvas.figure.clear('all')
        gc.collect(generation=2)
        self.close()

    def closeEvent(self, a0: QtGui.QCloseEvent) -> None:
        self.static_canvas.figure.clear('all')
        gc.collect(generation=2)


def main():
    app = QtWidgets.QApplication(sys.argv)
    mv = PlotterWidget()
    mv.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
