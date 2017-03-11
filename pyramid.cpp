#include "pyramid.h"
#include <iostream>
#include <memory>
using namespace std;

Pyramid::Pyramid()
{
    octaves = 0;
    scales = 0;
    images = nullptr;
}

Pyramid::Pyramid(int octaves, int scales) {
    this->octaves = octaves;
    this->scales = scales;
    this->images = make_unique<PyramidElement[]>((size_t) (octaves * scales));
    this->scalesInterval = pow(2, 1.0 / scales);
}

Pyramid::Pyramid(const MyImage& image, double basicSigma, int octavesNumber, int scalesNumber) {
    this->octaves = octavesNumber;
    this->scales = scalesNumber;
    this->images = make_unique<PyramidElement[]>((size_t) (octaves * scales));
    this->scalesInterval = pow(2, 1.0 / scales);
    basicSigma = sqrt(firstImageSigma * firstImageSigma - basicSigma * basicSigma);
    auto basicImage = image.convoluton(
                Kernel::createXGaussKernel(basicSigma),
                BorderType::MirrorBorder
    ).convoluton(
                Kernel::createYGaussKernel(basicSigma),
                BorderType::MirrorBorder
    );
    auto elem = PyramidElement(basicImage, firstImageSigma, firstImageSigma);
    for (int i = 0; i < octavesNumber; i++) {
        for (int j = 0; j < scalesNumber; j++) {
            setElement(i, j, elem);
            double nextGlobalSigma = elem.globalSigma * scalesInterval;
            if (j == scalesNumber - 1) {
                elem = PyramidElement(elem.image.divideImage(), firstImageSigma, nextGlobalSigma);
            } else {
                double nextSigma = elem.currentSigma * scalesInterval;
                double kernelParameter = sqrt(nextSigma * nextSigma - elem.currentSigma * elem.currentSigma);
                auto nextImage = elem.image.convoluton(
                            Kernel::createXGaussKernel(kernelParameter),
                            BorderType::MirrorBorder
                ).convoluton(
                            Kernel::createYGaussKernel(kernelParameter),
                            BorderType::MirrorBorder
                );
                elem = PyramidElement(nextImage, nextSigma, nextGlobalSigma);
            }
        }
    }
}

void Pyramid::savePyramid(const QString filename) {
    for (int i = 0; i < octaves; i++) {
        for (int j = 0; j < scales; j++) {
            QString temp = QString(filename);
            PyramidElement element = getElement(i, j);
            string text = "pyramid(" + to_string(i) + "-" + to_string(j)
                    + "), local sigma=" + to_string(element.currentSigma)
                    + ", global sigma=" + to_string(element.globalSigma)
                    +  ".jpg";
            element.image.save(temp.append(QString::fromStdString(text)));
        }
    }
}
