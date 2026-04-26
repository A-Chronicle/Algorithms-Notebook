#include<bits/stdc++.h>
using namespace std;

// so basically the problem is we have a 2xN grid
// and we need to fill it using I shaped and L shaped tiles
// I tile is 1x2 (can be placed vertical or horizontal)
// L tile is like a 2x2 square with one corner missing (4 rotations possible)

// i thought about this for a while and realized we can do column by column DP
// at each column, some cells might already be filled by a tile from previous column
// so the "state" is just which rows of current column are pre-filled

// state is a 2-bit number:
//   bit 0 = top row already filled
//   bit 1 = bottom row already filled
// so 4 states: 0 (nothing filled), 1 (top filled), 2 (bot filled), 3 (both filled)

// since N can be up to 10^15, normal DP would be too slow (O(N))
// but since we only have 4 states, we can use matrix exponentiation -> O(log N)

const long long MOD = 1e9 + 7;

// ---------- building transitions ----------

// trans[from_state] = all possible next_states we can reach
vector<int> trans[4];

// this function figures out all ways to fill empty cells of current column
// need_top, need_bot -> which cells still need to be filled
// next_mask -> what cells of next column are already occupied (by tiles we placed)
// i always fill top before bottom to avoid counting same arrangement twice
void findWays(int orig, bool need_top, bool need_bot, int next_mask) {

    // base case: current column is fully filled, record the next_mask
    if (!need_top && !need_bot) {
        trans[orig].push_back(next_mask);
        return;
    }

    if (need_top) {

        // place horizontal I on top row
        // it covers top[j] and top[j+1]
        if (!(next_mask & 1))
            findWays(orig, false, need_bot, next_mask | 1);

        // if bottom also needs filling:
        if (need_bot) {
            // place vertical I -> covers top[j] and bot[j], nothing spills
            findWays(orig, false, false, next_mask);

            // L tile: covers top[j], bot[j], top[j+1]  (like ⌐ shape)
            if (!(next_mask & 1))
                findWays(orig, false, false, next_mask | 1);

            // L tile: covers top[j], bot[j], bot[j+1]  (like ∟ shape)
            if (!(next_mask & 2))
                findWays(orig, false, false, next_mask | 2);
        }

        // L tile: covers top[j], top[j+1], bot[j+1]  (like backwards L)
        if ((next_mask & 3) == 0)
            findWays(orig, false, need_bot, next_mask | 3);

    } else {
        // only bottom needs filling at this point

        // place horizontal I on bottom row
        if (!(next_mask & 2))
            findWays(orig, false, false, next_mask | 2);

        // L tile: covers bot[j], top[j+1], bot[j+1]
        if ((next_mask & 3) == 0)
            findWays(orig, false, false, next_mask | 3);
    }
}

void buildTrans() {
    for (int mask = 0; mask < 4; mask++) {
        bool top_free = !(mask & 1);
        bool bot_free = !(mask & 2);
        findWays(mask, top_free, bot_free, 0);
    }
}

// ---------- matrix exponentiation ----------

// 4x4 matrix since we have 4 states
typedef vector<vector<long long>> Mat;

Mat multiply(Mat &A, Mat &B) {
    int n = A.size();
    Mat C(n, vector<long long>(n, 0));
    for (int i = 0; i < n; i++)
        for (int k = 0; k < n; k++) {
            if (A[i][k] == 0) continue;
            for (int j = 0; j < n; j++)
                C[i][j] = (C[i][j] + A[i][k] * B[k][j]) % MOD;
        }
    return C;
}

Mat matpow(Mat M, long long p) {
    int n = M.size();
    // start with identity matrix
    Mat result(n, vector<long long>(n, 0));
    for (int i = 0; i < n; i++) result[i][i] = 1;

    while (p > 0) {
        if (p % 2 == 1) result = multiply(result, M);
        M = multiply(M, M);
        p /= 2;
    }
    return result;
}

// build the 4x4 transition matrix from our transitions
Mat buildMatrix() {
    Mat T(4, vector<long long>(4, 0));
    for (int from = 0; from < 4; from++)
        for (int to : trans[from])
            T[to][from]++;
    return T;
}

// ---------- main solve ----------

long long solve(long long N) {
    if (N == 0) return 1;

    Mat T = buildMatrix();
    Mat TN = matpow(T, N);

    // we start at state 0 (nothing pre-filled)
    // and we want to end at state 0 (nothing spilling out of last column)
    return TN[0][0];
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    buildTrans();

    long long N;
    cin >> N;

    cout << solve(N) << endl;

    return 0;
}

// quick check in my head:
// N=1: only vertical I fits -> 1 way ✓
// N=2: two vertical I's, two horizontal I's, ... -> 2 ways ✓  
// N=3: -> 5 ways ✓ (matches the given example)
