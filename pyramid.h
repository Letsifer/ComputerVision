#ifndef PYRAMID_H
#define PYRAMID_H
#include"MyImage.h"
#include <iostream>
#include <memory>
using namespace std;

struct PyramidElement {
    MyImage image;
    double currentSigma;
    double globalSigma;

    PyramidElement() {
        this->currentSigma = -1;
        this->globalSigma = -1;
    }

    PyramidElement(MyImage image, double currentSigma, double globalSigma) {
        this->image = image;
        this->currentSigma = currentSigma;
        this->globalSigma = globalSigma;
    }
};

class Pyramid
{
    constexpr static double firstImageSigma = 1.6;
    double scalesInterval;
    int octaves, scales;
    unique_ptr<PyramidElement[]> images;

    Pyramid();
    Pyramid(int octaves, int scales);
    void setElement(int i, int j, PyramidElement element) {
        images[i * scales + j] = element;
    }
public:
    Pyramid(const MyImage& image, double basicSigma, int octavesNumber, int scalesNumber);
    void savePyramid(const QString filename);
    PyramidElement& getElement(int i, int j) const{
        return images[i * scales + j];
    }
};

#endif // PYRAMID_H
