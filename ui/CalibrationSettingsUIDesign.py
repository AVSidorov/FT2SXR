# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'CalibrationSettingsUIDesign.ui'
#
# Created by: PyQt5 UI code generator 5.15.4
#
# WARNING: Any manual changes made to this file will be lost when pyuic5 is
# run again.  Do not edit this file unless you know what you are doing.


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_CalibrationSettingsWidgetDesign(object):
    def setupUi(self, CalibrationSettingsWidgetDesign):
        CalibrationSettingsWidgetDesign.setObjectName("CalibrationSettingsWidgetDesign")
        CalibrationSettingsWidgetDesign.resize(300, 95)
        self.groupBox = QtWidgets.QGroupBox(CalibrationSettingsWidgetDesign)
        self.groupBox.setGeometry(QtCore.QRect(10, 10, 281, 71))
        self.groupBox.setObjectName("groupBox")
        self.gridLayout = QtWidgets.QGridLayout(self.groupBox)
        self.gridLayout.setObjectName("gridLayout")
        self.source_label = QtWidgets.QLabel(self.groupBox)
        self.source_label.setObjectName("source_label")
        self.gridLayout.addWidget(self.source_label, 0, 0, 1, 1)
        self.source_comboBox = QtWidgets.QComboBox(self.groupBox)
        self.source_comboBox.setObjectName("source_comboBox")
        self.source_comboBox.addItem("")
        self.source_comboBox.addItem("")
        self.source_comboBox.addItem("")
        self.gridLayout.addWidget(self.source_comboBox, 0, 1, 1, 1)

        self.retranslateUi(CalibrationSettingsWidgetDesign)
        QtCore.QMetaObject.connectSlotsByName(CalibrationSettingsWidgetDesign)

    def retranslateUi(self, CalibrationSettingsWidget):
        _translate = QtCore.QCoreApplication.translate
        CalibrationSettingsWidget.setWindowTitle(_translate("CalibrationSettingsWidget", "Calibration settings"))
        self.groupBox.setTitle(_translate("CalibrationSettingsWidget", "Settings"))
        self.source_label.setText(_translate("CalibrationSettingsWidget", "Источник калибровки"))
        self.source_comboBox.setItemText(0, _translate("CalibrationSettingsWidget", "Fe"))
        self.source_comboBox.setItemText(1, _translate("CalibrationSettingsWidget", "Mini-X2"))
        self.source_comboBox.setItemText(2, _translate("CalibrationSettingsWidget", "GSA"))
