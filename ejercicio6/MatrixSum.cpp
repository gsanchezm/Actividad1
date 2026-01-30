#include "MatrixSum.h"

long long MatrixSum::sumRecursive(const std::vector<std::vector<long long>>& A) {
    int rows = (int)A.size();
    if (rows == 0) return 0;
    int cols = (int)A[0].size();
    if (cols == 0) return 0;

    int N = rows * cols;
    return sumRange(A, 0, N - 1, cols);
}

long long MatrixSum::sumRange(const std::vector<std::vector<long long>>& A,
                              int left, int right, int cols) {
    if (left == right) {
        int r = left / cols;
        int c = left % cols;
        return A[r][c];
    }
    int mid = left + (right - left) / 2;
    return sumRange(A, left, mid, cols) + sumRange(A, mid + 1, right, cols);
}