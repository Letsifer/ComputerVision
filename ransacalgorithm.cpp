#include "ransacalgorithm.h"

RansacAlgorithm::RansacAlgorithm()
{

}

vector<double> RansacAlgorithm::findHomography(const vector<PointMatch> &matches) {
    int bestInliers = -1;
    vector<double> result;
    vector<int> choices(matches.size());
    iota(choices.begin(), choices.end(), 0);
    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        auto homography = findCurrentHomography(matches, choices);
        //checking this homography
        int inliers = 0;
        assert(homography.size() == GSL_MATRIX_COLUMNS);
        for (const PointMatch& match : matches) {
            const double denominator = countPartOfCoordinate(homography, 6, match);
            const double resultX = countPartOfCoordinate(homography, 0, match) / denominator;
            const double resultY = countPartOfCoordinate(homography, 3, match) / denominator;
            const double error = hypot(resultX - match.secondX, resultY - match.secondY);
            if (error < EPS) {
                inliers++;
            }
        }

        if (inliers > bestInliers) {
            bestInliers = inliers;
            result.clear();
            result.insert(result.end(), homography.begin(), homography.end());
        }
    }
    return result;
}

vector<double> RansacAlgorithm::findCurrentHomography(const vector<PointMatch> &matches, vector<int>& choices) {
    random_shuffle(choices.begin(), choices.end());
    //TOOD try to remake on smart pointers or to view
    gsl_matrix* matrix = gsl_matrix_calloc(GSL_MATRIX_ROWS, GSL_MATRIX_COLUMNS);
    for (int i = 0; i < K; i++) {
        const PointMatch& match = matches.at(choices.at(i));
        const int firstRowIndex = 2 * i, secondRowIndex = 2 * i + 1;
        gsl_matrix_set(matrix, firstRowIndex, 0, match.secondX);
        gsl_matrix_set(matrix, firstRowIndex, 1, match.secondY);
        gsl_matrix_set(matrix, firstRowIndex, 2, 1);
        gsl_matrix_set(matrix, firstRowIndex, 3, 0);
        gsl_matrix_set(matrix, firstRowIndex, 4, 0);
        gsl_matrix_set(matrix, firstRowIndex, 5, 0);
        gsl_matrix_set(matrix, firstRowIndex, 6, - match.firstX * match.secondX);
        gsl_matrix_set(matrix, firstRowIndex, 7, - match.firstX * match.secondY);
        gsl_matrix_set(matrix, firstRowIndex, 8, - match.firstX);

        gsl_matrix_set(matrix, secondRowIndex, 0, 0);
        gsl_matrix_set(matrix, secondRowIndex, 1, 0);
        gsl_matrix_set(matrix, secondRowIndex, 2, 0);
        gsl_matrix_set(matrix, secondRowIndex, 3, match.secondX);
        gsl_matrix_set(matrix, secondRowIndex, 4, match.secondY);
        gsl_matrix_set(matrix, secondRowIndex, 5, 1);
        gsl_matrix_set(matrix, secondRowIndex, 6, - match.firstY * match.secondX);
        gsl_matrix_set(matrix, secondRowIndex, 7, - match.firstY * match.secondY);
        gsl_matrix_set(matrix, secondRowIndex, 8, - match.firstY);
    }

    gsl_matrix* transposed = gsl_matrix_calloc(GSL_MATRIX_COLUMNS, GSL_MATRIX_ROWS);
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

//    const int size = 3;
//    gsl_matrix* homographyInMatrix = gsl_matrix_calloc(size, size);
//    for (int i = 0, cnt = 0; i < size; i++) {
//        for (int j = 0; j < size; j++) {
//            gsl_matrix_set(homographyInMatrix, i, j, gsl_vector_get(&singularColumn.vector, cnt++));
//        }
//    }
//    int num;
//    gsl_matrix* inverseMatrix = gsl_matrix_calloc(size, size);
//    gsl_permutation* permutation = gsl_permutation_alloc(size);
//    gsl_linalg_LU_decomp(homographyInMatrix, permutation, &num);
//    gsl_linalg_LU_invert(homographyInMatrix, permutation, inverseMatrix);

//    product = gsl_matrix_calloc(size, size);
//    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, homographyInMatrix, inverseMatrix, 0.0, product);
//    for (int i = 0; i < size; i++) {
//        for (int j = 0; j < size; j++) {
//            double value = gsl_matrix_get(product, i, j);
//             if (i == j)
//                 assert(abs(value - 1) <= (1e-6));
//             else
//                 assert(abs(value) <= 1e-6);
//        }
//    }

    vector<double> homography;
    for (int i = 0; i < GSL_MATRIX_COLUMNS; i++) {
        double value = gsl_vector_get(&singularColumn.vector, i);
        homography.push_back(value);
    }

    const double h22 = homography.at(8);
    for (int i = 0; i < homography.size(); i++) {
        homography[i] = homography[i] / h22;
    }

    gsl_matrix_free(matrix);
//    gsl_matrix_free(transposed);
    gsl_matrix_free(product);
    gsl_matrix_free(U);
//    gsl_matrix_free(homographyIn/*Matrix);
//    gsl_matrix_free(inverseMatrix);*/

    gsl_vector_free(sigmaVector);
    gsl_vector_free(workspace);

//    gsl_permutation_free(permutation);


    return homography;
}

double RansacAlgorithm::countPartOfCoordinate(const vector<double> &homography, const int startIndex, const PointMatch &match) {
    return homography.at(startIndex) * match.firstX + homography.at(startIndex + 1) * match.firstY + homography.at(startIndex + 2);
}
