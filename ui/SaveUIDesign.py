# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'SaveUIDesign.ui'
#
# Created by: PyQt5 UI code generator 5.15.6
#
# WARNING: Any manual changes made to this file will be lost when pyuic5 is
# run again.  Do not edit this file unless you know what you are doing.


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_save_Dialog(object):
    def setupUi(self, save_Dialog):
        save_Dialog.setObjectName("save_Dialog")
        save_Dialog.resize(401, 214)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(save_Dialog.sizePolicy().hasHeightForWidth())
        save_Dialog.setSizePolicy(sizePolicy)
        save_Dialog.setContextMenuPolicy(QtCore.Qt.NoContextMenu)
        save_Dialog.setModal(True)
        self.gridLayout = QtWidgets.QGridLayout(save_Dialog)
        self.gridLayout.setObjectName("gridLayout")
        spacerItem = QtWidgets.QSpacerItem(20, 40, QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Expanding)
        self.gridLayout.addItem(spacerItem, 4, 0, 1, 1)
        self.shot_lineEdit = QtWidgets.QLineEdit(save_Dialog)
        self.shot_lineEdit.setObjectName("shot_lineEdit")
        self.gridLayout.addWidget(self.shot_lineEdit, 2, 1, 1, 1)
        self.buttonBox = QtWidgets.QDialogButtonBox(save_Dialog)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Preferred, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.buttonBox.sizePolicy().hasHeightForWidth())
        self.buttonBox.setSizePolicy(sizePolicy)
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtWidgets.QDialogButtonBox.Cancel|QtWidgets.QDialogButtonBox.Ok)
        self.buttonBox.setObjectName("buttonBox")
        self.gridLayout.addWidget(self.buttonBox, 5, 0, 1, 2)
        self.place_comboBox = QtWidgets.QComboBox(save_Dialog)
        self.place_comboBox.setObjectName("place_comboBox")
        self.place_comboBox.addItem("")
        self.place_comboBox.addItem("")
        self.place_comboBox.addItem("")
        self.gridLayout.addWidget(self.place_comboBox, 1, 1, 1, 1)
        self.name_label = QtWidgets.QLabel(save_Dialog)
        self.name_label.setObjectName("name_label")
        self.gridLayout.addWidget(self.name_label, 0, 0, 1, 1)
        self.shot_label = QtWidgets.QLabel(save_Dialog)
        self.shot_label.setObjectName("shot_label")
        self.gridLayout.addWidget(self.shot_label, 2, 0, 1, 1)
        self.place_label = QtWidgets.QLabel(save_Dialog)
        self.place_label.setObjectName("place_label")
        self.gridLayout.addWidget(self.place_label, 1, 0, 1, 1)
        self.name_lineEdit = QtWidgets.QLineEdit(save_Dialog)
        self.name_lineEdit.setText("")
        self.name_lineEdit.setObjectName("name_lineEdit")
        self.gridLayout.addWidget(self.name_lineEdit, 0, 1, 1, 1)
        self.comm_label = QtWidgets.QLabel(save_Dialog)
        self.comm_label.setObjectName("comm_label")
        self.gridLayout.addWidget(self.comm_label, 3, 0, 1, 1)
        self.comm_lineEdit = QtWidgets.QLineEdit(save_Dialog)
        self.comm_lineEdit.setObjectName("comm_lineEdit")
        self.gridLayout.addWidget(self.comm_lineEdit, 3, 1, 1, 1)

        self.retranslateUi(save_Dialog)
        self.buttonBox.accepted.connect(save_Dialog.accept) # type: ignore
        self.buttonBox.rejected.connect(save_Dialog.reject) # type: ignore
        QtCore.QMetaObject.connectSlotsByName(save_Dialog)

    def retranslateUi(self, save_Dialog):
        _translate = QtCore.QCoreApplication.translate
        save_Dialog.setWindowTitle(_translate("save_Dialog", "Save .h5 file"))
        self.shot_lineEdit.setPlaceholderText(_translate("save_Dialog", "shot number"))
        self.place_comboBox.setItemText(0, _translate("save_Dialog", "АЦП"))
        self.place_comboBox.setItemText(1, _translate("save_Dialog", "Локально"))
        self.place_comboBox.setItemText(2, _translate("save_Dialog", "NAS"))
        self.name_label.setText(_translate("save_Dialog", "Имя файла"))
        self.shot_label.setText(_translate("save_Dialog", "Выстрел"))
        self.place_label.setText(_translate("save_Dialog", "Место сохранения"))
        self.name_lineEdit.setPlaceholderText(_translate("save_Dialog", "name"))
        self.comm_label.setText(_translate("save_Dialog", "Комментарий"))
        self.comm_lineEdit.setPlaceholderText(_translate("save_Dialog", "comment"))