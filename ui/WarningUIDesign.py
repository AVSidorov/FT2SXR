# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'WarningUIDesign.ui'
#
# Created by: PyQt5 UI code generator 5.15.4
#
# WARNING: Any manual changes made to this file will be lost when pyuic5 is
# run again.  Do not edit this file unless you know what you are doing.


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_WarningWidget(object):
    def setupUi(self, WarningWidget):
        WarningWidget.setObjectName("WarningWidget")
        WarningWidget.resize(307, 114)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(WarningWidget.sizePolicy().hasHeightForWidth())
        WarningWidget.setSizePolicy(sizePolicy)
        WarningWidget.setMinimumSize(QtCore.QSize(200, 100))
        WarningWidget.setToolTipDuration(-1)
        self.verticalLayout = QtWidgets.QVBoxLayout(WarningWidget)
        self.verticalLayout.setSizeConstraint(QtWidgets.QLayout.SetMaximumSize)
        self.verticalLayout.setObjectName("verticalLayout")
        self.label = QtWidgets.QLabel(WarningWidget)
        font = QtGui.QFont()
        font.setPointSize(8)
        font.setBold(True)
        font.setWeight(75)
        font.setKerning(True)
        font.setStyleStrategy(QtGui.QFont.PreferDefault)
        self.label.setFont(font)
        self.label.setAlignment(QtCore.Qt.AlignCenter)
        self.label.setObjectName("label")
        self.verticalLayout.addWidget(self.label)
        self.buttonBox = QtWidgets.QDialogButtonBox(WarningWidget)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel|QtWidgets.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.verticalLayout.addWidget(self.buttonBox)

        self.retranslateUi(WarningWidget)
        self.buttonBox.accepted.connect(WarningWidget.accept)
        self.buttonBox.rejected.connect(WarningWidget.reject)
        QtCore.QMetaObject.connectSlotsByName(WarningWidget)

    def retranslateUi(self, WarningWidget):
        _translate = QtCore.QCoreApplication.translate
        WarningWidget.setWindowTitle(_translate("WarningWidget", "Warning"))
        self.label.setText(_translate("WarningWidget", "Завершить прогамму и \n"
"выключить оборудование?"))
