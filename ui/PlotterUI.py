import sys
from PyQt5 import QtWidgets, QtCore
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg, NavigationToolbar2QT
from matplotlib.figure import Figure
import numpy as np
from ui.PlotterUIDesign import Ui_Plotter
from ui.reader import Reader


class PlotterWidget (QtWidgets.QMainWindow, Ui_Plotter):
    def __init__(self, parent=None, data_file=None, x_unit='samples'):
        super().__init__(parent=parent)
        self.setupUi(self)
        self.dir = data_file
        self.x_unit = x_unit
        if self.x_unit not in ('samples', 'msec'):
            self.x_unit = 'samples'

        self._main = QtWidgets.QWidget()
        self.setCentralWidget(self._main)
        layout = QtWidgets.QVBoxLayout(self._main)

        static_canvas = FigureCanvasQTAgg(Figure(tight_layout=True))

        layout.addWidget(static_canvas)
        layout.addWidget(self.shot_groupBox)
        layout.addWidget(self.axis_groupBox)

        self.addToolBar(NavigationToolbar2QT(static_canvas, self))

        # Plot will be here
        # t = np.linspace(0, 10, 501)
        # self._static_ax.plot(t, np.tan(t), ".")

        reader = Reader()
        reader.read(data_file)
        if len(reader.meta) == len(reader.data):
            n_plots = len(reader.meta)
            for i in range(1, n_plots+1):
                globals()[f"ax{i}"] = static_canvas.figure.add_subplot(n_plots, 1, i)
                eval(f'ax{i}').set_title(f'ch {reader.meta[i-1][1]}')
                eval(f'ax{i}').plot(reader.data[i-1])








def main():
    app = QtWidgets.QApplication(sys.argv)
    mv = PlotterWidget()
    mv.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()

