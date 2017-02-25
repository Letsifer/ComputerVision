#include "ui_mainwindow.h"
#include "kernel.h"
#include <iostream>
#include <memory>
#include <cmath>
using namespace std;
#ifndef IMAGE_H
#define IMAGE_H

enum class BorderType{BlackBorder, WhiteBorder, MirrorBorder, CopyBorder};

class MyImage
{
    constexpr static double BLACK_BORDER = 0, WHITE_BORDER = 255;
    int width, height;
    unique_ptr<double[]> pixels;
    void setPixel(int i, int j, double value);
    double getBorderPixel(int i, int j, const BorderType borderType) const;
    double pixelConvolution(int y, int x, const Kernel* kernel, const BorderType borderType) const;
    bool isXInRange(int value) const{
        return value >= 0 && value < width;
    }
    bool isYInRange(int value) const{
        return value >= 0 && value < height;
    }
    bool areNotInRange(int x, int y) const{
        return !isXInRange(x) || !isYInRange(y);
    }
    double convertToAnotherRange(double value, double oldMin, double newMin, double coefficient);


public:
    static unique_ptr<MyImage> createMyImageFromQImage(const QImage qImage);
    MyImage();
    MyImage(int height, int width);
    double getPixel(int i, int j) const;
    unique_ptr<MyImage> convoluton(const Kernel* kernel, BorderType borderType);
    shared_ptr<MyImage> normalize(double newMin, double newMax);
    unique_ptr<MyImage> countHypotenuse(const MyImage* other);
    bool save(const QString filename) const;
    QImage createQImageFromImage() const;
};

#endif // IMAGE_H
