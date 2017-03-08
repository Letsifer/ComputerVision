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

Pyramid Pyramid::buildPyramid(MyImage& image, double basicSigma, int octavesNumber, int scalesNumber) {
    auto result = Pyramid(octavesNumber, scalesNumber);
    basicSigma = sqrt(firstImageSigma * firstImageSigma - basicSigma * basicSigma);
    auto basicImage = image.convoluton(
                Kernel::createXGaussKernel(basicSigma),
                BorderType::MirrorBorder
    ).convoluton(
                Kernel::createYGaussKernel(basicSigma),
                BorderType::MirrorBorder
    );
    auto elem = PyramidElement(basicImage, firstImageSigma);
    for (int i = 0; i < octavesNumber; i++) {
        for (int j = 0; j < scalesNumber; j++) {
            result.setElement(i, j, elem);
            if (j == scalesNumber - 1) {
                PyramidElement firstElementInNextOctaves = PyramidElement(elem.image.divideImage(), firstImageSigma);
                elem = firstElementInNextOctaves;
            } else {
                double nextSigma = elem.currentSigma * result.scalesInterval;
                double kernelParameter = sqrt(nextSigma * nextSigma - elem.currentSigma * elem.currentSigma);
                auto nextImage = elem.image.convoluton(
                            Kernel::createXGaussKernel(kernelParameter),
                            BorderType::MirrorBorder
                ).convoluton(
                            Kernel::createYGaussKernel(kernelParameter),
                            BorderType::MirrorBorder
                );
                PyramidElement next = PyramidElement(nextImage, nextSigma);
                elem = next;
            }
        }
    }
    return result;
}

void Pyramid::savePyramid(QString filename) {
    for (int i = 0; i < octaves; i++) {
        for (int j = 0; j < scales; j++) {
            QString temp = QString(filename);
            string text = "pyramid(" + to_string(i) + "-" + to_string(j) + ").jpg";
            getElement(i, j).image.save(temp.append(QString::fromStdString(text)));
        }
    }
}
