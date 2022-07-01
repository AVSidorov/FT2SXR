# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'CentralWidgetUIDesign.ui'
#
# Created by: PyQt5 UI code generator 5.15.6
#
# WARNING: Any manual changes made to this file will be lost when pyuic5 is
# run again.  Do not edit this file unless you know what you are doing.


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_MainWidgetDesign(object):
    def setupUi(self, MainWidgetDesign):
        MainWidgetDesign.setObjectName("MainWidgetDesign")
        MainWidgetDesign.setWindowModality(QtCore.Qt.NonModal)
        MainWidgetDesign.resize(1096, 566)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(MainWidgetDesign.sizePolicy().hasHeightForWidth())
        MainWidgetDesign.setSizePolicy(sizePolicy)
        MainWidgetDesign.setMinimumSize(QtCore.QSize(550, 400))
        self.horizontalLayout = QtWidgets.QHBoxLayout(MainWidgetDesign)
        self.horizontalLayout.setSizeConstraint(QtWidgets.QLayout.SetMaximumSize)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.groupBox_3 = QtWidgets.QGroupBox(MainWidgetDesign)
        self.groupBox_3.setMaximumSize(QtCore.QSize(200, 16777215))
        self.groupBox_3.setObjectName("groupBox_3")
        self.gridLayout = QtWidgets.QGridLayout(self.groupBox_3)
        self.gridLayout.setObjectName("gridLayout")
        self.Init_pushButton = QtWidgets.QPushButton(self.groupBox_3)
        self.Init_pushButton.setEnabled(True)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.Init_pushButton.sizePolicy().hasHeightForWidth())
        self.Init_pushButton.setSizePolicy(sizePolicy)
        self.Init_pushButton.setMinimumSize(QtCore.QSize(0, 0))
        self.Init_pushButton.setMouseTracking(False)
        self.Init_pushButton.setObjectName("Init_pushButton")
        self.gridLayout.addWidget(self.Init_pushButton, 0, 0, 1, 1)
        self.manual_pushButton = QtWidgets.QPushButton(self.groupBox_3)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.manual_pushButton.sizePolicy().hasHeightForWidth())
        self.manual_pushButton.setSizePolicy(sizePolicy)
        self.manual_pushButton.setObjectName("manual_pushButton")
        self.gridLayout.addWidget(self.manual_pushButton, 1, 0, 1, 1)
        self.periodic_pushButton = QtWidgets.QPushButton(self.groupBox_3)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.periodic_pushButton.sizePolicy().hasHeightForWidth())
        self.periodic_pushButton.setSizePolicy(sizePolicy)
        self.periodic_pushButton.setObjectName("periodic_pushButton")
        self.gridLayout.addWidget(self.periodic_pushButton, 2, 0, 1, 1)
        self.external_pushButton = QtWidgets.QPushButton(self.groupBox_3)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.external_pushButton.sizePolicy().hasHeightForWidth())
        self.external_pushButton.setSizePolicy(sizePolicy)
        self.external_pushButton.setObjectName("external_pushButton")
        self.gridLayout.addWidget(self.external_pushButton, 3, 0, 1, 1)
        self.stop_pushButton = QtWidgets.QPushButton(self.groupBox_3)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.stop_pushButton.sizePolicy().hasHeightForWidth())
        self.stop_pushButton.setSizePolicy(sizePolicy)
        self.stop_pushButton.setObjectName("stop_pushButton")
        self.gridLayout.addWidget(self.stop_pushButton, 4, 0, 1, 1)
        self.save_pushButton = QtWidgets.QPushButton(self.groupBox_3)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.save_pushButton.sizePolicy().hasHeightForWidth())
        self.save_pushButton.setSizePolicy(sizePolicy)
        self.save_pushButton.setObjectName("save_pushButton")
        self.gridLayout.addWidget(self.save_pushButton, 5, 0, 1, 1)
        self.enableMiniX2_pushButton = QtWidgets.QPushButton(self.groupBox_3)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Minimum, QtWidgets.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.enableMiniX2_pushButton.sizePolicy().hasHeightForWidth())
        self.enableMiniX2_pushButton.setSizePolicy(sizePolicy)
        self.enableMiniX2_pushButton.setObjectName("enableMiniX2_pushButton")
        self.gridLayout.addWidget(self.enableMiniX2_pushButton, 6, 0, 1, 1)
        self.horizontalLayout.addWidget(self.groupBox_3)
        self.splitter = QtWidgets.QSplitter(MainWidgetDesign)
        self.splitter.setOrientation(QtCore.Qt.Vertical)
        self.splitter.setObjectName("splitter")
        self.groupBox = QtWidgets.QGroupBox(self.splitter)
        self.groupBox.setMinimumSize(QtCore.QSize(400, 270))
        self.groupBox.setMaximumSize(QtCore.QSize(16777215, 300))
        self.groupBox.setObjectName("groupBox")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.groupBox)
        self.verticalLayout.setObjectName("verticalLayout")
        self.status_tableWidget = QtWidgets.QTableWidget(self.groupBox)
        self.status_tableWidget.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.status_tableWidget.setFocusPolicy(QtCore.Qt.StrongFocus)
        self.status_tableWidget.setLayoutDirection(QtCore.Qt.LeftToRight)
        self.status_tableWidget.setAutoFillBackground(False)
        self.status_tableWidget.setInputMethodHints(QtCore.Qt.ImhNone)
        self.status_tableWidget.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.status_tableWidget.setLineWidth(1)
        self.status_tableWidget.setMidLineWidth(0)
        self.status_tableWidget.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAsNeeded)
        self.status_tableWidget.setSizeAdjustPolicy(QtWidgets.QAbstractScrollArea.AdjustToContents)
        self.status_tableWidget.setEditTriggers(QtWidgets.QAbstractItemView.NoEditTriggers)
        self.status_tableWidget.setTabKeyNavigation(True)
        self.status_tableWidget.setProperty("showDropIndicator", False)
        self.status_tableWidget.setDragEnabled(False)
        self.status_tableWidget.setDragDropOverwriteMode(False)
        self.status_tableWidget.setDragDropMode(QtWidgets.QAbstractItemView.NoDragDrop)
        self.status_tableWidget.setSelectionMode(QtWidgets.QAbstractItemView.NoSelection)
        self.status_tableWidget.setSelectionBehavior(QtWidgets.QAbstractItemView.SelectItems)
        self.status_tableWidget.setTextElideMode(QtCore.Qt.ElideLeft)
        self.status_tableWidget.setGridStyle(QtCore.Qt.SolidLine)
        self.status_tableWidget.setWordWrap(True)
        self.status_tableWidget.setCornerButtonEnabled(True)
        self.status_tableWidget.setObjectName("status_tableWidget")
        self.status_tableWidget.setColumnCount(5)
        self.status_tableWidget.setRowCount(6)
        item = QtWidgets.QTableWidgetItem()
        self.status_tableWidget.setVerticalHeaderItem(0, item)
        item = QtWidgets.QTableWidgetItem()
        self.status_tableWidget.setVerticalHeaderItem(1, item)
        item = QtWidgets.QTableWidgetItem()
        self.status_tableWidget.setVerticalHeaderItem(2, item)
        item = QtWidgets.QTableWidgetItem()
        self.status_tableWidget.setVerticalHeaderItem(3, item)
        item = QtWidgets.QTableWidgetItem()
        self.status_tableWidget.setVerticalHeaderItem(4, item)
        item = QtWidgets.QTableWidgetItem()
        self.status_tableWidget.setVerticalHeaderItem(5, item)
        item = QtWidgets.QTableWidgetItem()
        self.status_tableWidget.setHorizontalHeaderItem(0, item)
        item = QtWidgets.QTableWidgetItem()
        self.status_tableWidget.setHorizontalHeaderItem(1, item)
        item = QtWidgets.QTableWidgetItem()
        self.status_tableWidget.setHorizontalHeaderItem(2, item)
        item = QtWidgets.QTableWidgetItem()
        self.status_tableWidget.setHorizontalHeaderItem(3, item)
        item = QtWidgets.QTableWidgetItem()
        self.status_tableWidget.setHorizontalHeaderItem(4, item)
        self.status_tableWidget.horizontalHeader().setCascadingSectionResizes(False)
        self.status_tableWidget.horizontalHeader().setDefaultSectionSize(80)
        self.status_tableWidget.horizontalHeader().setMinimumSectionSize(100)
        self.status_tableWidget.horizontalHeader().setSortIndicatorShown(False)
        self.status_tableWidget.horizontalHeader().setStretchLastSection(True)
        self.status_tableWidget.verticalHeader().setCascadingSectionResizes(True)
        self.status_tableWidget.verticalHeader().setDefaultSectionSize(30)
        self.status_tableWidget.verticalHeader().setSortIndicatorShown(True)
        self.status_tableWidget.verticalHeader().setStretchLastSection(True)
        self.verticalLayout.addWidget(self.status_tableWidget)
        self.groupBox_2 = QtWidgets.QGroupBox(self.splitter)
        self.groupBox_2.setObjectName("groupBox_2")
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(self.groupBox_2)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.log_textBrowser = QtWidgets.QTextBrowser(self.groupBox_2)
        self.log_textBrowser.setObjectName("log_textBrowser")
        self.verticalLayout_2.addWidget(self.log_textBrowser)
        self.horizontalLayout.addWidget(self.splitter)

        self.retranslateUi(MainWidgetDesign)
        QtCore.QMetaObject.connectSlotsByName(MainWidgetDesign)

    def retranslateUi(self, MainWidgetDesign):
        _translate = QtCore.QCoreApplication.translate
        MainWidgetDesign.setWindowTitle(_translate("MainWidgetDesign", "MainWidget"))
        self.groupBox_3.setTitle(_translate("MainWidgetDesign", "Действия"))
        self.Init_pushButton.setText(_translate("MainWidgetDesign", "Init"))
        self.manual_pushButton.setText(_translate("MainWidgetDesign", "Manual start:\n"
"Calibration"))
        self.periodic_pushButton.setText(_translate("MainWidgetDesign", "Periodic start:\n"
"Calibration"))
        self.external_pushButton.setText(_translate("MainWidgetDesign", "External trigger:\n"
"Calibration"))
        self.stop_pushButton.setText(_translate("MainWidgetDesign", "Stop waiting"))
        self.save_pushButton.setText(_translate("MainWidgetDesign", "Save data"))
        self.enableMiniX2_pushButton.setText(_translate("MainWidgetDesign", "Mini-X2:\n"
"OFF"))
        self.groupBox.setTitle(_translate("MainWidgetDesign", "Статус"))
        self.status_tableWidget.setSortingEnabled(False)
        item = self.status_tableWidget.verticalHeaderItem(0)
        item.setText(_translate("MainWidgetDesign", "1"))
        item = self.status_tableWidget.verticalHeaderItem(1)
        item.setText(_translate("MainWidgetDesign", "2"))
        item = self.status_tableWidget.verticalHeaderItem(2)
        item.setText(_translate("MainWidgetDesign", "3"))
        item = self.status_tableWidget.verticalHeaderItem(3)
        item.setText(_translate("MainWidgetDesign", "4"))
        item = self.status_tableWidget.verticalHeaderItem(4)
        item.setText(_translate("MainWidgetDesign", "5"))
        item = self.status_tableWidget.verticalHeaderItem(5)
        item.setText(_translate("MainWidgetDesign", "6"))
        item = self.status_tableWidget.horizontalHeaderItem(0)
        item.setText(_translate("MainWidgetDesign", "Snapshot"))
        item = self.status_tableWidget.horizontalHeaderItem(1)
        item.setText(_translate("MainWidgetDesign", "ADC"))
        item = self.status_tableWidget.horizontalHeaderItem(2)
        item.setText(_translate("MainWidgetDesign", "Amplifier"))
        item = self.status_tableWidget.horizontalHeaderItem(3)
        item.setText(_translate("MainWidgetDesign", "PX-5"))
        item = self.status_tableWidget.horizontalHeaderItem(4)
        item.setText(_translate("MainWidgetDesign", "Comments"))
        self.groupBox_2.setTitle(_translate("MainWidgetDesign", "Процессы"))
