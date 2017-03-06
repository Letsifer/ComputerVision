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
    this->scalesInterval = pow(2, 1 / scales);
}

Pyramid Pyramid::buildPyramid(MyImage& image, double basicSigma, int octavesNumber, int scalesNumber) {
    auto result = Pyramid(octavesNumber, scalesNumber);
    auto basicImage = image.convoluton(
                Kernel::createGaussKernel(basicSigma),
                BorderType::MirrorBorder
    );
    auto elem = PyramidElement(basicImage, basicSigma);
    for (int i = 0; i < octavesNumber; i++) {
        for (int j = 0; j < scalesNumber; j++) {
            result.setElement(i, j, elem);
            double sigma = elem.currentSigma * result.scalesInterval;
            auto nextImage = elem.image.convoluton(
                        Kernel::createGaussKernel(sigma),
                        BorderType::MirrorBorder
            );
            if (j == scalesNumber - 1) {
                auto image = nextImage.divideImage();
                nextImage = image;
            }
            PyramidElement next = PyramidElement(nextImage, sigma);
            elem = next;
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
