#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <opencv2/highgui/highgui.hpp>
using namespace cv; //使用名字空间，防止名字冲突
namespace Ui
{
  class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void Hist_image(IplImage *target_image);
    void changsize(Mat &orig_image);
    void refresh();
    void transform_selected(IplImage* orig_image);

private slots:
    void on_pushButton_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

