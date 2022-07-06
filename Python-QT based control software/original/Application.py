import PyQt5
import pt_opencv as pt
import os,cv2,sys
# 添加PyQt5环境变量(解决运行平台问题)
dirname = os.path.dirname(PyQt5.__file__)
plugin_path = os.path.join(dirname, 'Qt', 'plugins', 'platforms')
os.environ['QT_QPA_PLATFORM_PLUGIN_PATH'] = plugin_path
from queue import Queue
from PyQt5.QtWidgets import QMainWindow,QApplication,QMessageBox
from PyQt5.QtCore import QThread,Qt,QCoreApplication
from PyQt5.QtGui import QImage,QPixmap
from mainwindow import Ui_MainWindow
import traceback
from enum import IntEnum,unique


@unique
class ColorFormat(IntEnum):
    Format_Invalid = 0
    Format_Mono = 1
    Format_MonoLSB = 2
    Format_Indexed8 = 3
    Format_RGB32 = 4
    Format_ARGB32 = 5
    Format_ARGB32_Premultiplied = 6
    Format_RGB16 = 7
    Format_ARGB8565_Premultiplied = 8
    Format_RGB666 = 9
    Format_ARGB6666_Premultiplied = 10
    Format_RGB555 = 11
    Format_ARGB8555_Premultiplied = 12
    Format_RGB888 = 13
    Format_RGB444 = 14
    Format_ARGB4444_Premultiplied = 15
    Format_RGBX8888 = 16
    Format_RGBA8888 = 17
    Format_RGBA8888_Premultiplied = 18
    Format_BGR30 = 19
    Format_A2BGR30_Premultiplied = 20
    Format_RGB30 = 21
    Format_A2RGB30_Premultiplied = 22
    Format_Alpha8 = 23
    Format_Grayscale8 = 24
    Format_Grayscale16 = 28
    Format_BGR888 = 29
    pass

class Camara_number(IntEnum):
    _0 = 0
    _1 = 1
    _2 = 2
    pass

class FlvGetBuffer(QThread):
    """FlvPutBuffer Thread"""
    def __init__(self,queue = None,label = None):
        super(FlvGetBuffer, self).__init__()
        self.queue = queue
        self.label = label
        self.runningstate  = False
        self.colorformat   = ColorFormat.Format_RGB888.value
        self.camara_number = Camara_number._2.value

    def image_get(self):
        while self.runningstate:
            try:
                if self.queue.qsize() > 0:
                    frame = self.queue.get()
                    # print(frame.size)
                    try:
                        image1  = pt.run(frame,self.camara_number)#入口函数：(原始图像,摄像头数量)
                        image1  = cv2.cvtColor(image1, cv2.COLOR_RGB2BGR)
                        #在这里添加滤镜
                        imgx = QImage(image1.data, image1.shape[1], image1.shape[0], self.colorformat)
                    except Exception:
                        traceback.print_exc()
                    self.label.setPixmap(QPixmap.fromImage(imgx))
                    cv2.waitKey(10)
            except Exception:
                traceback.print_exc()

    def _FormatChange(self,index):
        self.colorformat = index
        pass

    def _NumChange(self,index):
        self.camara_number = index
        pass

    def setRunState(self,state:bool):
        self.runningstate = state

    def run(self):
        self.image_get()

class Video2Buffer(QThread):

    def __init__(self,lineEdit=None,queue=None):
        super(Video2Buffer,self).__init__()
        self.lineEdit = lineEdit
        self.queue = queue
        self.runingstate = False
        self.video = cv2.VideoCapture(self.lineEdit.text())

        pass

    def __del__(self):
        self.video.release()

    def get_frame(self):
        success, image = self.video.read()
        if success:
            self.queue.put(image)
        cv2.waitKey(10)
        pass

    def setRunState(self,state:bool):
        self.runingstate = state
        pass

    def updateUrl(self):
        self.video.release()
        cv2.waitKey(200)
        print(self.lineEdit.text())
        self.video = cv2.VideoCapture(self.lineEdit.text())
        pass

    def run(self):
        self.updateUrl()
        while self.runingstate:
            try:
                self.get_frame()
                pass
            except Exception:
                traceback.print_exc()
    pass
_translate = QCoreApplication.translate
class Application(QMainWindow,Ui_MainWindow):

    def __init__(self):
        super(Application,self).__init__()
        self.setupUi(self)
        self.comboBox1AddItems()
        self.comboBox2AddItems()
        self.startButton.setEnabled(True)
        self.endButton.setEnabled(False)
        with open('history.txt','r') as f:
            self.lineEdit.setText(_translate("MainWindow", f.readline()))
        self.loadingPicture = QImage("pic/index.png")
        self.display_label.setScaledContents(True) # 自适应尺寸
        self.display_label.setPixmap(QPixmap.fromImage(self.loadingPicture))
        self.FlvQueue = Queue(4)  # 设置缓存buffer
        self.FlvThread = FlvGetBuffer(queue=self.FlvQueue,label=self.display_label)
        self.VideoThread = Video2Buffer(lineEdit=self.lineEdit,queue=self.FlvQueue)
        self.signalConnect()
        pass

    def comboBox1AddItems(self):
        for text,enum_text  in ColorFormat.__members__.items():
            self.comboBox1.addItem(text,enum_text.value)
        self.comboBox1.setCurrentIndex(ColorFormat.Format_RGB888.value)
        pass

    def comboBox2AddItems(self):#摄像头个数
        for text,enum_text  in Camara_number.__members__.items():
            self.comboBox2.addItem(text,enum_text.value)
        self.comboBox2.setCurrentIndex(Camara_number._2.value)
        pass

    def signalConnect(self):
        self.startButton.clicked.connect(self.start_clicked)
        self.endButton.clicked.connect(self.stop_clicked)
        self.comboBox1.currentIndexChanged.connect(self.FlvThread._FormatChange)
        self.comboBox2.currentIndexChanged.connect(self.FlvThread._NumChange)
        pass

    def start_clicked(self):
        self.startButton.setEnabled(False)
        self.endButton.setEnabled(True)
        self.VideoThread.setRunState(True)
        self.FlvThread.setRunState(True)
        self.VideoThread.start()
        self.FlvThread.start()
        pass

    def stop_clicked(self):
        self.startButton.setEnabled(True)
        self.endButton.setEnabled(False)
        self.VideoThread.setRunState(False)
        self.FlvThread.setRunState(False)
        pass

    def closeEvent(self,event):
        self.FlvThread.setRunState(False)
        self.VideoThread.setRunState(False)
        with open('history.txt','w') as f:
            f.write(self.lineEdit.text())
        self.close()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    b = Application()
    b.show()
    sys.exit(app.exec_())