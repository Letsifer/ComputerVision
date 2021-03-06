#include "ransacalgorithm.h"

RansacAlgorithm::RansacAlgorithm()
{

}

vector<double> RansacAlgorithm::findHomography(const vector<PointMatch> &matches, vector<PointMatch>& result) {
    vector<int> inliers;
    vector<int> choices(matches.size());
    iota(choices.begin(), choices.end(), 0);
    vector<int> currentInliers;
    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        random_shuffle(choices.begin(), choices.end());
        auto homography = findCurrentHomography(matches, choices, K);
        const double h22 = homography.at(8);
        for (int i = 0; i < homography.size(); i++) {
            homography[i] = homography[i] / h22;
        }
        int index = 0;
        for (const PointMatch& match : matches) {
            const double denominator = countPartOfCoordinate(homography, 6, match);
            const double resultX = countPartOfCoordinate(homography, 0, match) / denominator;
            const double resultY = countPartOfCoordinate(homography, 3, match) / denominator;
            const double error = hypot(resultX - match.second.x, resultY - match.second.y);
            if (error < EPS) {
                currentInliers.push_back(index);
            }
            index++;
        }
        if (inliers.size() < currentInliers.size()) {
            inliers.clear();
            inliers.insert(inliers.end(), currentInliers.begin(), currentInliers.end());
        }
        currentInliers.clear();
    }
    auto homography = findCurrentHomography(matches, inliers, inliers.size());
    for (int i = 0; i < inliers.size(); i++) {
        PointMatch match = matches.at(inliers.at(i));
        result.push_back(PointMatch(match.first, match.second));
    }
    const double h22 = homography.at(8);
    for (int i = 0; i < homography.size(); i++) {
        homography[i] = homography[i] / h22;
    }
    return homography;
}

vector<double> RansacAlgorithm::findCurrentHomography(const vector<PointMatch> &matches, vector<int>& choices,
                                                      const int points) {
    gsl_matrix* matrix = gsl_matrix_calloc(2 * points, GSL_MATRIX_COLUMNS);
    for (int i = 0; i < points; i++) {
        const PointMatch& match = matches.at(choices.at(i));
        const int firstRowIndex = 2 * i, secondRowIndex = 2 * i + 1;
        gsl_matrix_set(matrix, firstRowIndex, 0, match.first.x);
        gsl_matrix_set(matrix, firstRowIndex, 1, match.first.y);
        gsl_matrix_set(matrix, firstRowIndex, 2, 1);
        gsl_matrix_set(matrix, firstRowIndex, 3, 0);
        gsl_matrix_set(matrix, firstRowIndex, 4, 0);
        gsl_matrix_set(matrix, firstRowIndex, 5, 0);
        gsl_matrix_set(matrix, firstRowIndex, 6, - match.second.x * match.first.x);
        gsl_matrix_set(matrix, firstRowIndex, 7, - match.second.x * match.first.y);
        gsl_matrix_set(matrix, firstRowIndex, 8, - match.second.x);

        gsl_matrix_set(matrix, secondRowIndex, 0, 0);
        gsl_matrix_set(matrix, secondRowIndex, 1, 0);
        gsl_matrix_set(matrix, secondRowIndex, 2, 0);
        gsl_matrix_set(matrix, secondRowIndex, 3, match.first.x);
        gsl_matrix_set(matrix, secondRowIndex, 4, match.first.y);
        gsl_matrix_set(matrix, secondRowIndex, 5, 1);
        gsl_matrix_set(matrix, secondRowIndex, 6, - match.second.y * match.first.x);
        gsl_matrix_set(matrix, secondRowIndex, 7, - match.second.y * match.first.y);
        gsl_matrix_set(matrix, secondRowIndex, 8, - match.second.y);
    }

    gsl_matrix* transposed = gsl_matrix_calloc(GSL_MATRIX_COLUMNS, 2 * points);
    gsl_matrix_transpose_memcpy(transposed, matrix);

    gsl_matrix* product = gsl_matrix_calloc(GSL_MATRIX_COLUMNS, GSL_MATRIX_COLUMNS);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, transposed, matrix, 0.0, product);

    //SV decomposition
    gsl_matrix* U = gsl_matrix_calloc(GSL_MATRIX_COLUMNS, GSL_MATRIX_COLUMNS);
    gsl_vector* sigmaVector = gsl_vector_calloc(GSL_MATRIX_COLUMNS);
    gsl_vector* workspace = gsl_vector_calloc(GSL_MATRIX_COLUMNS);
    gsl_linalg_SV_decomp(product, U, sigmaVector, workspace);

    auto indexOfMinElement = gsl_vector_min_index(sigmaVector);
    gsl_vector_view singularColumn = gsl_matrix_column(U, indexOfMinElement);

    vector<double> homography;
    for (int i = 0; i < GSL_MATRIX_COLUMNS; i++) {
        double value = gsl_vector_get(&singularColumn.vector, i);
        homography.push_back(value);
    }

    gsl_matrix_free(matrix);
    gsl_matrix_free(product);
    gsl_matrix_free(U);

    gsl_vector_free(sigmaVector);
    gsl_vector_free(workspace);

    return homography;
}

double RansacAlgorithm::countPartOfCoordinate(const vector<double> &homography, const int startIndex, const PointMatch &match) {
    return homography.at(startIndex) *
            match.first.x + homography.at(startIndex + 1) *
            match.first.y + homography.at(startIndex + 2);
}
