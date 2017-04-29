#include "descriptormatcher.h"

DescriptorMatcher::DescriptorMatcher()
{}

vector<PointMatch> DescriptorMatcher::findMatchersBetweenDescriptors(
                const vector<Descriptor>& firstVector,
                const vector<Descriptor>& secondVector
            ) {
    vector<PointMatch> result;
    for (const Descriptor& descriptor : firstVector) {
        double minDistance = numeric_limits<double>::max(), secondMinDistance = minDistance;
        int indexMin = -1;
        for (unsigned int j = 0, size = secondVector.size(); j < size; j++) {
            double distance = descriptor.getDistance(secondVector.at(j));
            if (minDistance > distance) {
                secondMinDistance = minDistance;
                minDistance = distance;
                indexMin = j;
                continue;
            }
            if (secondMinDistance > distance) {
                secondMinDistance = distance;
            }
        }
        const double rate = minDistance / secondMinDistance;
        if (rate <= T) {
            Descriptor second = secondVector.at(indexMin);
            result.push_back(PointMatch(
                                 descriptor.getPoint(),
                                 second.getPoint()
                                 ));
        }
    }
    return result;
}
