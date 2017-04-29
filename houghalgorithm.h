#ifndef HOUGHALGORITHM_H
#define HOUGHALGORITHM_H
#define _USE_MATH_DEFINES
#include <vector>
#include <cmath>
#include "descriptormatcher.h"
using namespace std;

struct HoughTransforamtion {
    double x, y, scale, angle;
    HoughTransforamtion(double x, double y, double angle, double scale) :
        x(x), y(y), angle(angle), scale(scale){}
};

class HoughAlgorithm
{
    constexpr static int coordinatesDelimeter = 20;
    constexpr static int anglesBins = 6;
    constexpr static int scalesBins = 6;
    constexpr static double scaleStart = 1.0 / 8;
    static void fillIndexVector(vector<int>& vector, int value, int bins, int delimeter);
    static void fillAngleVector(vector<int>& vector, double value, int bins, double angleDelimeter);
    static int countIndex(int x, int y, int a, int s, int binsY);
public:
    HoughAlgorithm();
    static HoughTransforamtion getObjectsParameters(const vector<PointMatch>& matches,
                                                         int heightOfFirstImage, int widthOfFirstImage,
                                                         int heightOfSecondImage, int widthOfSecondImage
                                                        );
};

#endif // HOUGHALGORITHM_H
