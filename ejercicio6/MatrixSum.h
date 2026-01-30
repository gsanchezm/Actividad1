#pragma once
#include <vector>

class MatrixSum {
public:
    // Técnica: Divide y vencerás (recursivo)
    static long long sumRecursive(const std::vector<std::vector<long long>>& A);

private:
    static long long sumRange(const std::vector<std::vector<long long>>& A,
                              int left, int right, int cols);
};