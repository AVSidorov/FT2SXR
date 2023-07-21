import sys
from PyQt5 import QtWidgets
from ui.GSA.GSAUIDesign import Ui_GSAWidgetDesign


class GSAWidget(QtWidgets.QWidget, Ui_GSAWidgetDesign):
    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)

        # hardware
        amp_list = (('0', '22', '58', '78', '114', '134', '167', '187', '223'), 'mV')
        edge = ('24', 'ns')
        freq = ('626', 'Hz')

        # GSA initial values
        self.amplitude = None

        # signals
        self.amp_comboBox.currentTextChanged.connect(self.setamplitude)

        self.edge_val_label.setText(edge[0])
        self.edge_type_label.setText(edge[1])
        self.freq_val_label.setText(freq[0])
        self.freq_type_label.setText(freq[1])
        for i in amp_list[0]:
            self.amp_comboBox.addItem(i)
        self.amp_type_label.setText(amp_list[1])

    def setamplitude(self):
        self.amplitude = int(self.amp_comboBox.currentText())


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = GSAWidget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()