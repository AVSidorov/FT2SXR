import gc
import sys
import os
from ui.JOURNAL.ShotSettingsUIDesign import Ui_shotSettings
from core.sxr_protocol_pb2 import MainPacket, SystemStatus, Commands, JournalStatus
from core.sxr_protocol import packet_init
from PyQt5 import QtWidgets, QtCore, QtGui
import datetime
import time
from core.fileutils import today_dir


class ShotSettings(QtWidgets.QWidget, Ui_shotSettings):
    channel0 = QtCore.pyqtSignal(bytes)

    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.setupUi(self)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        self.address = 23
        self.status = JournalStatus()

        self.sxr_number_spinBox.valueChanged.connect(self.set_number)
        self.file_name_lineEdit.textChanged.connect(self.set_file_name)
        self.shot_number_spinBox.valueChanged.connect(self.set_shot)
        self.comment_textEdit.textChanged.connect(self.set_comment)
        self.day_textEdit.textChanged.connect(self.set_daycomment)
        self.install_pushButton.clicked.connect(self.install_settings)
        self.return_pushButton.clicked.connect(self.return_settings)

        self.notification_label.hide()

        # self.sxr_number_spinBox.setValue(0)
        # self.ui2status()

    def set_number(self):
        self.status.SXRshot = self.sxr_number_spinBox.value()

        timestamp = time.time()
        curtime = datetime.datetime.fromtimestamp(timestamp)
        file = f'SXR{curtime.year - 2000:02d}{curtime.month:02d}{curtime.day:02d}_{str(self.status.SXRshot).zfill(3)}'
        self.file_name_lineEdit.setText(file)

    def set_file_name(self):
        name = self.file_name_lineEdit.text()
        if name + '.h5' in os.listdir(today_dir()):
            self.file_name_lineEdit.setStyleSheet('color: rgb(255, 0, 0)')
            self.notification_label.show()
            self.install_pushButton.setDisabled(True)
        else:
            self.file_name_lineEdit.setStyleSheet('color: rgb(0, 0, 0)')
            self.notification_label.hide()
            self.install_pushButton.setEnabled(True)
        self.status.filename = name

    def set_shot(self):
        self.status.TOKAMAKshot = self.shot_number_spinBox.value()

    def set_comment(self):
        self.status.comment = self.comment_textEdit.toPlainText()

    def set_daycomment(self):
        self.status.daycomment = self.day_textEdit.toPlainText()

    def ui2status(self):
        self.set_number()
        self.set_shot()
        self.set_file_name()
        self.set_comment()
        self.set_daycomment()

    def status2ui(self):
        self.sxr_number_spinBox.blockSignals(True)
        self.sxr_number_spinBox.setValue(self.status.SXRshot)
        self.sxr_number_spinBox.blockSignals(False)

        self.file_name_lineEdit.setText(self.status.filename)
        self.shot_number_spinBox.setValue(self.status.TOKAMAKshot)
        self.comment_textEdit.setText(self.status.comment)
        self.day_textEdit.setText(self.status.daycomment)

    def install_settings(self):
        request = packet_init(SystemStatus.JOURNAL, self.address)
        request.command = Commands.SET
        if self.status.IsInitialized():
            request.data = self.status.SerializeToString()
        if request.IsInitialized():
            self.channel0.emit(request.SerializeToString())

    def return_settings(self):
        request = packet_init(SystemStatus.JOURNAL, self.address)
        request.command = Commands.STATUS
        self.channel0.emit(request.SerializeToString())

    def next_shot(self):
        self.shot_number_spinBox.setValue(self.shot_number_spinBox.value() + 1)
        self.sxr_number_spinBox.setValue(self.sxr_number_spinBox.value() + 1)
        self.comment_textEdit.setText('')
        self.install_settings()

    @QtCore.pyqtSlot(bytes)
    def channel0_slot(self, data: bytes):
        request = MainPacket()
        request.ParseFromString(data)
        if request.sender == SystemStatus.JOURNAL and request.address == self.address:
            if request.command in (Commands.STATUS ^ 0xFFFFFFFF, Commands.SET ^ 0xFFFFFFFF):
                self.blockSignals(True)
                self.status.ParseFromString(request.data)
                self.status2ui()
                self.blockSignals(False)

    def hideEvent(self, a0: QtGui.QHideEvent) -> None:
        gc.collect()
        self.close()


def main():
    app = QtWidgets.QApplication(sys.argv)
    ex = ShotSettings()
    ex.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
