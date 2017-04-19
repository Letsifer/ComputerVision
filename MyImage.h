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
    MyImage& operator=(MyImage&& sample);
    MyImage& operator=(const MyImage& sample);
    int getHeight() const {
        return height;
    }
    int getWidth() const {
        return width;
    }
    static MyImage createMyImageFromQImage(const QImage qImage);
    MyImage();
    MyImage(int height, int width);
    MyImage(const MyImage &sample);
    double getPixel(int i, int j) const;
    double getBorderPixel(int i, int j, const BorderType borderType) const;
    void setPixel(int i, int j, double value);
    MyImage convoluton(const Kernel& kernel, BorderType borderType) const;
    double pixelConvolution(int y, int x, const Kernel& kernel, const BorderType borderType) const;
    MyImage normalize(double newMin, double newMax) const;
    MyImage countHypotenuse(const MyImage& other) const;
    bool save(const QString filename);
    QImage createQImageFromImage();
    MyImage divideImage() const;
    MyImage subtract(const MyImage other) const;
};

#endif // IMAGE_H
