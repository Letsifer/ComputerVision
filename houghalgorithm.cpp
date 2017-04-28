#include "houghalgorithm.h"

HoughAlgorithm::HoughAlgorithm()
{

}

HoughTransforamtion HoughAlgorithm::getObjectsParameters(const vector<PointMatch> &matches, int heightOfFirstImage, int widthOfFirstImage,
                                                         int heightOfSecondImage, int widthOfSecondImage) {
       const int sizeOFirstfImage = heightOfFirstImage * widthOfFirstImage;
       const int sizeOfSecondImage = heightOfSecondImage * widthOfSecondImage;
       const int firstImageCenterX = widthOfFirstImage / 2,
                 firstImageCenterY = heightOfFirstImage / 2;
       const int binsX = widthOfSecondImage / coordinatesDelimeter,
                 binsY = heightOfSecondImage / coordinatesDelimeter;
       unique_ptr<double[]> borders = make_unique<double[]>(scalesBins + 1);
       //start from 1/8 to 8, 6 bins and 7 borders
       for (int i = 0, factor = 1; i < scalesBins + 1; i++, factor *= 2) {
           double value = scaleStart * factor;
           borders[i] = value;
       }
       unique_ptr<double[]> centers = make_unique<double[]>(scalesBins);
       for (int i = 0; i < scalesBins; i++) {
           double value = (borders[i] + borders[i + 1]) / 2;
           centers[i] = value;
       }
       const int size = binsX * binsY * anglesBins * scalesBins;
       unique_ptr<int[]> accumulator = make_unique<int[]>(size);
       for (int i = 0; i < size; i++) {
           accumulator[i] = 0;
       }

       int currentMax = 0;
       int maxX = 0, maxY = 0, maxA = 0, maxS = 0;
       vector<int> indexesX, indexesY, indexesS, indexesA;
       for (const PointMatch& match : matches) {
           //переводим дескриптор в пространство относительно центра левого изображения
           const int x1 = match.firstX - firstImageCenterX,
                     y1 = match.firstY - firstImageCenterY;
           const int length = hypot(x1, y1);
           const double a1 = match.firstOrientation;
//                        s1 = sizeOFirstfImage / ;
//           const double secondScale = sizeOFirstfImage / ;
           //"раскодируем" параметры исходного объекта в пространство правого изображения
           double angleDecode = a1 - match.secondOrientation;
           const double scaleDecode = match.secondScale / match.firstScale,
                        xDecode = match.secondX + length * scaleDecode * cos(angleDecode),
                        yDecode = match.secondY + length * scaleDecode * sin(angleDecode);
           if (angleDecode < 0) {
               angleDecode += 2 * M_PI;
           }
           if (angleDecode > 2 * M_PI) {
               angleDecode -= 2 * M_PI;
           }
           if (xDecode < 0  || xDecode >= widthOfSecondImage) {
               continue;
           }
           if (yDecode < 0 && yDecode >= heightOfSecondImage) {
               continue;
           }
           fillIndexVector(indexesX, xDecode, binsX, coordinatesDelimeter);
           fillIndexVector(indexesY, yDecode, binsY, coordinatesDelimeter);
           fillAngleVector(indexesA, angleDecode, anglesBins, 2 * M_PI / anglesBins);

           for (int i = 0; i < scalesBins; i++) {
               if (borders[i] <= scaleDecode && scaleDecode < borders[i + 1]) { //here
                   indexesS.push_back(i);
                   if (scaleDecode > centers[i]) {
                       //take bin right to current, if current bin is not the last
                       if (i < scalesBins - 1) {
                           indexesS.push_back(i + 1);
                       }
                   } else {
                       //take bin left to current, if current bin is not the first
                       if (i > 0) {
                           indexesS.push_back(i - 1);
                       }
                   }
                   break;
               }
           }

           for (const int x : indexesX) {
               for (const int y : indexesY) {
                   for (const int a : indexesA) {
                       for (const int s : indexesS) {
                           int index = countIndex(x, y, a, s, binsY);
                           accumulator[index]++;
                           //mb rewrite
                           if (accumulator[index] > currentMax) {
                               currentMax = accumulator[index];
                               maxX = x;
                               maxY = y;
                               maxA = a;
                               maxS = s;
                           }
                       }
                   }
               }
           }

           indexesX.clear();
           indexesY.clear();
           indexesA.clear();
           indexesS.clear();
       }
       //take the center ob bin
       HoughTransforamtion t = HoughTransforamtion(
                   (maxX + 0.5) * coordinatesDelimeter,
                   (maxY + 0.5) * coordinatesDelimeter,
                   (maxA + 0.5) * M_PI / anglesBins,
                   centers[maxS]
                   );
       return t;
}

int HoughAlgorithm::countIndex(int x, int y, int a, int s, int binsY) {
    int index = x * binsY * anglesBins * scalesBins +
                y * anglesBins * scalesBins +
                a * scalesBins +
                s;
    return index;
}

void HoughAlgorithm::fillIndexVector(vector<int> &vector, const double value, const int bins, const int delimeter) {
    double valueX = value / delimeter;
    int indexX = valueX;
    vector.push_back(indexX);
    if (valueX > 0.5 && valueX < bins - 0.5) {//take two indexes
        if (valueX - indexX >= 0.5) {
            vector.push_back(indexX + 1);
        } else {
            vector.push_back(indexX - 1);
        }
    }
}

void HoughAlgorithm::fillAngleVector(vector<int> &vector, const double value, const int bins, const double angleDelimeter) {
    double valueX = value / angleDelimeter;
    int indexX = valueX;
    vector.push_back(indexX);
    if (valueX - indexX >= 0.5) {
        vector.push_back((indexX + 1) % bins);
    } else {
        vector.push_back((indexX - 1 + bins) % bins);
    }
}
