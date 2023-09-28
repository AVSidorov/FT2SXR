import os
import sys
import configparser
from PyQt5 import QtWidgets, QtCore, QtGui
import psutil
import gc
import shutil
import time
from datetime import date
from ui.NAS.nasWidgetDesign import Ui_nasWidgetDesign
from core.fileutils import today_dir, work_dir
# from nasWidgetDesign import Ui_nasWidgetDesign
# from fileutils import today_dir, work_dir


class nasWidget(QtWidgets.QWidget, Ui_nasWidgetDesign):
    def __init__(self, parent=None, work_dir=None):
        curdir = os.getcwd()
        os.chdir(os.path.join(curdir, 'ui', 'NAS'))
        super().__init__(parent=parent)
        self.setupUi(self)
        # os.chdir(r'D:\home\projects\SXR\FT2SXR')
        os.chdir(curdir)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose, True)

        self.settings = {
            'data_path': '',
            'nas_path': '',
            'compression': 0
        }
        self._compresserMover = None
        self.is_working_flag = False
        # self.wannastop = False
        self.paths_good_flags = {'data_path': True,
                                 'nas_path': True}

        self.move_progressBar.hide()
        self.compress_progressBar.hide()
        self.statusbar = QtWidgets.QStatusBar()
        self.statusbar.setSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Fixed)
        self.verticalLayout_3.addWidget(self.statusbar)
        self.statusbar.show()
        self.statusbar.showMessage('Ready!')

        self.dataPath_lineEdit.textChanged.connect(self.check_data_path)
        self.nasPath_lineEdit.textChanged.connect(self.check_nas_path)
        self.todayDataPath_pushButton.clicked.connect(self.set_today_data_dir)
        self.selectNasPath_pushButton.clicked.connect(self.select_nas_path)
        self.selectDataPath_pushButton.clicked.connect(self.select_data_path)
        self.defaultCompression_pushButton.clicked.connect(self.set_default_compression)
        self.moveCompress_pushButton.clicked.connect(self.compress_move)
        self.move_pushButton.clicked.connect(self.just_move)
        self.stop_pushButton.clicked.connect(self.stop)
        self.compress_pushButton.clicked.connect(self.just_compress)

        self.compression_spinBox.valueChanged.connect(self.check_compression_level)

        self.dir_stat = None
        self.files_in_dir = []
        self.files_to_send = []

        self.set_settings()

        self.timer = QtCore.QTimer()
        self.timer.timeout.connect(self.check_devs)
        self.timer.start(1000)

        gc.collect(generation=2)

    def set_settings(self):
        config = configparser.ConfigParser()
        config.read(os.path.join(work_dir(), 'ui', 'NAS', 'settings.ini'))
        self.settings['data_path'] = config['nas']['data_path']
        self.settings['nas_path'] = config['nas']['nas_path']
        self.settings['compression'] = int(config['nas']['compression'])
        self.nasPath_lineEdit.setText(config['nas']['nas_path'])
        self.compression_spinBox.setValue(int(config['nas']['compression']))

        if config['nas']['date'] == os.path.split(today_dir())[-1]:
            self.dataPath_lineEdit.setText(config['nas']['data_path'])
        else:
            self.todayDataPath_pushButton.click()
            config['nas']['date'] = os.path.split(today_dir())[-1]
            with open(os.path.join(work_dir(), 'ui', 'NAS', 'settings.ini'), 'w') as f:
                config.write(f)
        del config

        self.check_devs()

    def check_data_path(self):
        path = self.dataPath_lineEdit.text()
        if os.path.isabs(path) and os.path.exists(path) and os.path.isdir(path):
            self.dataPath_lineEdit.setStyleSheet('color: rgb(0, 0, 0)')
            self.dataPath_lineEdit.setFont(self.label.font())
            self.settings['data_path'] = path
            self.save_settings()
            self.paths_good_flags['data_path'] = True
            self.activate_buttons()
        else:
            self.paths_good_flags['data_path'] = False
            self.activate_buttons()
            self.dataPath_lineEdit.setStyleSheet('color: rgb(255, 0, 0)')
            self.dataPath_lineEdit.setFont(self.label.font())

    def check_nas_path(self):
        path = self.nasPath_lineEdit.text()
        if os.path.isabs(path) and os.path.exists(path) and (os.path.ismount(path) or os.path.isdir(path)):
            self.nasPath_lineEdit.setStyleSheet('color: rgb(0, 0, 0)')
            self.nasPath_lineEdit.setFont(self.label.font())
            self.settings['nas_path'] = path
            self.save_settings()
            self.paths_good_flags['nas_path'] = True
            self.activate_buttons()
        else:
            self.nasPath_lineEdit.setStyleSheet('color: rgb(255, 0, 0)')
            self.nasPath_lineEdit.setFont(self.label.font())
            self.paths_good_flags['nas_path'] = False
            self.activate_buttons()

    def check_compression_level(self):
        self.settings['compression'] = self.compression_spinBox.value()
        self.save_settings()

    def save_settings(self):
        config = configparser.ConfigParser()
        config.read(os.path.join(work_dir(), 'ui', 'NAS', 'settings.ini'))
        config['nas']['data_path'] = str(self.settings['data_path'])
        config['nas']['nas_path'] = str(self.settings['nas_path'])
        config['nas']['compression'] = str(self.settings['compression'])

        with open(os.path.join(work_dir(), 'ui', 'NAS', 'settings.ini'), 'w') as f:
            config.write(f)

        del config

        gc.collect(generation=2)

    def set_today_data_dir(self):
        self.dataPath_lineEdit.setText(today_dir())

    def select_nas_path(self):
        nas_path = QtWidgets.QFileDialog.getExistingDirectory(self,
                                                              "Select the NAS directory")
        if nas_path != '':
            self.nasPath_lineEdit.setText(nas_path)

    def select_data_path(self):
        data_path = QtWidgets.QFileDialog.getExistingDirectory(self,
                                                               "Select the data directory")
        if data_path != '':
            self.dataPath_lineEdit.setText(data_path)

    def set_default_compression(self):
        config = configparser.ConfigParser()
        config.read(os.path.join(work_dir(), 'ui', 'NAS', 'settings.ini'))
        self.compression_spinBox.setValue(int(config['DEFAULT']['compression']))

        del config

    def check_devs(self):
        try:
            if os.path.exists(self.settings['data_path']):
                self.paths_good_flags['data_path'] = True
                self.activate_buttons()
                files_in_dir = os.listdir(self.settings['data_path'])
                dir_stat = os.stat(self.settings['data_path'])
                if self.files_in_dir != files_in_dir or self.dir_stat != dir_stat:
                    n = 0
                    v = 0
                    for file in os.scandir(self.settings['data_path']):
                        stat = file.stat()
                        v += stat.st_size
                        n += 1
                    self.dataPath_label.setText(f'Файлов в папке: {n} ({round(v / 1048576, 1)} Мб)')
                    self.files_in_dir = files_in_dir
                    self.dir_stat = dir_stat
            else:
                raise FileNotFoundError
        except FileNotFoundError:
            self.paths_good_flags['data_path'] = False
            self.activate_buttons()
            self.dataPath_label.setText('Невозможно просмотреть содержимое папки')

        try:
            if os.path.exists(self.settings['nas_path']):
                disk = psutil.disk_usage(self.settings['nas_path'])
                total = round(disk.total / 1024 / 1024 / 1024, 1)
                available = round(disk.free / 1024 / 1024 / 1024, 1)
                usage = round((1 - available / total) * 100, 1)
                self.memory_label.setText(f'занято {usage}% (доступно {available}Gb)')
                self.state_label.setText('Connected')
                self.state_label.setStyleSheet('color : green')
                self.state_label.setFont(self.label_3.font())
                self.paths_good_flags['nas_path'] = True
                self.activate_buttons()
            else:
                raise FileNotFoundError
        except FileNotFoundError:
            self.paths_good_flags['nas_path'] = False
            self.activate_buttons()
            # self.stop()
            self.statusbar.showMessage('NAS недоступен!', 1000)
            self.memory_label.setText(f'невозможно обратиться к NAS')
            self.state_label.setText('Disconnected')
            self.state_label.setStyleSheet('color : red')
            self.state_label.setFont(self.label_3.font())

    def compress_move_files(self, do_compress=False, do_move=False):
        self._compresserMover = compresserMover(self.files_in_dir.copy(), self.settings.copy(), do_compress=do_compress, do_move=do_move)
        self._compresserMover.is_working_signal.connect(lambda flag: self.set_working(flag))
        self._compresserMover.statusbar_signal.connect(lambda msg, time: self.statusbar.showMessage(msg, time))
        self._compresserMover.show_compress_progressbar_signal.connect(lambda: self.compress_progressBar.show())
        self._compresserMover.hide_compress_progressbar_signal.connect(lambda: self.compress_progressBar.hide())
        self._compresserMover.compress_progressbar_signal.connect(lambda progress: self.compress_progressBar.setValue(progress))
        self._compresserMover.show_move_progressbar_signal.connect(lambda: self.move_progressBar.show())
        self._compresserMover.hide_move_progressbar_signal.connect(lambda: self.move_progressBar.hide())
        self._compresserMover.move_progressbar_signal.connect(lambda progress: self.move_progressBar.setValue(progress))
        self._compresserMover.start()

    # def move_files(self):
    #     self.statusbar.showMessage('Sending to the NAS...', 10000)
    #     files_in_dir = self.files_in_dir
    #     dir = self.settings['data_path']
    #     total_files = len(files_in_dir)
    #     nas_dir = os.path.join(self.settings['nas_path'], str(date.today().year)[:2] + os.path.split(dir)[-1][:2],
    #                            os.path.split(dir)[-1])
    #     os.makedirs(nas_dir, exist_ok=True)
    #
    #     n = 0
    #     self.move_progressBar.show()
    #     for file in files_in_dir:
    #         shutil.copy2(os.path.join(dir, file), nas_dir, follow_symlinks=True)
    #         n += 1
    #         self.move_progressBar.setValue(int(n / total_files * 100))
    #         if self.wannastop is True:
    #             self.wannastop = False
    #             break
    #     self.move_progressBar.hide()
    #     self.move_progressBar.setValue(0)

    def compress_move(self):
        self.compress_move_files(do_compress=True, do_move=True)

    def just_move(self):
        self.compress_move_files(do_compress=False, do_move=True)

    def just_compress(self):
        self.compress_move_files(do_compress=True, do_move=False)

    def stop(self):
        if self._compresserMover is not None:
            try:
                if self._compresserMover.isRunning():
                    self._compresserMover.stop()
            except Exception:
                pass

    def set_working(self, flag):
        self.is_working_flag = flag

    def activate_buttons(self):
        if self.paths_good_flags['nas_path'] and self.paths_good_flags['data_path']:
            self.compress_pushButton.setEnabled(True)
            self.move_pushButton.setEnabled(True)
            self.moveCompress_pushButton.setEnabled(True)
        elif not self.paths_good_flags['nas_path'] and self.paths_good_flags['data_path']:
            self.compress_pushButton.setEnabled(True)
            self.move_pushButton.setDisabled(True)
            self.moveCompress_pushButton.setDisabled(True)
        else:
            self.compress_pushButton.setDisabled(True)
            self.move_pushButton.setDisabled(True)
            self.moveCompress_pushButton.setDisabled(True)


class compresserMover(QtCore.QThread):
    compress_progressbar_signal = QtCore.pyqtSignal(int)
    move_progressbar_signal = QtCore.pyqtSignal(int)
    statusbar_signal = QtCore.pyqtSignal((str, int))
    set_working_signal = QtCore.pyqtSignal()
    show_compress_progressbar_signal = QtCore.pyqtSignal()
    hide_compress_progressbar_signal = QtCore.pyqtSignal()
    show_move_progressbar_signal = QtCore.pyqtSignal()
    hide_move_progressbar_signal = QtCore.pyqtSignal()
    is_working_signal = QtCore.pyqtSignal(bool)

    def __init__(self, files_in_dir, settings, do_compress=False, do_move=False):
        QtCore.QThread.__init__(self)
        self.files_in_dir = files_in_dir
        self.settings = settings
        self.wannastop = False
        self._do_compress = do_compress
        self._do_move = do_move

    def run(self):
        self.is_working_signal.emit(True)

        if self._do_compress and not self.wannastop:
            self.statusbar_signal.emit('Compressing...', 10000)
            files_in_dir = self.files_in_dir
            dir = self.settings['data_path']
            total_files = len(files_in_dir)
            n = 0
            self.show_compress_progressbar_signal.emit()
            for file in files_in_dir:
                if file[-3:] == '.h5':
                    file_old = os.path.splitext(file)[0] + '_old' + os.path.splitext(file)[-1]
                    os.rename(os.path.join(dir, file), os.path.join(dir, file_old))
                    if self.settings['compression'] == 0:
                        os.system(f'{os.path.join(work_dir(), "hdfview", "HDFView", "hdf5tools", "h5repack.exe")} '
                                  f'-f NONE {os.path.join(dir, file_old)} {os.path.join(dir, file)}')
                    else:
                        os.system(f'{os.path.join(work_dir(), "hdfview", "HDFView", "hdf5tools", "h5repack.exe")} '
                                  f'-f GZIP={self.settings["compression"]} {os.path.join(dir, file_old)} {os.path.join(dir, file)}')
                    os.remove(os.path.join(dir, file_old))
                n += 1
                self.compress_progressbar_signal.emit(int(n / total_files * 100))
                if self.wannastop is True:
                    self.wannastop = False
                    break
            self.hide_compress_progressbar_signal.emit()
            self.compress_progressbar_signal.emit(0)

        if self._do_move and not self.wannastop:
            self.statusbar_signal.emit('Sending to the NAS...', 10000)
            files_in_dir = self.files_in_dir
            dir = self.settings['data_path']
            total_files = len(files_in_dir)
            nas_dir = os.path.join(self.settings['nas_path'], str(date.today().year)[:2] + os.path.split(dir)[-1][:2],
                                   os.path.split(dir)[-1])
            os.makedirs(nas_dir, exist_ok=True)

            n = 0
            self.show_move_progressbar_signal.emit()
            for file in files_in_dir:
                shutil.copy2(os.path.join(dir, file), nas_dir, follow_symlinks=True)
                n += 1
                self.move_progressbar_signal.emit(int(n / total_files * 100))
                if self.wannastop is True:
                    self.wannastop = False
                    break
            self.hide_move_progressbar_signal.emit()
            self.move_progressbar_signal.emit(0)

        self.statusbar_signal.emit('Done!', 10000)
        self.is_working_signal.emit(False)

    def stop(self):
        self.wannastop = True


def main(work_dir=today_dir()):
    try:
        # Включите в блок try/except, если вы также нацелены на Mac/Linux
        from PyQt5.QtWinExtras import QtWin  # !!!
        myappid = 'FT2SXRNAS'  # !!!
        QtWin.setCurrentProcessExplicitAppUserModelID(myappid)  # !!!
    except ImportError:
        pass

    app = QtWidgets.QApplication(sys.argv)
    app.setWindowIcon(QtGui.QIcon(os.path.join(os.getcwd(), 'style', 'icons', 'icons8-upload-to-the-cloud-48.png')))
    mv = nasWidget(work_dir=work_dir)
    mv.setWindowIcon(QtGui.QIcon(os.path.join(os.getcwd(), 'style', 'icons', 'icons8-upload-to-the-cloud-48.png')))
    mv.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()
