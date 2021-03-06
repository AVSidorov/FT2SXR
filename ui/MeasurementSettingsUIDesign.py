# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'MeasurementSettingsUIDesign.ui'
#
# Created by: PyQt5 UI code generator 5.15.4
#
# WARNING: Any manual changes made to this file will be lost when pyuic5 is
# run again.  Do not edit this file unless you know what you are doing.


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_MeasurementSettingsWidget(object):
    def setupUi(self, MeasurementSettingsWidget):
        MeasurementSettingsWidget.setObjectName("MeasurementSettingsWidget")
        MeasurementSettingsWidget.resize(346, 161)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(MeasurementSettingsWidget.sizePolicy().hasHeightForWidth())
        MeasurementSettingsWidget.setSizePolicy(sizePolicy)
        MeasurementSettingsWidget.setMinimumSize(QtCore.QSize(250, 145))
        self.verticalLayout = QtWidgets.QVBoxLayout(MeasurementSettingsWidget)
        self.verticalLayout.setSizeConstraint(QtWidgets.QLayout.SetMaximumSize)
        self.verticalLayout.setObjectName("verticalLayout")
        self.groupBox = QtWidgets.QGroupBox(MeasurementSettingsWidget)
        self.groupBox.setObjectName("groupBox")
        self.gridLayout = QtWidgets.QGridLayout(self.groupBox)
        self.gridLayout.setObjectName("gridLayout")
        self.shot_label = QtWidgets.QLabel(self.groupBox)
        self.shot_label.setObjectName("shot_label")
        self.gridLayout.addWidget(self.shot_label, 0, 0, 1, 1)
        self.shot_spinBox = QtWidgets.QSpinBox(self.groupBox)
        self.shot_spinBox.setObjectName("shot_spinBox")
        self.gridLayout.addWidget(self.shot_spinBox, 0, 1, 1, 1)
        self.position_label = QtWidgets.QLabel(self.groupBox)
        self.position_label.setObjectName("position_label")
        self.gridLayout.addWidget(self.position_label, 1, 0, 1, 1)
        self.position_doubleSpinBox = QtWidgets.QDoubleSpinBox(self.groupBox)
        self.position_doubleSpinBox.setObjectName("position_doubleSpinBox")
        self.gridLayout.addWidget(self.position_doubleSpinBox, 1, 1, 1, 1)
        self.time_label = QtWidgets.QLabel(self.groupBox)
        self.time_label.setObjectName("time_label")
        self.gridLayout.addWidget(self.time_label, 2, 0, 1, 1)
        self.start_timeEdit = QtWidgets.QTimeEdit(self.groupBox)
        self.start_timeEdit.setDateTime(QtCore.QDateTime(QtCore.QDate(2000, 1, 1), QtCore.QTime(0, 0, 0)))
        self.start_timeEdit.setMinimumDateTime(QtCore.QDateTime(QtCore.QDate(2000, 1, 1), QtCore.QTime(0, 0, 0)))
        self.start_timeEdit.setMaximumTime(QtCore.QTime(23, 59, 59))
        self.start_timeEdit.setCurrentSection(QtWidgets.QDateTimeEdit.HourSection)
        self.start_timeEdit.setCurrentSectionIndex(0)
        self.start_timeEdit.setObjectName("start_timeEdit")
        self.gridLayout.addWidget(self.start_timeEdit, 2, 1, 1, 1)
        self.type_label = QtWidgets.QLabel(self.groupBox)
        self.type_label.setObjectName("type_label")
        self.gridLayout.addWidget(self.type_label, 3, 0, 1, 1)
        self.type_comboBox = QtWidgets.QComboBox(self.groupBox)
        self.type_comboBox.setObjectName("type_comboBox")
        self.type_comboBox.addItem("")
        self.type_comboBox.addItem("")
        self.gridLayout.addWidget(self.type_comboBox, 3, 1, 1, 1)
        self.verticalLayout.addWidget(self.groupBox)

        self.retranslateUi(MeasurementSettingsWidget)
        QtCore.QMetaObject.connectSlotsByName(MeasurementSettingsWidget)

    def retranslateUi(self, MeasurementSettingsWidget):
        _translate = QtCore.QCoreApplication.translate
        MeasurementSettingsWidget.setWindowTitle(_translate("MeasurementSettingsWidget", "Measurement settings"))
        self.groupBox.setTitle(_translate("MeasurementSettingsWidget", "Settings"))
        self.shot_label.setText(_translate("MeasurementSettingsWidget", "Shot number"))
        self.position_label.setText(_translate("MeasurementSettingsWidget", "Position"))
        self.time_label.setText(_translate("MeasurementSettingsWidget", "Start time"))
        self.start_timeEdit.setDisplayFormat(_translate("MeasurementSettingsWidget", "H:mm:ss"))
        self.type_label.setText(_translate("MeasurementSettingsWidget", "Type"))
        self.type_comboBox.setItemText(0, _translate("MeasurementSettingsWidget", "SXR"))
        self.type_comboBox.setItemText(1, _translate("MeasurementSettingsWidget", "BKG"))
