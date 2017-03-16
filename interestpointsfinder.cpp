#include "interestpointsfinder.h"

vector<InterestingPoint> InterestPointsFinder::moravecAlgorithm(
            const MyImage image, const int windowsShift,
            const int sizeOfWindow, const double contrastBorder,
            const BorderType type
            ) {
    const int halfSizeOfWindow = sizeOfWindow / 2;
    MyImage contrastImage = findContrastsImageForMoravecAlgorithm(
                image, windowsShift, halfSizeOfWindow, type
                );
    return findInteresingPointsFromContrastImage(contrastImage, contrastBorder, type, halfSizeOfWindow);
}

vector<InterestingPoint> InterestPointsFinder::harrisAlgorithm(
            const MyImage image, const double contrastBorder, const BorderType type
            ) {
    auto gradientX = image.convoluton(Kernel::createXSobelKernel(), type);
    auto gradientY = image.convoluton(Kernel::createYSobelKernel(), type);
    const int height = image.getHeight(), width = image.getWidth();
    MyImage contrastImage = MyImage(height, width);
    const double sigmaForWindow = 1, k = 0.06;
    Kernel gaussKernel = Kernel::createGaussKernel(sigmaForWindow);
    const int halfWidthOfKernel = gaussKernel.getKernelHalfHeight();
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double A = 0, B = 0, C = 0;
            for (int u = -halfWidthOfKernel; u < halfWidthOfKernel; u++) {
                for (int v = -halfWidthOfKernel; v < halfWidthOfKernel; v++) {
                    double fromGauss = gaussKernel.getElementInRelationToCenter(u, v),
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

MyImage InterestPointsFinder::findContrastsImageForMoravecAlgorithm(
        const MyImage image, const int windowsShift,
        const int halfSizeOfWindow, const BorderType type
        ) {
    const int height = image.getHeight(), width = image.getWidth();
    MyImage result = MyImage(height, width);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double minError = HUGE_VAL;
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

vector<InterestingPoint> InterestPointsFinder::findInteresingPointsFromContrastImage(
            const MyImage contrastImage, double contrastBorder, BorderType type, int halfSizeOfWindow
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
                InterestingPoint point = InterestingPoint(j, i, contrastImage.getPixel(i, j));
                result.push_back(point);
            }
        }
    }
    return result;
}

vector<InterestingPoint> InterestPointsFinder::adaptiveNonMaximumSuppression(
        const MyImage image,
        vector<InterestingPoint> points,
        const unsigned int necessaryPoints
        ) {
    if (points.size() <= necessaryPoints) {
        return points;
    }
    const double filterCoefficient = 0.9;
    const int maxRadius = max(image.getHeight(), image.getWidth()) / 2 + 1;
    for (int r = 1; r < maxRadius; r++) {
        for (int i = 0; i < points.size(); i++) {
            for (int j = 0; j < points.size(); j++) {
                if (i == j) {
                    continue;
                }
                if (points[i].getDistance(points[j]) <= r && points[i].contrast * filterCoefficient >= points[j].contrast) {
                    points.erase(points.begin() + j);
                    j--;
                    if (points.size() == necessaryPoints) {
                        return points;
                    }
                }
            }
        }
    }
    return points;
}
