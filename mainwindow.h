#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "pyramid.h"
#include "descriptormatcher.h"
#include "interestpointsfinder.h"
#include "MyImage.h"
#include "descriptorfinder.h"
#include "ransacalgorithm.h"

#include <QTextStream>
#include <QMainWindow>
#include <iostream>
#include <memory>
#include <vector>
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
    void workWithImageInThirdLab(const QString filename, const QImage& image);
    void printForThirdLab(QImage image, const vector<InterestingPoint> points, const QString filename);

    void lab4();

    void findBlobs();

    QImage drawMatches(const QImage& image1, const QImage& image2, const vector<PointMatch>& matches);
    void ransac();
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
