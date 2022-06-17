import csv
import sys
import os
from PyQt5 import QtWidgets
from ui.AmplifierUIDesign import Ui_AmplifierWidgetDesign
from core.sxr_protocol_pb2 import MainPacket, AmpStatus, SystemStatus, Commands
from core.sxr_protocol import packet_init
from PyQt5 import QtWidgets, QtCore


class AmplifierWidget(QtWidgets.QWidget, Ui_AmplifierWidgetDesign):
    channel0 = QtCore.pyqtSignal(bytes)

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)

        # Amplifier initial values
        self.gainA = 0.0
        self.gainB = 0.0
        self.decay = 0.0
        self.switch_state = 0b0000
        self.status = AmpStatus()
        self.address = 14

        last_file = {}
        with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'amp_last.csv'), newline='') as file:
            cal = csv.DictReader(file, delimiter=',')
            for i in cal:
                last_file = i
        self.gainA = float(last_file['gainA'])
        self.gainB = float(last_file['gainB'])
        self.switch_state = bin(int(last_file['switch_state']))
        print(self.switch_state)

        # signals
        self.gainA_doubleSpinBox.valueChanged.connect(self.setgainA)
        self.gainB_doubleSpinBox.valueChanged.connect(self.setgainB)
        self.time45_checkBox.stateChanged.connect(self.setdecay)
        self.time9_checkBox.stateChanged.connect(self.setdecay)
        self.time13_checkBox.stateChanged.connect(self.setdecay)
        self.time17_checkBox.stateChanged.connect(self.setdecay)

        cal_file = []
        with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'amp_cal.csv'), newline='') as file:
            cal = csv.DictReader(file, delimiter=';')
            for i in cal:
                cal_file.append(i)
        self.cal_textBrowser.setText(self.make_table(cal_file))

        self.setdecay()

        self.status2ui()
        self.ui2status()

    def make_table(self, cal_file):
        table = '<!DOCTYPE HTML>' \
                '<html>' \
                '<head>' \
                '<meta charset="utf-8">' \
                '<title>Калибровка</title>' \
                '</head>'

        cal_file = sorted(cal_file, key=lambda d: f'{d["Channel"]}{d["Config"]}')

        prev = {'Channel': '', 'Config': ''}
        n = 10
        for i in cal_file:
            if i['Channel'] != prev['Channel'] or i['Config'] != prev['Config']:
                table += '</table></body>'

                table += '<body><table border="1" align="center" width="160">'
                table += f'<caption>{i["Channel"]} ({i["Config"]})</caption>'
                table += f'<tr>' \
                         f'<th>{"Limb"}</th>' \
                         f'<th>{"Amp"}</th>' \
                         f'</tr>'
                table += f'<tr><td>{i["Limb"]}</td><td>{i["Amp"]}</td></tr>'
            elif i['Channel'] == prev['Channel'] and i['Config'] == prev['Config']:
                table += f'<tr><td>{i["Limb"]}</td><td>{i["Amp"]}</td></tr>'
            prev = i
        table.replace('</table></body>', '', 1)
        table += '</html>'
        return table

    def setgainA(self):
        self.gainA = round(self.gainA_doubleSpinBox.value(), 2)
        self.ui2status()

    def setgainB(self):
        self.gainB = round(self.gainB_doubleSpinBox.value(), 2)
        self.ui2status()

    def setdecay(self):
        self.time45_checkBox.setDisabled(True)
        self.time9_checkBox.setDisabled(True)
        self.time13_checkBox.setDisabled(True)
        self.time17_checkBox.setDisabled(True)

        c = 1e-8  # F
        r0 = 30037.8  # Ohm (then all switches off)
        r45 = 206.21  # Ohm 205
        r9 = 415.27  # Ohm 412
        r13 = 603.54  # Ohm 619
        r17 = 794.16  # Ohm 825
        t09 = 0.105361  # RC
        t01 = 2.30259  # RC

        new_state = 0b0000
        if self.time45_checkBox.checkState():
            new_state |= 0b0001
        if self.time9_checkBox.checkState():
            new_state |= 0b0010
        if self.time13_checkBox.checkState():
            new_state |= 0b0100
        if self.time17_checkBox.checkState():
            new_state |= 0b1000
        self.switch_state = new_state

        r = (1/r0 + int(new_state & 0b0001)/r45 + int(new_state >> 1 & 0b0001)/r9
             + int(new_state >> 2 & 0b0001)/r13 + int(new_state >> 3 & 0b0001)/r17)**(-1)
        t = (t01-t09) * c * r * 1e6  # ms
        t = round(t, 1)
        self.decay = t
        self.sum_time_label.setText(str(t))

        self.time45_checkBox.setEnabled(True)
        self.time9_checkBox.setEnabled(True)
        self.time13_checkBox.setEnabled(True)
        self.time17_checkBox.setEnabled(True)

        self.ui2status()

    def status2ui(self):
        self.gainA_doubleSpinBox.setValue(self.gainA)
        self.gainB_doubleSpinBox.setValue(self.gainB)

        state = bin(int(self.switch_state))
        # if state

    def ui2status(self):
        self.status.gainA = self.gainA
        self.status.gainB = self.gainB
        self.status.tail = self.switch_state

        request = packet_init(SystemStatus.AMP, self.address)
        request.command = Commands.SET
        if self.status.IsInitialized():
            request.data = self.status.SerializeToString()
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

        with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'amp_last.csv'), 'w', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(('gainA', 'gainB', 'switch_state'))
            writer.writerow((self.gainA, self.gainB, self.switch_state))

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        request = MainPacket()
        request.ParseFromString(data)
        if request.sender == SystemStatus.AMP:
            if request.command in (Commands.STATUS ^ 0xFFFFFFFF, Commands.SET ^ 0xFFFFFFFF):
                self.blockSignals(True)
                self.status.ParseFromString(request.data)
                self.status2ui()
                self.blockSignals(False)


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = AmplifierWidget()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
