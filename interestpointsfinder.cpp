#include "interestpointsfinder.h"
#include <qdebug.h>

vector<InterestingPoint> InterestPointsFinder::moravecAlgorithm(
            const MyImage& image, const int windowsShift,
            const int sizeOfWindow, const double contrastBorder,
            const BorderType type
            ) {
    const int halfSizeOfWindow = sizeOfWindow / 2;
    MyImage contrastImage = findContrastsImageForMoravecAlgorithm(
                image, windowsShift, halfSizeOfWindow, type
                );
    return findInteresingPointsFromContrastImage(contrastImage, contrastBorder, type, halfSizeOfWindow);
}

MyImage InterestPointsFinder::findContrastsImageForMoravecAlgorithm(
        const MyImage& image, const int windowsShift,
        const int halfSizeOfWindow, const BorderType type
        ) {
    const int height = image.getHeight(), width = image.getWidth();
    MyImage result = MyImage(height, width);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double minError = numeric_limits<double>::max();
            for (int dx = -windowsShift; dx <= windowsShift; dx++) {
                for (int dy = -windowsShift; dy <= windowsShift; dy++) {
                    if (dx == 0 && dy == 0) {
                       continue;
                    }
                    double error = 0;
                    for (int u = -halfSizeOfWindow; u <= halfSizeOfWindow; u++) {
                        for (int v = -halfSizeOfWindow; v <=halfSizeOfWindow; v++) {
                            double imagePixel = image.getBorderPixel(i + v, j + u, type);
                            double shiftImagePixel = image.getBorderPixel(i + v + dy, j + u + dx, type);
                            double difference = imagePixel - shiftImagePixel;
                            error += difference * difference;
                        }
                    }
                    minError = min(minError,error);
                }
            }
            result.setPixel(i, j, minError);
        }
    }
    return result;
}

double InterestPointsFinder::computeHarrisInOnePoint(const MyImage &image,
                                                     const int i, const int j, const double sigma) {
    auto gauss = Kernel::createGaussKernel(sigma);
    auto kernelSobelX = Kernel::createXSobelKernel(),
         kernelSobelY = Kernel::createYSobelKernel();
    const double k = 0.06;
    double A = 0, B = 0, C = 0;
    const int halfWidthOfKernel = gauss.getWIdth() / 2;
    for (int u = -halfWidthOfKernel; u <= halfWidthOfKernel; u++) {
        for (int v = -halfWidthOfKernel; v <= halfWidthOfKernel; v++) {
            double fromGauss = gauss.getElementInRelationToCenter(v, u),
                   fromGradientX = image.pixelConvolution(i + u, j + v, kernelSobelX, BorderType::MirrorBorder),
                   fromGradientY = image.pixelConvolution(i + u, j + v, kernelSobelY, BorderType::MirrorBorder);
            A += fromGauss * fromGradientX * fromGradientX;
            B += fromGauss * fromGradientX * fromGradientY;
            C += fromGauss * fromGradientY * fromGradientY;
        }
    }
//    double descr = sqrt((A - C) * (A - C) + 4*B*B);
//    return min(abs((A + C - descr) / 2), abs((A + C + descr)/ 2));

    const double determinant = A * C - B * B,
                 trace = A + C;
    double contrast = determinant - k * trace * trace;
    return abs(contrast);
}

vector<InterestingPoint> InterestPointsFinder::harrisAlgorithm(
            const MyImage& image, const int windowSize, const double contrastBorder, const BorderType type
            ) {
    auto gradientX = image.convoluton(Kernel::createXSobelKernel(), type);
    auto gradientY = image.convoluton(Kernel::createYSobelKernel(), type);
    const int height = image.getHeight(), width = image.getWidth();
    MyImage contrastImage = MyImage(height, width);
    const double k = 0.06;
    const int halfWidthOfKernel = windowSize / 2;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double A = 0, B = 0, C = 0;
            for (int u = -halfWidthOfKernel; u <= halfWidthOfKernel; u++) {
                for (int v = -halfWidthOfKernel; v <= halfWidthOfKernel; v++) {
                    double fromGauss = Kernel::canculateGaussInOnePoint(windowSize / 6, u, v),
                           fromGradientX = gradientX.getBorderPixel(i + u, j + v, type),
                           fromGradientY = gradientY.getBorderPixel(i + u, j + v, type);
                    A += fromGauss * fromGradientX * fromGradientX;
                    B += fromGauss * fromGradientX * fromGradientY;
                    C += fromGauss * fromGradientY * fromGradientY;
                }
            }
            const double determinant = A * C - B * B,
                         trace = A + C;
            double contrast = determinant - k * trace * trace;
            contrastImage.setPixel(i, j, contrast);
        }
    }
    return findInteresingPointsFromContrastImage(contrastImage, contrastBorder, type, halfWidthOfKernel);
}

vector<InterestingPoint> InterestPointsFinder::findInteresingPointsFromContrastImage(
            const MyImage& contrastImage, double contrastBorder, BorderType type, int halfSizeOfWindow
            ) {
    vector<InterestingPoint> result;
    const int height = contrastImage.getHeight(), width = contrastImage.getWidth();
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (contrastBorder > contrastImage.getPixel(i, j)) {
                continue;
            }
            bool isLocalMaximum = true;
            for (int y = -halfSizeOfWindow; y <= halfSizeOfWindow; y++) {
                for (int x = -halfSizeOfWindow; x <= halfSizeOfWindow; x++) {
                    if (x == 0 && y == 0) {
                        continue;
                    }
                    if (contrastImage.getBorderPixel(i + y, j + x, type) > contrastImage.getPixel(i, j)) {
                        isLocalMaximum = false;
                        break;
                    }
                }
                if (!isLocalMaximum) {
                    break;
                }
            }
            if (isLocalMaximum) {
                result.emplace_back(j, i, contrastImage.getPixel(i, j));
            }
        }
    }
    return result;
}

void InterestPointsFinder::adaptiveNonMaximumSuppression(
        vector<InterestingPoint>& points,
        const unsigned int necessaryPoints
        ) {
    if (points.size() <= necessaryPoints) {
        return;
    }
    const double filterCoefficient = 1;
    for (unsigned int i = 0; i < points.size(); i++) {
        for (unsigned int j = 0; j < points.size(); j++) {
            if (i == j) {
                continue;
            }
            if (points[i].contrast * filterCoefficient >= points[j].contrast) {
                points[j].radiusToGreaterContrast = min(points[j].radiusToGreaterContrast, points[i].getDistance(points[j]));
            }
        }
    }
    sort(points.begin(), points.end(),
         [](const auto &point1, const auto &point2) {
        return point1.radiusToGreaterContrast > point2.radiusToGreaterContrast;
    });
    points.resize(necessaryPoints);
}
