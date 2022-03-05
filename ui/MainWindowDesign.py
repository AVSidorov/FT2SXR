# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'c:/Users/sid/OneDrive/!SCN/IN_WORK/PROG/ft2sxr/ui/MainWindowDesign.ui'
#
# Created by: PyQt5 UI code generator 5.9.2
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(800, 600)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Fixed, QtWidgets.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(MainWindow.sizePolicy().hasHeightForWidth())
        MainWindow.setSizePolicy(sizePolicy)
        MainWindow.setMaximumSize(QtCore.QSize(1920, 1080))
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Maximum, QtWidgets.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.centralwidget.sizePolicy().hasHeightForWidth())
        self.centralwidget.setSizePolicy(sizePolicy)
        self.centralwidget.setObjectName("centralwidget")
        self.mdiArea = QtWidgets.QMdiArea(self.centralwidget)
        self.mdiArea.setGeometry(QtCore.QRect(10, 10, 781, 381))
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Maximum, QtWidgets.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.mdiArea.sizePolicy().hasHeightForWidth())
        self.mdiArea.setSizePolicy(sizePolicy)
        self.mdiArea.setObjectName("mdiArea")
        self.buttonStart = QtWidgets.QPushButton(self.centralwidget)
        self.buttonStart.setGeometry(QtCore.QRect(50, 430, 121, 51))
        self.buttonStart.setObjectName("buttonStart")
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 800, 21))
        self.menubar.setObjectName("menubar")
        self.menuSave_Files = QtWidgets.QMenu(self.menubar)
        self.menuSave_Files.setObjectName("menuSave_Files")
        self.menuOpen_Files = QtWidgets.QMenu(self.menubar)
        self.menuOpen_Files.setObjectName("menuOpen_Files")
        self.menuInstall = QtWidgets.QMenu(self.menubar)
        self.menuInstall.setObjectName("menuInstall")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)
        self.actionADC = QtWidgets.QAction(MainWindow)
        self.actionADC.setObjectName("actionADC")
        self.menuInstall.addAction(self.actionADC)
        self.menubar.addAction(self.menuSave_Files.menuAction())
        self.menubar.addAction(self.menuOpen_Files.menuAction())
        self.menubar.addAction(self.menuInstall.menuAction())

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.buttonStart.setText(_translate("MainWindow", "Start"))
        self.menuSave_Files.setTitle(_translate("MainWindow", "Save Files"))
        self.menuOpen_Files.setTitle(_translate("MainWindow", "Open Files"))
        self.menuInstall.setTitle(_translate("MainWindow", "Install"))
        self.actionADC.setText(_translate("MainWindow", "ADC"))

