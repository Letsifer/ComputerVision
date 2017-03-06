#ifndef PYRAMID_H
#define PYRAMID_H
#include"MyImage.h"
#include <iostream>
#include <memory>
using namespace std;

struct PyramidElement {
    MyImage image;
    double currentSigma;

    PyramidElement() {
        this->currentSigma = -1;
    }

    PyramidElement(MyImage image, double sigma) {
        this->image = image;
        this->currentSigma = sigma;
    }

//    PyramidElement& operator=(PyramidElement& sample) {
//        if (this == &sample) {
//            return *this;
//        }
//        currentSigma = sample.currentSigma;
//        image = sample.image;
//        return *this;
//    }
};

class Pyramid
{
    double scalesInterval;
    int octaves, scales;
    unique_ptr<PyramidElement[]> images;

    Pyramid();
    Pyramid(int octaves, int scales);
    void setElement(int i, int j, PyramidElement element) {
        images[i * octaves + j] = element;
    }
    PyramidElement getElement(int i, int j) {
        return images[i * octaves + j];
    }

public:
    static Pyramid buildPyramid(MyImage& image, double basicSigma, int octavesNumber, int scalesNumber);
    void savePyramid(QString filename);
};

#endif // PYRAMID_H
