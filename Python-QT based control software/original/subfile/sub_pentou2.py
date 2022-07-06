import cv2
import numpy as np



"""# 基本参数配置######################################################################################################"""
beH_cj = 0
hEnd_cj = 700
beW_cj = 0
wLen_cj = 1300

beH_lock = 880
hEnd_lock = 980
beW_lock = 200
wLen_lock = 260

beH_gongjian = 830
hEnd_gongjian = 1000
beW_gongjian = 180
wLen_gongjian = 360

"""                                     \/
----------------------------------------------->
      |                  |beH            
      |                  |
      |                  |
 ---->|                  |            |<----wLen //结束点位  
      |                  |            |        
   beW|<-----------------|------------|     
      |                  |            | 
      |                  |            |       
      |                  |____________|____       
      |                                 /\
                                        ||-hEnd //结束点位
      
beH   起点高
beW   起点
hEnd  终点高
wLen  高度

"""
status = 0
flag_pentou2 = 1
myColors = [[87, 113, 124, 123, 255, 227]]  # 黄色识别卡贴 颜色过滤识别 参数配置
myColorValues = [[255, 255, 0]]  # 标记 颜色过滤识别结果 颜色配置

"""# 通过得到RGB每个通道的直方图来计算相似度################################################################################"""
def classify_hist_with_split(image1, image2, size=(256, 256)):
    # 将图像resize后，分离为RGB三个通道，再计算每个通道的相似值
    image1 = cv2.resize(image1, size)
    image2 = cv2.resize(image2, size)
    sub_image1 = cv2.split(image1)
    sub_image2 = cv2.split(image2)
    sub_data = 0
    for im1, im2 in zip(sub_image1, sub_image2):
        sub_data += calculate(im1, im2)
    sub_data = sub_data / 3
    return sub_data



"""# 计算单通道的直方图的相似值###########################################################################################"""
def calculate(image1, image2):
    hist1 = cv2.calcHist([image1], [0], None, [256], [0.0, 255.0])
    hist2 = cv2.calcHist([image2], [0], None, [256], [0.0, 255.0])
    # 计算直方图的重合度
    degree = 0
    for i in range(len(hist1)):
        if hist1[i] != hist2[i]:
            degree = degree + (1 - abs(hist1[i] - hist2[i]) / max(hist1[i], hist2[i]))
        else:
            degree = degree + 1
    degree = degree / len(hist1)
    return degree




"""# 差值感知算法######################################################################################################"""
def dHash(img):
    # 缩放8*8
    img = cv2.resize(img, (9, 8))
    # 转换灰度图
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    hash_str = ''
    # 每行前一个像素大于后一个像素为1，相反为0，生成哈希
    for i in range(8):
        for j in range(8):
            if gray[i, j] > gray[i, j + 1]:
                hash_str = hash_str + '1'
            else:
                hash_str = hash_str + '0'
    return hash_str



"""# Hash值对比#######################################################################################################"""
def cmpHash(hash1, hash2):
    n = 0
    # hash长度不同则返回-1代表传参出错
    if len(hash1)!=len(hash2):
        return -1
    # 遍历判断
    for i in range(len(hash1)):
        # 不相等则n计数+1，n最终为相似度
        if hash1[i] != hash2[i]:
            n = n + 1
    return n



"""# 读取摄像头2图像####################################################################################################"""
def lockshexiangtou2(flag_pentou2,img):

    resize_img = img[beH_cj:hEnd_cj, beW_cj:wLen_cj] # 裁剪每帧图片
    img_gongjian = resize_img[beW_gongjian:wLen_gongjian, beH_gongjian:hEnd_gongjian] # 裁剪每帧图片
    resize_penzui = resize_img[beW_lock:wLen_lock, beH_lock:hEnd_lock] # 裁剪每帧图片



    if flag_pentou2 == 1:
       cv2.imwrite('pentou.png', resize_penzui)
       flag_pentou2 = 0

    return resize_img,resize_penzui,img_gongjian




"""# 子函数：findcolor找到摄像头2识别到的工件缺陷点并返回中心点框线坐标值#######################################################"""
def findColor(img_yuantu, img, myColors, myColorValues):
    imgHSV = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    count = 0
    newPoints = []
    for color in myColors:                            # 传参 将颜色参数读入inrage并返回颜色块getcountour的参数x,y,w,h
        lower = np.array(color[0:3])
        upper = np.array(color[3:6])
        mask = cv2.inRange(imgHSV, lower, upper)
        x, y, w, h, count = getContours(mask,img_yuantu)
        if x != 0 and y != 0:
            newPoints.append([x, y, count])
        # cv2.imshow(str(color[0]), mask)              # 现实颜色框的外轮廓
    return newPoints, x, y, w, h, count                # 返回中心点矩阵框的右上角x,y和绘制矩形的宽度w与高度h count为检测到的零件毛刺




"""# 子函数：getcontour根据找到的每个小颜色块定位喷头边界 并返回喷头定位框的x,y,w,h数据##########################################"""
def getContours(img_wb,img_yuantu):
    imgResult, contours, hierarchy = cv2.findContours(img_wb, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    count = 0
    x, y, w, h = 0, 0, 0, 0
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area > 10 and area < 800:                             # 过滤掉背景的过大色块与过小色块
            cv2.drawContours(imgResult, cnt, -1, (255, 0, 0), 3)
            peri = cv2.arcLength(cnt, True)
            approx = cv2.approxPolyDP(cnt, 0.02 * peri, True)
            x, y, w, h = cv2.boundingRect(approx)
            cv2.rectangle(img_yuantu,(beH_gongjian+x,beW_gongjian+y),(beH_gongjian+x+w,beW_gongjian+y+h),(200,200,200),1)
            # 在主图中框选出质量有问题的点
            count = count + 1
    return x, y, w, h, count  # 返回x,y,w,h数据 count为检测到的零件毛刺



"""# 摄像头2主函数#####################################################################################################"""
def sxt2(img):

    img1_pt2_process, img2, img3 = lockshexiangtou2(flag_pentou2,img)
    n = classify_hist_with_split(img1_pt2_process, img2)
    hash1 = dHash(img1_pt2_process)
    hash2 = dHash(img2)
    m = cmpHash(hash1, hash2)

    # 根据情况改变颜色
    LockColors = (255, 0, 0) # 默认颜色
    if m > 25 or n > 0.3:
       LockColors=(0,255,0)
       pass
    elif 20 >= m > 15 or n > 0.2:
       LockColors=(0,255,0)
       pass
    elif m <= 15 or n < 0.1:
       LockColors=(255,0,0)
       pass
    cv2.rectangle(img1_pt2_process,(beH_lock-1,beW_lock-1),(hEnd_lock,wLen_lock),LockColors,2) # 改变框选颜色
    cv2.rectangle(img1_pt2_process,(beH_gongjian-1,beW_gongjian-1),(hEnd_gongjian,wLen_gongjian),(255,0,0),2)

    cv2.rectangle(img1_pt2_process, (beH_gongjian+2, wLen_gongjian+2), (beH_gongjian + 110, wLen_gongjian+20), (0, 255, 255), -1)
    cv2.putText(img1_pt2_process,'workpiece',(beH_gongjian+5,wLen_gongjian+15), cv2.FONT_HERSHEY_COMPLEX,0.6,(255, 0, 0), 1) # 绘制workplace

    cv2.rectangle(img1_pt2_process, (beH_lock-2, beW_lock-2), (beH_lock + 80, beW_lock-20), (0, 255, 255), -1)
    cv2.putText(img1_pt2_process,'nozzle',(beH_lock,beW_lock-5), cv2.FONT_HERSHEY_COMPLEX,0.6,(255, 0, 0), 1) # 绘制workplace
    #  工件识别
    newPoints, x, y, w, h, count = findColor(img1_pt2_process, img3, myColors, myColorValues)  # 返回框框

    return img1_pt2_process, m, n, count