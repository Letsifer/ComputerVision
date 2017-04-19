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

    PyramidElement() : currentSigma(-1), globalSigma(-1){}
    PyramidElement(MyImage image, double currentSigma, double globalSigma) : image(image), currentSigma(currentSigma), globalSigma(globalSigma){}
};

struct BlobsCenter {
    int octave, scale;
    int x, y;
    double sigma;
    BlobsCenter(int octave, int scale, int x, int y, double sigma) : octave(octave), scale(scale), x(x), y(y), sigma(sigma){}
};

class Pyramid
{
    constexpr static double firstImageSigma = 1.6, basicSigma = 0.5;
    constexpr static double EPS = 1e-5;
    double scalesInterval;
    int octaves, scales;
    unique_ptr<PyramidElement[]> images;

    Pyramid();
    Pyramid(int octaves, int scales);
    void setElement(int i, int j, PyramidElement element) {
        images[i * scales + j] = element;
    }
    bool isExtremum(int octave, int scale, int i, int j) const;
public:
    Pyramid(const MyImage& image, int octavesNumber, int scalesNumber);
    Pyramid createDogPyramid() const;
    vector<BlobsCenter> findExtremums() const;
    void savePyramid(const QString filename);
    const PyramidElement& getElement(int i, int j) const{
        return images[i * scales + j];
    }
};

#endif // PYRAMID_H
