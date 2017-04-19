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

Pyramid::Pyramid(const MyImage& image, int octavesNumber, int scalesNumber) {
    this->octaves = octavesNumber;
    this->scales = scalesNumber + 3;
    this->images = make_unique<PyramidElement[]>((size_t) (octaves * scales));
    this->scalesInterval = pow(2, 1.0 / scalesNumber);
    const double kernelParameter = sqrt(firstImageSigma * firstImageSigma - basicSigma * basicSigma);
    auto basicImage = image.convoluton(
                Kernel::createXGaussKernel(kernelParameter),
                BorderType::MirrorBorder
    ).convoluton(
                Kernel::createYGaussKernel(kernelParameter),
                BorderType::MirrorBorder
    );
    setElement(0, 0, PyramidElement(basicImage, firstImageSigma, firstImageSigma));
    for (int i = 0; i < octaves; i++) {
        for (int j = 0; j < scales; j++) {
            if (j == 0) {
                if (i == 0) {
                    continue;
                }
                auto& latest = getElement(i - 1, scalesNumber - 1);
                double nextGlobalSigma = latest.globalSigma * scalesInterval;
                setElement(i, j, PyramidElement(latest.image.divideImage(), firstImageSigma, nextGlobalSigma));
            } else {
                auto& latest = getElement(i, j - 1);
                double nextSigma = latest.currentSigma * scalesInterval;
                double nextGlobalSigma = latest.globalSigma * scalesInterval;
                double kernelParameter = sqrt(nextSigma * nextSigma - latest.currentSigma * latest.currentSigma);
                auto nextImage = latest.image.convoluton(
                            Kernel::createXGaussKernel(kernelParameter),
                            BorderType::MirrorBorder
                ).convoluton(
                            Kernel::createYGaussKernel(kernelParameter),
                            BorderType::MirrorBorder
                );
                setElement(i, j, PyramidElement(nextImage, nextSigma, nextGlobalSigma));
            }
        }
    }
}

vector<BlobsCenter> Pyramid::findExtremums() const {
    vector<BlobsCenter> result;
    for (int octave = 0; octave < octaves; octave++) {
        for (int scale = 1; scale < scales - 1; scale++) {
            const double sigma = getElement(octave, scale).currentSigma;
            const int width = getElement(octave, scale).image.getWidth(),
                     height = getElement(octave, scale).image.getHeight();
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    if (isExtremum(octave, scale, i, j)) {
                        result.emplace_back(octave, scale, j, i, sigma);
                    }
                }
            }
        }
    }
    return result;
}

bool Pyramid::isExtremum(const int octave, const int scale, const int i, const int j) const {
    const double value = getElement(octave, scale).image.getPixel(i, j);
    if (abs(value) < EPS) {
        return false;
    }
    bool isMin = true, isMax = true;
    for (int z = scale - 1; z <= scale + 1; z++) {
        for (int y = i - 1; y <= i + 1; y++) {
            for (int x = j - 1; x <= j + 1; x++) {
                if (z == scale && y == i && x == j) {
                    continue;
                }
                double compared = getElement(octave, z).image.getBorderPixel(y, x, BorderType::MirrorBorder);
                if (compared > value) {
                    isMax = false;
                }
                if (value > compared) {
                    isMin = false;
                }
                if (!isMin && !isMax) {
                    return false;
                }
            }
        }
    }
    return isMin || isMax;
}

Pyramid Pyramid::createDogPyramid() const {
    Pyramid result = Pyramid(octaves, scales - 1);
    for (int i = 0; i < octaves; i++) {
        for (int j = 0; j < scales - 1; j++) {
            const auto difference = getElement(i, j + 1).image.subtract(getElement(i, j).image);
            const double localSigma = getElement(i, j).currentSigma,
                         globalSigma = getElement(i, j).globalSigma;
            result.setElement(i, j, PyramidElement(difference, localSigma, globalSigma));
        }
    }
    return result;
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
