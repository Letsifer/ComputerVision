#include "ui_mainwindow.h"
#include "kernel.h"
#include <iostream>
#include <memory>
#include <cmath>
using namespace std;
#ifndef IMAGE_H
#define IMAGE_H

enum class BorderType{BlackBorder, WhiteBorder, MirrorBorder};

class MyImage
{
    const double BLACK_BORDER = 255, WHITE_BORDER = 0;
    int width, height;
    unique_ptr<double[]> pixels;
    //double* pixels; //TODO change to smart pointers
    void setPixel(int i, int j, double value);
    double getBorderPixel(int i, int j, const BorderType borderType);
    double pixelConvolution(int y, int x, const Kernel& kernel, const BorderType borderType);
    bool isXInRange(int value) {
        return value >= 0 && value < width;
    }
    bool isYInRange(int value) {
        return value >= 0 && value < height;
    }
    bool areNotInRange(int x, int y) {
        return !isXInRange(x) || !isYInRange(y);
    }

public:
    MyImage();
    MyImage(int height, int width);
    MyImage(int height, int width, const QImage image);
    MyImage& operator=(MyImage &&other);
    double getPixel(int i, int j) const;
    unique_ptr<MyImage> convoluton(const Kernel& kernel, BorderType borderType);
    unique_ptr<MyImage> normalize();
    unique_ptr<MyImage> countHypotenuse(const MyImage& other);
    bool save(const QString filename) const;
    QImage createQImageFromImage() const;
};

#endif // IMAGE_H
