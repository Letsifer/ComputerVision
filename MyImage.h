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
    double pixelConvolution(int y, int x, const Kernel& kernel, const BorderType borderType) const;
    bool isXInRange(int value) const{
        return value >= 0 && value < width;
    }
    bool isYInRange(int value) const{
        return value >= 0 && value < height;
    }
    bool areNotInRange(int x, int y) const{
        return !isXInRange(x) || !isYInRange(y);
    }
    double convertToAnotherRange(double value, double oldMin, double newMin, double coefficient) const;


public:
    MyImage& operator=(MyImage& sample) {
        if (this == &sample) {
            return *this;
        }
        width = sample.width;
        height = sample.height;
        pixels = move(sample.pixels);
        return *this;
    }

    static MyImage createMyImageFromQImage(const QImage qImage);
    MyImage();
    MyImage(int height, int width);
    MyImage(const MyImage &sample);
    double getPixel(int i, int j) const;
    MyImage convoluton(const Kernel& kernel, BorderType borderType) const;
    MyImage normalize(double newMin, double newMax) const;
    MyImage countHypotenuse(const MyImage& other) const;
    bool save(const QString filename);
    QImage createQImageFromImage();
    MyImage divideImage() const;
};

#endif // IMAGE_H
