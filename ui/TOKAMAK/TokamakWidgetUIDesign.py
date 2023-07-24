# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'TokamakWidgetUIDesign.ui'
#
# Created by: PyQt5 UI code generator 5.15.8
#
# WARNING: Any manual changes made to this file will be lost when pyuic5 is
# run again.  Do not edit this file unless you know what you are doing.


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_tokamakWidget(object):
    def setupUi(self, tokamakWidget):
        tokamakWidget.setObjectName("tokamakWidget")
        tokamakWidget.resize(409, 207)
        self.gridLayout = QtWidgets.QGridLayout(tokamakWidget)
        self.gridLayout.setObjectName("gridLayout")
        self.label = QtWidgets.QLabel(tokamakWidget)
        self.label.setObjectName("label")
        self.gridLayout.addWidget(self.label, 0, 0, 1, 1)
        self.density_doubleSpinBox = QtWidgets.QDoubleSpinBox(tokamakWidget)
        self.density_doubleSpinBox.setAccelerated(True)
        self.density_doubleSpinBox.setDecimals(1)
        self.density_doubleSpinBox.setSingleStep(0.1)
        self.density_doubleSpinBox.setObjectName("density_doubleSpinBox")
        self.gridLayout.addWidget(self.density_doubleSpinBox, 1, 1, 1, 1)
        self.label_2 = QtWidgets.QLabel(tokamakWidget)
        self.label_2.setObjectName("label_2")
        self.gridLayout.addWidget(self.label_2, 1, 0, 1, 1)
        self.mode_comboBox = QtWidgets.QComboBox(tokamakWidget)
        self.mode_comboBox.setObjectName("mode_comboBox")
        self.mode_comboBox.addItem("")
        self.mode_comboBox.addItem("")
        self.mode_comboBox.addItem("")
        self.gridLayout.addWidget(self.mode_comboBox, 0, 1, 1, 1)
        self.current_spinBox = QtWidgets.QSpinBox(tokamakWidget)
        self.current_spinBox.setAccelerated(True)
        self.current_spinBox.setMaximum(500)
        self.current_spinBox.setObjectName("current_spinBox")
        self.gridLayout.addWidget(self.current_spinBox, 2, 1, 1, 1)
        self.label_3 = QtWidgets.QLabel(tokamakWidget)
        self.label_3.setObjectName("label_3")
        self.gridLayout.addWidget(self.label_3, 2, 0, 1, 1)
        self.power_spinBox = QtWidgets.QSpinBox(tokamakWidget)
        self.power_spinBox.setAccelerated(True)
        self.power_spinBox.setMaximum(500)
        self.power_spinBox.setObjectName("power_spinBox")
        self.gridLayout.addWidget(self.power_spinBox, 3, 1, 1, 1)
        self.label_4 = QtWidgets.QLabel(tokamakWidget)
        self.label_4.setObjectName("label_4")
        self.gridLayout.addWidget(self.label_4, 3, 0, 1, 1)
        self.return_pushButton = QtWidgets.QPushButton(tokamakWidget)
        self.return_pushButton.setMinimumSize(QtCore.QSize(190, 44))
        font = QtGui.QFont()
        font.setPointSize(9)
        self.return_pushButton.setFont(font)
        self.return_pushButton.setCursor(QtGui.QCursor(QtCore.Qt.PointingHandCursor))
        self.return_pushButton.setObjectName("return_pushButton")
        self.gridLayout.addWidget(self.return_pushButton, 4, 0, 1, 1)
        self.install_pushButton = QtWidgets.QPushButton(tokamakWidget)
        self.install_pushButton.setMinimumSize(QtCore.QSize(190, 44))
        font = QtGui.QFont()
        font.setPointSize(9)
        self.install_pushButton.setFont(font)
        self.install_pushButton.setCursor(QtGui.QCursor(QtCore.Qt.PointingHandCursor))
        self.install_pushButton.setDefault(True)
        self.install_pushButton.setObjectName("install_pushButton")
        self.gridLayout.addWidget(self.install_pushButton, 4, 1, 1, 1)

        self.retranslateUi(tokamakWidget)
        QtCore.QMetaObject.connectSlotsByName(tokamakWidget)

    def retranslateUi(self, tokamakWidget):
        _translate = QtCore.QCoreApplication.translate
        tokamakWidget.setWindowTitle(_translate("tokamakWidget", "Tokamak"))
        self.label.setText(_translate("tokamakWidget", "Режим разряда:"))
        self.label_2.setText(_translate("tokamakWidget", "Плотность, полос:"))
        self.mode_comboBox.setItemText(0, _translate("tokamakWidget", "OH"))
        self.mode_comboBox.setItemText(1, _translate("tokamakWidget", "RF"))
        self.mode_comboBox.setItemText(2, _translate("tokamakWidget", "Тлеющий"))
        self.label_3.setText(_translate("tokamakWidget", "Ток, кА:"))
        self.label_4.setText(_translate("tokamakWidget", "Мощность ВЧ, кВт:"))
        self.return_pushButton.setText(_translate("tokamakWidget", "Вернуть настройки"))
        self.install_pushButton.setText(_translate("tokamakWidget", "Установить настройки"))