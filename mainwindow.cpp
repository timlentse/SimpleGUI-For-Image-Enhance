/*******************************************
       添加需要用到的函数的头文件
*******************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore/QCoreApplication>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/opencv.hpp>
#include <opencv2/legacy/compat.hpp>
 int wid,Fun_selector;
 double c,r;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->doubleSpinBox->setEnabled(0);
    wid=ui->label->width();
    refresh();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//图像缩放子函数
void MainWindow::changsize(Mat & orig_image)
{    //using namespace cv; //使用名字空间，防止名字冲突
          double scalar;  //定义缩放比例
          int width,height;  //缩放后图像的宽高
          Mat dst ;
          scalar = max(orig_image.rows,orig_image.cols);
          scalar /= wid;
          height = (orig_image.rows /scalar);
          width  = (orig_image.cols /scalar);
          cv::resize(orig_image, dst, Size(width,height),CV_INTER_CUBIC);
          imwrite("change.tif",dst);  //保存缩放图
}

//以下的子函数是变换函数，就是说将采取对数，幂律，或者直方图均衡其中的一种做变换，具体采取哪种将取决用户的选择
void MainWindow::transform_selected( IplImage* orig_image)
{
    CvScalar scalar;
    IplImage * dst = orig_image ;
    if (Fun_selector==0)    //下拉菜单索引值为0，则代表选择log变换
    {
        for(int i=0;i<dst->height;i++)
        {
            for(int j=0;j<dst->width;j++)
            {
                scalar=cvGet2D(dst,i,j);
                scalar.val[0] = c*log10(scalar.val[0]+1);
                cvSet2D(dst,i,j,scalar);
            }
        }
    }

    else if (Fun_selector==1)  //下拉菜单索引值为1，则代表选择幂律变换
    {
        for(int i=0;i< dst->height;i++)
        {
            for(int j=0;j< dst->width;j++)
            {
                scalar= cvGet2D(dst,i,j);
                scalar.val[0] = c*pow(scalar.val[0],r);
                cvSet2D(dst,i,j,scalar);
            }
        }
    }
    else               //否则代表用户选择了直方图均衡
    {   IplImage* pro_image = cvCreateImage(cvGetSize(dst),8,1);
        cvEqualizeHist(dst,pro_image);
        cvSaveImage("temp.tif",pro_image);
    }
   cvSaveImage("temp.tif",dst);
}


void MainWindow::refresh(){
    Fun_selector= ui->comboBox->currentIndex();  //获取下拉菜单的索引值
    r= ui->doubleSpinBox->value();      //获取r值
    c= ui->doubleSpinBox_2->value();    //获取c值
    //第一步当然就是先载入原始图像，得到原图像的数据，作为变换函数的参数输入
    IplImage* orignal_image = cvLoadImage("/home/timlentse/Qtproject/Image/Fig1038(a)(noisy_fingerprint).tif",0);
    Mat orignal2 = imread("/home/timlentse/Qtproject/Image/Fig1038(a)(noisy_fingerprint).tif",0);

    changsize(orignal2);  //改变原图的尺寸，让原图在窗口中按一定的比例缩放

    QPixmap pix("change.tif");
    ui->label->setPixmap(pix);  //显示缩放后的原图图像

    Hist_image(orignal_image);     //画出原图的直方图

    QPixmap pix2("Histogram.tif");
    ui->label_2->setPixmap(pix2);  //显示直方图


    //进行对数变换，或者是幂律变换，具体实际是进行那种变换取决于Fun_tran的值，0为对数，1为幂律,2为直方图均衡
    transform_selected(orignal_image);

    IplImage *tram_image = cvLoadImage("temp.tif",0); //读取变换后的图像数据
    Hist_image(tram_image);     //把变换后的图像作为参数输入，得出它的直方图
    Mat orignal=imread("temp.tif",0);
    changsize(orignal);

    QPixmap pix3("change.tif");
    ui->label_3->setPixmap(pix3);  //显示 变换后的图

    QPixmap pix4("Histogram.tif");
    ui->label_4->setPixmap(pix4); //显示直方图
}

//槽函数，当且仅当用户鼠标点击apply button 才会触发这个函数的执行
void MainWindow::on_pushButton_clicked()
{
    refresh();
}



//同样是一个槽函数当下拉菜单索引值发生改变，则执行这个程序
void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    if(index==0)     //当索引值为0，表示用户选择了log ,r输入框需要变灰，enable 值为0
    {
        ui->doubleSpinBox->setEnabled(0);
        ui->doubleSpinBox_2->setEnabled(1);
    }
    else if(index==1)  //当索引值为1，表示用户选择了幂律 ，r,c输入都要激活
    {
        ui->doubleSpinBox->setEnabled(1);
        ui->doubleSpinBox_2->setEnabled(1);
    }
    else               //当索引值为2，表示用户选择了直方图均衡 ，r,c输入都要变灰
    {
        ui->doubleSpinBox->setEnabled(0);
        ui->doubleSpinBox_2->setEnabled(0);
    }
}



//直方图均衡变换函数
void MainWindow::Hist_image(IplImage *target_image)
{
    float range[] = {0,255};  //直方图的灰度级的范围（以8bit图为例）,range是一个数组
    float* ranges[]={range};  //凡是变量前加了星号，都代表定义该变量是指针变量，就像ranges

    //不妨先定义我们将要画的直方图的尺寸，256×256
    int hist_width=256;
    int hist_height=256;

    //原图转换为灰度图
    IplImage* gray_image = target_image;
    //获取灰度图一维直方图的数据，统计图像在[0 255]像素值的数据情况，也就是说灰度为0的像素有多少个
    //，像素为1的像素点有多少个。。。。。如此类推，这些数据都将会存在gray_hist里面
    CvHistogram* gray_hist = cvCreateHist(1,&hist_width,CV_HIST_ARRAY,ranges,1);
    cvCalcHist(&gray_image,gray_hist,0,0);  //计算灰度图像的一维直方图,修改*gray_image可以获得原来图像的直方图


    //归一化直方图
    cvNormalizeHist(gray_hist,1.0);
    int scale = 2;
    //创建一张一维直方图的“图”，横坐标设为灰度级别，纵坐标为像素个数（*scale）
    IplImage* hist_image = cvCreateImage(cvSize(hist_width*scale,hist_height),8,3);
    cvZero(hist_image);
    //统计直方图中的最大直方块
    float max_value = 0;
    cvGetMinMaxHistValue(gray_hist, 0,&max_value,0,0);
    //分别将每个直方块的值绘制到hist_image中
    for(int i=0;i<hist_width;i++)
    {
        float bin_val = cvQueryHistValue_1D(gray_hist,i); //这里可以把bin_val理解为灰度级为i的概率
        int intensity = cvRound(bin_val*hist_height/max_value);  //要绘制的高度
        cvRectangle(  hist_image,
                      cvPoint(i*scale,hist_height-1),
                      cvPoint((i+1)*scale - 1, hist_height-intensity),
                      CV_RGB(255,255,0));
    }
    cvSaveImage("Histogram.tif",hist_image); //保存直方图
}
