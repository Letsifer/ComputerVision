#ifndef INTERESTPOINTSFINDER_H
#define INTERESTPOINTSFINDER_H
#include "MyImage.h"
#include "cmath"
#include <vector>
using namespace std;

struct InterestingPoint {
    int x, y;
    double contrast;
    InterestingPoint(int x, int y, double contrast) {
        this->x = x;
        this->y = y;
        this->contrast = contrast;
    }
    int getX() const {
        return x;
    }
    int getY() const {
        return y;
    }
    double getDistance(const InterestingPoint point) const{
        double differenceX = x - point.x,
               differenceY = y - point.y;
        return sqrt(differenceX * differenceX + differenceY * differenceY);
    }

};

class InterestPointsFinder
{
    static MyImage findContrastsImageForMoravecAlgorithm(
            const MyImage image, const int windowsShift,
            const int halfSizeOfWindow, const BorderType type
            );
    static vector<InterestingPoint> findInteresingPointsFromContrastImage(
            const MyImage contrastImage, const double contrastBorder,
            const BorderType type, const int halfSizeOfWindow
            );
public:
    static vector<InterestingPoint> moravecAlgorithm(
            const MyImage image, const int windowsShift,
            const int sizeOfWindow, const double contrastBorder, const BorderType type
            );
    static vector<InterestingPoint> harrisAlgorithm(
            const MyImage image, const double contrastBorder, const BorderType type
            );
    static vector<InterestingPoint> adaptiveNonMaximumSuppression(
            const MyImage image,
            vector<InterestingPoint> points,
            const unsigned int necessaryPoints
            );

};

#endif // INTERESTPOINTSFINDER_H
