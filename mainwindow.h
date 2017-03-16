#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <memory>
#include "MyImage.h"
#include <vector>
#include "interestpointsfinder.h"
#include <QDir>
#include <QPainter>
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    void lab1();
    void lab2();
    void lab3();
    void workWithImageInThirdLab(const QString filename, const QPixmap pixmap);
    void printForThirdLab(const QPixmap pix, const vector<InterestingPoint> points, const QString filename);
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
