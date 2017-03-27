#ifndef DESCRIPTORFINDER_H
#define DESCRIPTORFINDER_H
#include<cmath>
#include"MyImage.h"
#include <iostream>
using namespace std;

struct Basket {
    double value;
    double centerOfBasket;
    Basket() : value(0), centerOfBasket(0){};
    double getDistanceToCenter(double angle) const{
        double absolutDistance = abs(angle - centerOfBasket);
        return min(absolutDistance, 2 * M_PI - absolutDistance);
    }
};

struct Hystogramm {
    unique_ptr<Basket[]> baskets;
    int basketsNumber;
    Hystogramm(int basketsNumber) : basketsNumber(basketsNumber) {
        baskets = make_unique<Basket[]>((size_t)(basketsNumber));
        double basicValue = M_PI / basketsNumber;
        for (int i = 0; i < basketsNumber; i++) {
            baskets[i].centerOfBasket = (2 * i + 1) * basicValue;
        }
    }

    double getBasketValue(int i) const{
        return baskets[i].value;
    }

    pair<Basket&, Basket&> getNeighborsToPoint(double angle) const{
        int indexOfMin = -1, indexOfSecondMin = -1;
        double minValue = numeric_limits<double>::max(), secondMinValue = minValue;
        for (int i = 0; i < basketsNumber; i++) {
            double distanceToCenter = baskets[i].getDistanceToCenter(angle);
            if (minValue > distanceToCenter) {
                indexOfSecondMin = indexOfMin;
                secondMinValue = minValue;
                indexOfMin = i;
                minValue = distanceToCenter;
                continue;
            }
            if (secondMinValue > distanceToCenter) {
                indexOfSecondMin = i;
                secondMinValue = distanceToCenter;
            }
        }
        return pair<Basket&, Basket&>(baskets[indexOfMin], baskets[indexOfSecondMin]);
    }
};

class Descriptor
{
    int pointX, pointY;
    unique_ptr<double[]> elements;
    int sizeOfDescriptor;
    void normalize();
    Hystogramm findHystogrammForRegion(const MyImage& image,
                                       int leftX, int topY,
                                       int width, int height,
                                       int basketsInHystogramm
                                       );
    double countValueForBasket(double value, double angle, double center1, double center2);
    double findDistanceCoefficient(int x, int y);
public:
    Descriptor(const MyImage& image,
               int pointX, int pointY,
               int regionsX, int regionsY,
               int sizeOfRegionX, int sizeOfRegionY,
               int basketsInHystogramm
               );
    int getPointX() const {return pointX;}
    int getPointY() const {return pointY;}
    double getElement(int i) const{
        return elements[i];
    }
    int getSize() const {
        return sizeOfDescriptor;
    }
    double getDistance(const Descriptor& descriptor) const;
};

#endif // DESCRIPTORFINDER_H
