# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'PX5UIDesign.ui'
#
# Created by: PyQt5 UI code generator 5.15.4
#
# WARNING: Any manual changes made to this file will be lost when pyuic5 is
# run again.  Do not edit this file unless you know what you are doing.


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_PX5WidgetDesign(object):
    def setupUi(self, PX5WidgetDesign):
        PX5WidgetDesign.setObjectName("PX5WidgetDesign")
        PX5WidgetDesign.resize(300, 310)
        self.groupBox = QtWidgets.QGroupBox(PX5WidgetDesign)
        self.groupBox.setGeometry(QtCore.QRect(10, 10, 281, 56))
        self.groupBox.setObjectName("groupBox")
        self.gridLayout = QtWidgets.QGridLayout(self.groupBox)
        self.gridLayout.setObjectName("gridLayout")
        self.enable_label = QtWidgets.QLabel(self.groupBox)
        self.enable_label.setObjectName("enable_label")
        self.gridLayout.addWidget(self.enable_label, 0, 0, 1, 1)
        self.enable_pushButton = QtWidgets.QPushButton(self.groupBox)
        self.enable_pushButton.setObjectName("enable_pushButton")
        self.gridLayout.addWidget(self.enable_pushButton, 0, 1, 1, 1)
        self.settings_toolBox = QtWidgets.QToolBox(PX5WidgetDesign)
        self.settings_toolBox.setGeometry(QtCore.QRect(10, 70, 281, 231))
        self.settings_toolBox.setFrameShape(QtWidgets.QFrame.NoFrame)
        self.settings_toolBox.setFrameShadow(QtWidgets.QFrame.Plain)
        self.settings_toolBox.setObjectName("settings_toolBox")
        self.start_page = QtWidgets.QWidget()
        self.start_page.setGeometry(QtCore.QRect(0, 0, 281, 150))
        self.start_page.setObjectName("start_page")
        self.gridLayout_2 = QtWidgets.QGridLayout(self.start_page)
        self.gridLayout_2.setObjectName("gridLayout_2")
        self.startsouece_label = QtWidgets.QLabel(self.start_page)
        self.startsouece_label.setObjectName("startsouece_label")
        self.gridLayout_2.addWidget(self.startsouece_label, 0, 0, 1, 1)
        self.startsource_comboBox = QtWidgets.QComboBox(self.start_page)
        self.startsource_comboBox.setObjectName("startsource_comboBox")
        self.startsource_comboBox.addItem("")
        self.startsource_comboBox.addItem("")
        self.gridLayout_2.addWidget(self.startsource_comboBox, 0, 1, 1, 1)
        self.tomeasure_label = QtWidgets.QLabel(self.start_page)
        self.tomeasure_label.setObjectName("tomeasure_label")
        self.gridLayout_2.addWidget(self.tomeasure_label, 1, 0, 1, 1)
        self.tomeasure_comboBox = QtWidgets.QComboBox(self.start_page)
        self.tomeasure_comboBox.setObjectName("tomeasure_comboBox")
        self.tomeasure_comboBox.addItem("")
        self.tomeasure_comboBox.addItem("")
        self.gridLayout_2.addWidget(self.tomeasure_comboBox, 1, 1, 1, 1)
        self.spectrsource_label = QtWidgets.QLabel(self.start_page)
        self.spectrsource_label.setObjectName("spectrsource_label")
        self.gridLayout_2.addWidget(self.spectrsource_label, 2, 0, 1, 1)
        self.spectrsource_comboBox = QtWidgets.QComboBox(self.start_page)
        self.spectrsource_comboBox.setObjectName("spectrsource_comboBox")
        self.spectrsource_comboBox.addItem("")
        self.spectrsource_comboBox.addItem("")
        self.spectrsource_comboBox.addItem("")
        self.spectrsource_comboBox.addItem("")
        self.gridLayout_2.addWidget(self.spectrsource_comboBox, 2, 1, 1, 1)
        self.colltime_label = QtWidgets.QLabel(self.start_page)
        self.colltime_label.setObjectName("colltime_label")
        self.gridLayout_2.addWidget(self.colltime_label, 3, 0, 1, 1)
        self.colltime_spinBox = QtWidgets.QSpinBox(self.start_page)
        self.colltime_spinBox.setObjectName("colltime_spinBox")
        self.gridLayout_2.addWidget(self.colltime_spinBox, 3, 1, 1, 1)
        self.settings_toolBox.addItem(self.start_page, "")
        self.mca_page = QtWidgets.QWidget()
        self.mca_page.setGeometry(QtCore.QRect(0, 0, 281, 150))
        self.mca_page.setObjectName("mca_page")
        self.gridLayout_3 = QtWidgets.QGridLayout(self.mca_page)
        self.gridLayout_3.setObjectName("gridLayout_3")
        self.numberch_label = QtWidgets.QLabel(self.mca_page)
        self.numberch_label.setObjectName("numberch_label")
        self.gridLayout_3.addWidget(self.numberch_label, 0, 0, 1, 1)
        self.numberch_spinBox = QtWidgets.QSpinBox(self.mca_page)
        self.numberch_spinBox.setObjectName("numberch_spinBox")
        self.gridLayout_3.addWidget(self.numberch_spinBox, 0, 1, 1, 1)
        self.label_6 = QtWidgets.QLabel(self.mca_page)
        self.label_6.setObjectName("label_6")
        self.gridLayout_3.addWidget(self.label_6, 1, 0, 1, 1)
        self.doubleSpinBox = QtWidgets.QDoubleSpinBox(self.mca_page)
        self.doubleSpinBox.setObjectName("doubleSpinBox")
        self.gridLayout_3.addWidget(self.doubleSpinBox, 1, 1, 1, 1)
        self.flattop_label = QtWidgets.QLabel(self.mca_page)
        self.flattop_label.setObjectName("flattop_label")
        self.gridLayout_3.addWidget(self.flattop_label, 2, 0, 1, 1)
        self.flattop_doubleSpinBox = QtWidgets.QDoubleSpinBox(self.mca_page)
        self.flattop_doubleSpinBox.setObjectName("flattop_doubleSpinBox")
        self.gridLayout_3.addWidget(self.flattop_doubleSpinBox, 2, 1, 1, 1)
        self.pktime_elabel = QtWidgets.QLabel(self.mca_page)
        self.pktime_elabel.setObjectName("pktime_elabel")
        self.gridLayout_3.addWidget(self.pktime_elabel, 3, 0, 1, 1)
        self.pktime_doubleSpinBox = QtWidgets.QDoubleSpinBox(self.mca_page)
        self.pktime_doubleSpinBox.setObjectName("pktime_doubleSpinBox")
        self.gridLayout_3.addWidget(self.pktime_doubleSpinBox, 3, 1, 1, 1)
        self.pileup_label = QtWidgets.QLabel(self.mca_page)
        self.pileup_label.setObjectName("pileup_label")
        self.gridLayout_3.addWidget(self.pileup_label, 4, 0, 1, 1)
        self.pileup_pushButton = QtWidgets.QPushButton(self.mca_page)
        self.pileup_pushButton.setObjectName("pileup_pushButton")
        self.gridLayout_3.addWidget(self.pileup_pushButton, 4, 1, 1, 1)
        self.settings_toolBox.addItem(self.mca_page, "")
        self.outsig_page = QtWidgets.QWidget()
        self.outsig_page.setObjectName("outsig_page")
        self.gridLayout_4 = QtWidgets.QGridLayout(self.outsig_page)
        self.gridLayout_4.setObjectName("gridLayout_4")
        self.outputdac_label = QtWidgets.QLabel(self.outsig_page)
        self.outputdac_label.setObjectName("outputdac_label")
        self.gridLayout_4.addWidget(self.outputdac_label, 0, 0, 1, 1)
        self.outputdac_comboBox = QtWidgets.QComboBox(self.outsig_page)
        self.outputdac_comboBox.setObjectName("outputdac_comboBox")
        self.outputdac_comboBox.addItem("")
        self.outputdac_comboBox.addItem("")
        self.outputdac_comboBox.addItem("")
        self.outputdac_comboBox.addItem("")
        self.outputdac_comboBox.addItem("")
        self.gridLayout_4.addWidget(self.outputdac_comboBox, 0, 1, 1, 1)
        self.dacoffdet_label = QtWidgets.QLabel(self.outsig_page)
        self.dacoffdet_label.setObjectName("dacoffdet_label")
        self.gridLayout_4.addWidget(self.dacoffdet_label, 1, 0, 1, 1)
        self.dacoffset_doubleSpinBox = QtWidgets.QDoubleSpinBox(self.outsig_page)
        self.dacoffset_doubleSpinBox.setObjectName("dacoffset_doubleSpinBox")
        self.gridLayout_4.addWidget(self.dacoffset_doubleSpinBox, 1, 1, 1, 1)
        self.aux1_label = QtWidgets.QLabel(self.outsig_page)
        self.aux1_label.setObjectName("aux1_label")
        self.gridLayout_4.addWidget(self.aux1_label, 2, 0, 1, 1)
        self.aux1_comboBox = QtWidgets.QComboBox(self.outsig_page)
        self.aux1_comboBox.setObjectName("aux1_comboBox")
        self.aux1_comboBox.addItem("")
        self.aux1_comboBox.addItem("")
        self.aux1_comboBox.addItem("")
        self.gridLayout_4.addWidget(self.aux1_comboBox, 2, 1, 1, 1)
        self.aux2_label = QtWidgets.QLabel(self.outsig_page)
        self.aux2_label.setObjectName("aux2_label")
        self.gridLayout_4.addWidget(self.aux2_label, 3, 0, 1, 1)
        self.aux2_comboBox = QtWidgets.QComboBox(self.outsig_page)
        self.aux2_comboBox.setObjectName("aux2_comboBox")
        self.aux2_comboBox.addItem("")
        self.aux2_comboBox.addItem("")
        self.aux2_comboBox.addItem("")
        self.aux2_comboBox.addItem("")
        self.gridLayout_4.addWidget(self.aux2_comboBox, 3, 1, 1, 1)
        self.settings_toolBox.addItem(self.outsig_page, "")

        self.retranslateUi(PX5WidgetDesign)
        self.settings_toolBox.setCurrentIndex(2)
        QtCore.QMetaObject.connectSlotsByName(PX5WidgetDesign)

    def retranslateUi(self, PX5WidgetDesign):
        _translate = QtCore.QCoreApplication.translate
        PX5WidgetDesign.setWindowTitle(_translate("PX5WidgetDesign", "PX-5"))
        self.groupBox.setTitle(_translate("PX5WidgetDesign", "Включение или отключение управления"))
        self.enable_label.setText(_translate("PX5WidgetDesign", "Запуск PX-5"))
        self.enable_pushButton.setText(_translate("PX5WidgetDesign", "Disable"))
        self.startsouece_label.setText(_translate("PX5WidgetDesign", "Источник запуска"))
        self.startsource_comboBox.setItemText(0, _translate("PX5WidgetDesign", "Токамак"))
        self.startsource_comboBox.setItemText(1, _translate("PX5WidgetDesign", "АЦП"))
        self.tomeasure_label.setText(_translate("PX5WidgetDesign", "Измерение импульса"))
        self.tomeasure_comboBox.setItemText(0, _translate("PX5WidgetDesign", "Амплитуда"))
        self.tomeasure_comboBox.setItemText(1, _translate("PX5WidgetDesign", "Интервал"))
        self.spectrsource_label.setText(_translate("PX5WidgetDesign", "Источник спектра"))
        self.spectrsource_comboBox.setItemText(0, _translate("PX5WidgetDesign", "NORM"))
        self.spectrsource_comboBox.setItemText(1, _translate("PX5WidgetDesign", "FAST"))
        self.spectrsource_comboBox.setItemText(2, _translate("PX5WidgetDesign", "PUR"))
        self.spectrsource_comboBox.setItemText(3, _translate("PX5WidgetDesign", "RTD"))
        self.colltime_label.setText(_translate("PX5WidgetDesign", "Время сбора данных"))
        self.settings_toolBox.setItemText(self.settings_toolBox.indexOf(self.start_page), _translate("PX5WidgetDesign", "Режим запуска "))
        self.numberch_label.setText(_translate("PX5WidgetDesign", "Число каналов"))
        self.label_6.setText(_translate("PX5WidgetDesign", "Усиление сигнала"))
        self.flattop_label.setText(_translate("PX5WidgetDesign", "Flat Top Width"))
        self.pktime_elabel.setText(_translate("PX5WidgetDesign", "Peaking Time"))
        self.pileup_label.setText(_translate("PX5WidgetDesign", "Pile-up Reject "))
        self.pileup_pushButton.setText(_translate("PX5WidgetDesign", "Enable"))
        self.settings_toolBox.setItemText(self.settings_toolBox.indexOf(self.mca_page), _translate("PX5WidgetDesign", "Установки для измерения спектров MCA "))
        self.outputdac_label.setText(_translate("PX5WidgetDesign", "Signal for Output DAC "))
        self.outputdac_comboBox.setItemText(0, _translate("PX5WidgetDesign", "OFF"))
        self.outputdac_comboBox.setItemText(1, _translate("PX5WidgetDesign", "FAST"))
        self.outputdac_comboBox.setItemText(2, _translate("PX5WidgetDesign", "SHAPED"))
        self.outputdac_comboBox.setItemText(3, _translate("PX5WidgetDesign", "INPUT"))
        self.outputdac_comboBox.setItemText(4, _translate("PX5WidgetDesign", "PEAK"))
        self.dacoffdet_label.setText(_translate("PX5WidgetDesign", "DAC offset"))
        self.aux1_label.setText(_translate("PX5WidgetDesign", "AUX1"))
        self.aux1_comboBox.setItemText(0, _translate("PX5WidgetDesign", "DAC"))
        self.aux1_comboBox.setItemText(1, _translate("PX5WidgetDesign", "AUXOUT1"))
        self.aux1_comboBox.setItemText(2, _translate("PX5WidgetDesign", "AUXIN1"))
        self.aux2_label.setText(_translate("PX5WidgetDesign", "AUX2"))
        self.aux2_comboBox.setItemText(0, _translate("PX5WidgetDesign", "AUXOUT2"))
        self.aux2_comboBox.setItemText(1, _translate("PX5WidgetDesign", "AUXIN2"))
        self.aux2_comboBox.setItemText(2, _translate("PX5WidgetDesign", "GATEH"))
        self.aux2_comboBox.setItemText(3, _translate("PX5WidgetDesign", "GATEL"))
        self.settings_toolBox.setItemText(self.settings_toolBox.indexOf(self.outsig_page), _translate("PX5WidgetDesign", "Выходные сигналы"))
