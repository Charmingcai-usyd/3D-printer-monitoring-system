import cv2
import numpy as np
import argparse


"""# 配置 摄像头1 的颜色识别后的抓取######################################################################################"""
ap = argparse.ArgumentParser()
ap.add_argument("-v", "--video", type=str,
                help="path to input video file")
ap.add_argument("-t", "--tracker", type=str, default="kcf",
                help="OpenCV object tracker type")
args = vars(ap.parse_args())

"""# 参数配置 opencv已经实现了的追踪算法##################################################################################"""
OPENCV_OBJECT_TRACKERS = {  ##摄像头1 抓取参数配置
    "csrt": cv2.TrackerCSRT_create,
    "kcf": cv2.TrackerKCF_create,
    "boosting": cv2.TrackerBoosting_create,
    "mil": cv2.TrackerMIL_create,
    "tld": cv2.TrackerTLD_create,
    "medianflow": cv2.TrackerMedianFlow_create,
    "mosse": cv2.TrackerMOSSE_create
}
trackers = cv2.MultiTracker_create()  ##摄像头1 多对象抓取 传参函数

"""#摄像头1 实现颜色抓取 参数配置#########################################################################################"""
myColors = [[20, 191, 121, 49, 255, 255]]  # 黄色识别卡贴 颜色过滤识别 参数配置
GJColors = [[67, 129, 84, 98, 230, 229],[51, 129, 31, 165, 255, 255]]  # 工件颜色识别 参数配置

"""#子函数：Fans_findColor找到摄像头1识别到的颜色标签并返回颜色块的中心点###########################################################"""


def Fans_findColor(img, myColors, x_min, y_min, x_max, y_max):
    imgHSV = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    count = 0
    newPoints = []
    for color in myColors:  # 传参 将颜色参数读入inrage并返回颜色块getcountour的参数x,y,w,h
        lower = np.array(color[0:3])
        upper = np.array(color[3:6])
        mask = cv2.inRange(imgHSV, lower, upper)
        x, y, w, h = Fans_getContours(mask, x_min, y_min, x_max, y_max)
        if x != 0 and y != 0:
            newPoints.append([x, y, count])
        count += 1
    return newPoints, x, y, w, h  # 返回中心点矩阵框的右上角x,y和绘制矩形的宽度w与高度h


"""#子函数：Fans_findColor找到摄像头1识别到的颜色标签并返回颜色块的中心点###########################################################"""


def GJ_findColor(img, GJColors, x_min, y_min, x_max, y_max):
    imgHSV = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    count = 0
    newPoints = []
    for color in GJColors:  # 传参 将颜色参数读入inrage并返回颜色块getcountour的参数x,y,w,h
        lower = np.array(color[0:3])
        upper = np.array(color[3:6])
        mask = cv2.inRange(imgHSV, lower, upper)
        img, x, y, w, h = GJ_getContours(img, mask, x_min, y_min, x_max,
                                         y_max)  # img原图 mask二值化图像x_min,y_min,x_max y_max限定范围
        if x != 0 and y != 0:
            newPoints.append([x, y, count])
        count += 1
    return img, newPoints, x, y, w, h  # 返回中心点矩阵框的右上角x,y和绘制矩形的宽度w与高度h


"""#风扇返回抓取框框#######################################################################################################"""


def Fans_getcoordinate(imglock, x_min, y_min, x_max, y_max):
    newPoints, x, y, w, h = Fans_findColor(imglock, myColors, x_min, y_min, x_max, y_max)  ##返回框框
    return newPoints, x, y, w, h


"""#工件返回抓取框框#######################################################################################################"""


def GJ_getcoordinate(imglock, x_min, y_min, x_max, y_max):
    img, newPoints, x, y, w, h = GJ_findColor(imglock, GJColors, x_min, y_min, x_max, y_max)  ##返回框框
    return img, newPoints, x, y, w, h


"""#子函数：getcontour根据找到的每个小颜色块定位喷头边界 并返回喷头定位框的x,y,w,h数据###########################################"""


def Fans_getContours(img_wb, x_min, y_min, x_max, y_max):
    imgResult, contours, hierarchy = cv2.findContours(img_wb, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    x, y, w, h = 0, 0, 0, 0
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area > 200:  # 过滤掉背景的过大色块与过小色块
            cv2.drawContours(imgResult, cnt, -1, (255, 0, 0), 3)
            peri = cv2.arcLength(cnt, True)
            approx = cv2.approxPolyDP(cnt, 0.02 * peri, True)
            x, y, w, h = cv2.boundingRect(approx)
            if x < x_min:  # 框选算法，滑动计算
                x_min = x
            if y < y_min:
                y_min = y
            if x + w > x_max:
                x_max = x + w
            if y + h > y_max:
                y_max = y + h
    w = x_max - x_min
    h = y_max - y_min
    m = 5  ##m为边界框选边界的拓展宽度 与原边拓展宽度为 m
    x_min = x_min - m
    y_min = y_min - m
    w = w + 2 * m
    h = h + 2 * m
    return x_min, y_min, w, h  ##返回拓展宽度后的x,y,w,h数据


"""#子函数：getcontour根据找到的每个小颜色块定位喷头边界 并返回喷头定位框的x,y,w,h数据###########################################"""


def GJ_getContours(img, img_wb, x_min, y_min, x_max, y_max):
    int
    GJ_count_Ture = 0
    GJ_count_All = 0
    imgResult, contours, hierarchy = cv2.findContours(img_wb, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    x, y, w, h, txtx, txty = 0, 0, 0, 0, 130, 20
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area > 6000:  # 过滤掉背景的过大色块与过小色块

            cv2.drawContours(imgResult, cnt, -1, (255, 0, 0), 3)
            peri = cv2.arcLength(cnt, True)
            approx = cv2.approxPolyDP(cnt, 0.02 * peri, True)
            x, y, w, h = cv2.boundingRect(approx)
            GJ_count_All += 1
            if x > x_max and y > y_max and x_min > x + w and y_min > y + h:  # 范围限定 与风扇识别保持同步
                   GJ_count_Ture += 1
                   cv2.rectangle(img, (x, y), (x + w, y + h), (125, 35, 125), 2)
                   cv2.rectangle(img, (x, y), (x + 45, y + 15), (125, 35, 125), -1)  # 绘制抓取限定边界
                   cv2.putText(img, 'Part', (x, y + 15), cv2.FONT_HERSHEY_COMPLEX, 0.6, (0, 0, 255), 1)  # 绘制Track抓取文字标记
                   pass
            else: # 其他角说明工件有问题
                pass
            pass


    cv2.rectangle(img, (txtx, txty - 15), (txtx + 170, txty + 5), (255, 255, 0), -1)
    s1 = 'All:%2d' % GJ_count_All
    s2 = 'Valid:%2d' % GJ_count_Ture
    cv2.putText(img, s1, (txtx, txty), cv2.FONT_HERSHEY_COMPLEX, 0.6, (0, 0, 255), 2)  # 绘制抓取零件数量
    cv2.putText(img, s2, (txtx+80, txty), cv2.FONT_HERSHEY_COMPLEX, 0.6, (0, 0, 255), 2)  # 绘制抓取零件数量
    return img, x, y, w, h  ##返回x,y,w,h数据


"""#子函数：进行缩放####################################################################################################"""


def getcurrentframe(frame):

    #

    return frame


"""#喷头1子函数#######################################################################################################"""


def sxt1(preprocess_img, counting, flag, scan, num_camara):
    if num_camara != 1:
        x_min_track, y_min_track, x_max_track, y_max_track = 500, 350, 100, 100  # 确定风扇范围
        x_min_GJ, y_min_GJ, x_max_GJ, y_max_GJ = 500, 680, 50, 360  # 确定工件范围
    # 从喷头数量判断抓取区域
    else:
        x_min_track, y_min_track, x_max_track, y_max_track = 600, 410, 1, 1  # 确定风扇范围
        x_min_GJ, y_min_GJ, x_max_GJ, y_max_GJ = 620, 490, 50, 50  # 确定工件范围 这里的值是滑动值所以是相反的

    img = getcurrentframe(preprocess_img)  # 返回frame
    frame_1 = img.copy()
    frame_2 = img.copy()

    img, newPoints, x, y, w, h = GJ_getcoordinate(frame_2, x_min_GJ, y_min_GJ, x_max_GJ, y_max_GJ)  # 得到当前框选的工件
    newPoints, x_track, y_track, w_track, h_track = Fans_getcoordinate(frame_1, x_min_track, y_min_track, x_max_track, y_max_track)  # 得到当前框选的
    (success, boxes) = trackers.update(frame_1)  # 追踪返回结果

    for box in boxes:
        (x, y, w, h) = [int(v) for v in box]

    counting = counting + 1  # 扫描信号为上电后约 1s后发出并只执行一次
    if flag == 0 and counting > 30:
        scan = 1
        counting = 0
        flag = 1

    if scan == 1:  # 上电延时后的扫描信号检测，如果出现扫描信号执行颜色识别算法并框选计算出的喷头 这里的抓取用到了img
        # 选择一个区域，按s
        box = (x, y, w, h)
        # 创建一个新的追踪器
        tracker = OPENCV_OBJECT_TRACKERS[args["tracker"]]()
        trackers.add(tracker, img, box)
        scan = 0

    cv2.rectangle(img, (x_max_track, y_max_track), (x_min_track, y_min_track), (125, 255, 0), 1)
    # 绘制抓取限定边界
    cv2.rectangle(img, (x_max_GJ, y_max_GJ), (x_min_GJ, y_min_GJ), (125, 0, 125), 1)
    # 绘制抓取限定边界
    if x_track > x_max_track and y_track > y_max_track and x_min_track > x_track + w_track and y_min_track > y_track + h_track:
        # 范围限定与风扇识别保持同步
        cv2.rectangle(img, (x_track, y_track), (x_track + w_track, y_track + h_track), (255, 0, 255), 2)
        cv2.rectangle(img, (x_track, y_track), (x_track + 80, y_track + 15), (255, 125, 125), -1)
        cv2.putText(img, 'Track', (x_track, y_track + 11), cv2.FONT_HERSHEY_COMPLEX, 0.8, (0, 0, 255), 1)
        # 绘制Track抓取文字标记
        pass
    # 绘制抓取图像
    cv2.rectangle(img, (x_max_track, y_max_track), (x_max_track + 95, y_max_track + 15), (255, 0, 255), -1)
    cv2.putText(img, 'Track area', (x_max_track, y_max_track + 11), cv2.FONT_HERSHEY_COMPLEX, 0.5, (0, 255, 0),
                1)  # 绘制喷头抓取信息标签
    cv2.rectangle(img, (x_max_GJ, y_max_GJ), (x_max_GJ + 155, y_max_GJ + 15), (125, 0, 125), -1)
    cv2.putText(img, '3D printing parts', (x_max_GJ, y_max_GJ + 11), cv2.FONT_HERSHEY_COMPLEX, 0.5, (255, 255, 0),
                1)  # 绘制零件抓取信息标签
    return img
