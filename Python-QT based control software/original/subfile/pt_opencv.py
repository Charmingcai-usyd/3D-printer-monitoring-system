import sub_pentou1 as pt1
import sub_pentou2 as pt2
import cv2

"""""
                                       开发代码说明
        作者：charming cai
        功能介绍： 基于3D打印机喷头工作状态与工件识别的工程代码
        实现目标：
                 摄像头1主要正面检测喷头运动状态与工件是否出现轻微位移
                 摄像头2主要识别喷头是否出现堵头与出料过快
        实现过程：  
                 摄像头1主要基于颜色识别与运动抓取，在分割图像后对工件的边缘斜率进行判断
                 摄像头2主要基于直方图判断喷头是否出现堵头，并对最上层的物料进行质量检测
        
        需求变化： 
                2021.3.17 增加了串流流媒体服务器拉流与QT功能的引入，正式搭建起上位机架构
        
        开发日志： 
                2021.2.4         V1.1 实现摄像头1的风扇特征抓取
                2021.4.30        V1.2 实现接口功能实用化
                2021.5.1         V1.2.1 单目摄像头实现基本抓取功能 并接入rtmp视频流没有问题
                2021.5.2         v1.3.1 单目双目实现规定区域颜色喷头抓取
                2021.5.3         v1.4.2 基本实现全部功能 
                2021.5.4         v1.4.3 完成UI优化 增加了四边识别工件的功能 基本实现功能包装
                2021.5.5         v1.5.2 完成技术封装与代码规范化 完成部分全功能测试
                
"""

"""                                     ||
----------------------------------------\/----->
      |                  |beH            
      |                  |
      |                  |
 ---->|                  |            |<----wLen //结束点位  
      |                  |            |        
   beW|<-----------------|------------|     
      |                  |************| 
      |                  |****Image***|       
      |                  |____________|____       
      |                                 /\
                                        ||-hEnd //结束点位

beH   起点高
beW   起点
hEnd  终点高
wLen  高度
"""

"""                                           基本参数配置                                      """
test1 = cv2.VideoCapture("F:/毕设/视频/正面打印视频.mp4")# 导入单摄像头测试视频
#test1 = cv2.VideoCapture(1)# 导入单摄像头测试视频

# 切图参数
pt1_cut_img_w = 690
pt1_cut_img_l = 635
pt1_cut_img_b = 3
pt1_cut_color = (125, 125, 0)
pt2_cut_img_w = 690
pt2_cut_img_l = 635
pt2_cut_img_b = 3
pt2_cut_color = (125, 0, 255)

pt1_all_img_b = 3
pt1_all_color = (125, 125, 0)


"""                                           基本参数配置                                                             """

"""                                              主函数                                                               """
def run(pt_img,num_camara):
    staticmethod
    counting = 0
    flag = 0
    scan = 0

    if num_camara == 2:# 如果摄像头默认为一个
       # 分割图像pt1
       cv2.rectangle(pt_img, (pt1_cut_img_b, pt1_cut_img_b), (pt1_cut_img_l+pt1_cut_img_b,pt1_cut_img_w+pt1_cut_img_b), pt1_cut_color, pt1_cut_img_b)
     # resize_pt1_img = pt_img[pt1_cut_img_b:pt1_cut_img_w+pt1_cut_img_b,  pt1_cut_img_b:pt1_cut_img_l+pt1_cut_img_b]  # 裁剪每帧图片
     # cv2.imshow("resize_pt1_img",resize_pt1_img)
       # 分割图像 pt2
       cv2.rectangle(pt_img, (pt2_cut_img_b+pt2_cut_img_l, pt2_cut_img_b),(pt2_cut_img_b+pt1_cut_img_l+pt2_cut_img_l, pt2_cut_img_b+pt2_cut_img_w),pt2_cut_color, pt2_cut_img_b)
     # resize_pt2_img = pt_img[pt2_cut_img_b: pt1_cut_img_w+pt2_cut_img_w+pt2_cut_img_b,  pt2_cut_img_b+pt1_cut_img_l: pt1_cut_img_l+pt2_cut_img_l+pt2_cut_img_b]  ##裁剪每帧图片
     # cv2.imshow("resize_pt2_img",resize_pt2_img)
       # 文字标记
       img_pt1_process = pt1.sxt1(pt_img, counting, flag, scan, num_camara)# pt_img全图, resize_pt1_img切割图, counting, flag, scan
       # img_pt1_process 将图像传递到pt2子函数中 m n hsit与Hash值
       img1_pt1pt2_process, m, n, count = pt2.sxt2(img_pt1_process)

       cv2.rectangle(img1_pt1pt2_process, (pt1_cut_img_b + 3, pt1_cut_img_b + 3), (125, pt1_cut_img_b + 23), pt2_cut_color, -1)
       cv2.rectangle(img1_pt1pt2_process, (pt1_cut_img_b + pt2_cut_img_b + pt2_cut_img_l, pt1_cut_img_b + 3), (pt2_cut_img_b + pt2_cut_img_l + 125, pt1_cut_img_b + 23), pt1_cut_color, -1)
       cv2.putText(img1_pt1pt2_process,'Channel 1',(pt1_cut_img_b + 1, pt1_cut_img_b + 20), cv2.FONT_HERSHEY_COMPLEX, 0.7, pt1_cut_color, 2)
       cv2.putText(img1_pt1pt2_process,'Channel 2',(pt1_cut_img_b + pt2_cut_img_b + pt2_cut_img_l, pt1_cut_img_b + 20), cv2.FONT_HERSHEY_COMPLEX, 0.7, pt2_cut_color, 2)

       # 根据状态打标签

       m = 'Hash:%2d' % m
       cv2.putText(img1_pt1pt2_process, m, (pt1_cut_img_b + pt2_cut_img_b + pt2_cut_img_l, pt1_cut_img_b + 35), cv2.FONT_HERSHEY_COMPLEX, 0.5, (0, 0, 255),1)
       n = 'Hist:%3f' % n
       cv2.putText(img1_pt1pt2_process, n, (pt1_cut_img_b + pt2_cut_img_b + pt2_cut_img_l, pt1_cut_img_b + 50), cv2.FONT_HERSHEY_COMPLEX, 0.5, (168, 165, 32), 1)
       p = 'Defect:%3d' % count
       cv2.putText(img1_pt1pt2_process, p, (pt1_cut_img_b + pt2_cut_img_b + pt2_cut_img_l, pt1_cut_img_b + 65), cv2.FONT_HERSHEY_COMPLEX, 0.5, (32, 165, 32), 1)

       # pt1 pt2完成对视频流的图像绘制截取
       # cv2.imshow("img1_pt1pt2_process", img1_pt1pt2_process)  # 显示抓取后的图像 名称为喷头1处理图像
       # 弹出
       # 测试 分别显示图像
       pass

    if num_camara == 1:# 如果摄像头默认为一个
      success,img = test1.read()
      img1_pt1pt2_process = pt1.sxt1(img, counting, flag, scan, num_camara)#入口图像需要为视频流
      pass

    # if pt1.cv2.waitKey(1) & 0xFF == ord('q'):
    #   break
    if num_camara == 0:  # 调试时摄像头为0 导入测试视频
      img1_pt1pt2_process = pt_img
      pass

    cv2.setUseOptimized(True)
    # 使用官方API进行优化
    return img1_pt1pt2_process


