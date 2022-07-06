# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'mainwindow.ui'
#
# Created by: PyQt5 UI code generator 5.15.0
#
# WARNING: Any manual changes made to this file will be lost when pyuic5 is
# run again.  Do not edit this file unless you know what you are doing.


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.setFixedSize(1350, 680)##主界面长宽
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.display_label = QtWidgets.QLabel(self.centralwidget)
        self.display_label.setGeometry(QtCore.QRect(13, 10, 1320, 580))##视频界面
        self.display_label.setObjectName("display_label")
        self.startButton = QtWidgets.QPushButton(self.centralwidget)
        self.startButton.setGeometry(QtCore.QRect(960, 605, 161, 51))##开始按键
        self.startButton.setObjectName("startButton")
        self.endButton = QtWidgets.QPushButton(self.centralwidget)
        self.endButton.setGeometry(QtCore.QRect(1160, 605, 161, 51))##结束按键
        self.endButton.setObjectName("endButton")
        self.lineEdit = QtWidgets.QLineEdit(self.centralwidget)
        self.lineEdit.setGeometry(QtCore.QRect(110, 620, 251, 20))##编辑地址
        self.lineEdit.setObjectName("lineEdit")
        self.label = QtWidgets.QLabel(self.centralwidget)
        self.label.setGeometry(QtCore.QRect(50, 620, 71, 20))##标签1
        self.label.setObjectName("label")
        self.comboBox1 = QtWidgets.QComboBox(self.centralwidget)
        self.comboBox1.setGeometry(QtCore.QRect(460, 620, 201, 22))##选择滤镜
        self.comboBox1.setObjectName("comboBox")
        self.label_2 = QtWidgets.QLabel(self.centralwidget)
        self.label_2.setGeometry(QtCore.QRect(400, 620, 71, 20))##标签2
        self.label_2.setObjectName("label_2")
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 800, 23))
        self.menubar.setObjectName("menubar")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)
        self.label_3 = QtWidgets.QLabel(self.centralwidget)
        self.label_3.setGeometry(QtCore.QRect(720, 620, 71, 20))##标签3
        self.label_3.setObjectName("label_3")
        self.comboBox2 = QtWidgets.QComboBox(self.centralwidget)
        self.comboBox2.setGeometry(QtCore.QRect(790, 620, 70, 22))##选择摄像头个数
        self.comboBox2.setObjectName("comboBox")

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "网络串流媒体播放器 Power By 蔡宗珉"))
        self.display_label.setText(_translate("MainWindow", "开始"))
        self.startButton.setText(_translate("MainWindow", "运行"))
        self.endButton.setText(_translate("MainWindow", "暂停"))
        self.lineEdit.setText(_translate("MainWindow", "rtmp://192.168.1.100:1935/live/test"))
        self.label.setText(_translate("MainWindow", "拉流地址："))
        self.label_2.setText(_translate("MainWindow", "颜色编码："))
        self.label_3.setText(_translate("MainWindow", "摄像头数量："))

