#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "pyramid.h"
#include "descriptormatcher.h"
#include "interestpointsfinder.h"
#include "MyImage.h"
#include "descriptorfinder.h"
#include "ransacalgorithm.h"
#include "houghalgorithm.h"

#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QMainWindow>
#include <iostream>
#include <memory>
#include <vector>
#include <QDir>
#include <QPainter>
#include <QGraphicsScene>
using namespace std;

namespace Ui {
class MainWindow;
struct ImageAndResult;
}

struct ImageAndResult {
    QString filename;
    QImage original;
    bool tryToFind;
    bool foundResult;
    QImage result;
    ImageAndResult(QString filename, QImage original) : filename(filename), original(original), tryToFind(false), foundResult(false){}
};

class MainWindow : public QMainWindow
{
    constexpr static int BORDER_TO_FIND_HOUGH_OBJECT = 3;
    const static QString foundResultText;
    const static QString notFoundResultText;
    const static QString notObjectFoundText;
    QImage object;
    /*
     * Vector that contains images and result. If tryToFind = true, than no more searches will start.
     */
    vector<ImageAndResult> images;

    Q_OBJECT
    void findBlobs();
    QImage drawMatches(const QImage& image1, const QImage& image2, const vector<PointMatch>& matches);
    void drawDescriptors(const QImage& image, const vector<Descriptor>& descriptors, const QString& name);
    void ransac();
    void hough(const QImage object, ImageAndResult& container);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onFindFileButtonClicked();
    void onFindFolderButtonClicked();
    void clickOnItem(const QModelIndex &index);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
