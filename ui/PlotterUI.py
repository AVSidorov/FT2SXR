import sys
from PyQt5 import QtWidgets, QtCore
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg, NavigationToolbar2QT
from matplotlib.figure import Figure
import numpy as np
from ui.PlotterUIDesign import Ui_Plotter


class PlotterWidget (QtWidgets.QMainWindow, Ui_Plotter):
    def __init__(self, parent=None, data_dir=None, x_unit='samples'):
        super().__init__(parent=parent)
        self.setupUi(self)
        self.dir = data_dir
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
        self._static_ax = static_canvas.figure.subplots()

        # Plot will be here
        t = np.linspace(0, 10, 501)
        self._static_ax.plot(t, np.tan(t), ".")

def main():
    app = QtWidgets.QApplication(sys.argv)
    mv = PlotterWidget()
    mv.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()

