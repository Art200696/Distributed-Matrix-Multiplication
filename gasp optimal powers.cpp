//
//  main.cpp
//  practice
//
//  Created by Mahmud on 03/20/19.
//  Copyright © 2018 Mahmud. All rights reserved.
//

/*
 The following code calculates the optimal powers set
 for GASP codes with parameters K and L. The solution for
 K = L = 20 case is handled seperately since the results has been used
 for the comparisons with other schemes.
 */

#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <numeric>
#include <functional>
#include <utility>

using namespace std;

const long long TIME_MAX = 1LL << 35; // ~1 minute expected;
// change according to the computing resources

const int INDEX_SKIPS = 6;
const int TMAX = 10; // maximum number of iterations for T values

class Coefficients{
    int size;
    vector<int> rowCoefficients;
    vector<int> columnCoefficients;
    
public:
    Coefficients() {
        
    }
    Coefficients(int __size) {
        size = __size;
    }
    Coefficients(vector<int> __row, vector<int> __column, int __size) {
        rowCoefficients = __row;
        columnCoefficients = __column;
        size = __size;
    }
    int getSize() {
        return size;
    }
    vector<int> getRowData() {
        return rowCoefficients;
    }
    vector<int> getColumnData() {
        return columnCoefficients;
    }
    pair<vector<int>, vector<int> > getData() {
        pair<vector<int>, vector<int> > result =
                            pair<vector<int>, vector<int> >(rowCoefficients, columnCoefficients);
        return result;
    }
    ~Coefficients() {
        
    }
};

double getRate(int K, int L, int N) {
    return 1. * K * L / N;
}
double binom(int N, int K) {
    double product = 1;
    for (int i = N - K + 1; i <= N; i ++) product *= i;
    for (int i = 1; i <= K; i ++) product /= i;
    return product;
}

int K, L;

int main() {
    cin >> K >> L;
    
    
    for (int t = 1; t <= 1; t ++) {
        int kmax = K + t + INDEX_SKIPS;
        int lmax = L + t + INDEX_SKIPS;
        vector<int> rowBits(kmax);
        vector<int> columnBits(lmax);
        long long expectedOperations = binom(kmax, K + t) * binom(lmax, L + t) * (kmax * lmax);
        if (expectedOperations > TIME_MAX) {
            cout << "The operations will take long." << endl;
            cout << "Aborting..." << endl;
            break;
        }
        for (int i = kmax - 1; i >= kmax - K - t + 1; i --) rowBits[i] = 1;
        for (int i = lmax - 1; i >= lmax - L - t + 1; i --) columnBits[i] = 1;
        rowBits[0] = 1;
        columnBits[0] = 1;
        Coefficients bestCoeffs(0);
        do {
            vector<int> __columnBits = columnBits;
            do {
                vector<bool> active(kmax + lmax, 0);
                vector<int> lx, cx;
                for (int i = 0; i < kmax; i ++) if (rowBits[i]) lx.push_back(i);
                for (int i = 0; i < lmax; i ++) if (columnBits[i]) cx.push_back(i);
                vector<bool> important(kmax + lmax, 0);
                bool valid = true;
                for (int i = 0; i < K && valid; i ++) {
                    for (int j = 0; j < L && valid; j ++) {
                        if (important[lx[i] + cx[j]]) valid = false;
                        important[lx[i] + cx[j]] = true;
                    }
                }
                for (int i = 0; i < K + t && valid; i ++) {
                    for (int j = 0; j < L + t && valid; j ++) {
                        active[lx[i] + cx[j]] = 1;
                        if (i < K && j < L) continue;
                        if (important[lx[i] + cx[j]]) valid = false;
                    }
                }
                if (!valid) continue;
                cout << "valid!" << endl;
                cout << int(lx.size()) << " vs " << int(cx.size()) << endl;
                for (int i: lx) cout << i; cout << " ";
                for (int i: cx) cout << i; cout << endl;
                cout << K << " " << K + t << " " << L << " " << L + t << endl;
                int activeCounter = accumulate(active.begin(), active.end(), 0);
                
                if (activeCounter > bestCoeffs.getSize()) {
                    // cout << activeCounter << endl;
                    bestCoeffs = Coefficients(rowBits, columnBits, activeCounter);
                }
            } while (next_permutation(++columnBits.begin(), columnBits.end()));
            columnBits = __columnBits;
        } while (next_permutation(++rowBits.begin(), rowBits.end()));
        
        cout << "for t = " << t << ", the optimal powers are:" << endl;
        cout << "row coefficients:";
        for (int coef: bestCoeffs.getRowData()) {
            cout << " " << coef;
        } cout << endl;
        cout << "column coefficients:";
        for (int coef: bestCoeffs.getColumnData()) {
            cout << " " << coef;
        } cout << endl;
        cout << "aha" << endl;
        cout << bestCoeffs.getSize() << endl;
    }
    
    return 0;
}
