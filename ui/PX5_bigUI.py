import sys
from PyQt5 import QtWidgets, QtCore
from ui.PX5_bigDesign import Ui_PX5Design
from core.sxr_protocol_pb2 import MainPacket, AdcStatus
from core.sxr_protocol import packet_init


class PX5Widget(QtWidgets.QWidget, Ui_PX5Design):
    channel0 = QtCore.pyqtSignal(bytes)

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)

        # on/off fields
        self.PURE_comboBox.currentTextChanged.connect(self.switch_fld)
        self.PRET_comboBox.currentTextChanged.connect(self.switch_fld)
        self.PREC_comboBox.currentTextChanged.connect(self.switch_fld)
        self.PRET_comboBox.currentTextChanged.connect(self.switch_fld)
        self.SOFF_comboBox.currentTextChanged.connect(self.switch_fld)
        self.TLLD_comboBox.currentTextChanged.connect(self.switch_fld)
        self.PRET_comboBox.currentTextChanged.connect(self.switch_fld)
        self.CUSP_comboBox.currentTextChanged.connect(self.switch_fld)
        self.PAPZ_comboBox.currentTextChanged.connect(self.switch_fld)
        self.INOF_comboBox.currentTextChanged.connect(self.switch_fld)
        self.TECS_comboBox.currentTextChanged.connect(self.switch_fld)
        self.HVSE_comboBox.currentTextChanged.connect(self.switch_fld)
        self.DACO_comboBox.currentTextChanged.connect(self.switch_fld)
        self.AUO1_comboBox.currentTextChanged.connect(self.switch_fld)
        self.AUO2_comboBox.currentTextChanged.connect(self.switch_fld)
        self.GPIN_comboBox.currentTextChanged.connect(self.switch_fld)
        self.PURE_fast_comboBox.currentTextChanged.connect(self.switch_fld)
        self.PRET_comboBox.currentTextChanged.connect(self.switch_fld)

    def switch_fld(self):
        print('hi')

def main():
    app = QtWidgets.QApplication(sys.argv)

    win = QtWidgets.QDialog()
    win.setModal(True)
    ex = PX5Widget(win)
    win.verticalLayout = QtWidgets.QVBoxLayout(win)
    win.verticalLayout.addWidget(ex)
    win.show()

    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
