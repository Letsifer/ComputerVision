#ifndef RANSACALGORITHM_H
#define RANSACALGORITHM_H

#include "descriptormatcher.h"
#include <vector>
#include <math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>

class RansacAlgorithm
{
    constexpr static int K = 4; //число сопоставления для выборки
    constexpr static double EPS = 3; //погрешность в расстоянии между получившимися точками с матрицей гомографии и исходными точками
    //TODO remake it after remaking choose matches method
    constexpr static int MAX_ITERATIONS = 1; //максимальное количество итераций алгоритма

    constexpr static int GSL_MATRIX_ROWS = 2 * K, GSL_MATRIX_COLUMNS = 9;

    static vector<int> chooseMatches();
    static vector<double> findCurrentHomography(const vector<PointMatch>& matches);
    static double countPartOfCoordinate(const vector<double>& homography, int startIndex, const PointMatch& match);
public:
    RansacAlgorithm();
    static vector<double> findHomography(const vector<PointMatch>& matches);
};

#endif // RANSACALGORITHM_H
