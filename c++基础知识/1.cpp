#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

bool overlapping(vector<int>& a, vector<int>& b) {
    if (b[0] >= a[2]) return false;
    if (b[2] <= a[0]) return false;
    if (b[1] >= a[3]) return false;
    if (b[3] <= a[1]) return false;
    return true;
}

int main() {
    int n;  cin >> n;
    vector<vector<int> > rects(n, vector<int>(4));
    vector<vector<bool> > sign(n, vector<bool>(4, false));
    for (int i = 0; i < n; i++) cin >> rects[i][0];
    for (int i = 0; i < n; i++) cin >> rects[i][1];
    for (int i = 0; i < n; i++) cin >> rects[i][2];
    for (int i = 0; i < n; i++) cin >> rects[i][3];
    for (int i = 0; i < n; i++) {
        sign[i][i] = true;
        for (int j = i + 1; j < n; j++) {
            sign[i][j] = overlapping(rects[i], rects[j]);
            sign[j][i] = sign[i][j];
        }
    }
    int a[11] = {2, 4, 8, 16, 18, 21, 26, 33, 35, 41, 49};
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 11; j++) {
            if (!sign[a[i]][a[j]]) {
                cout << "i=" << a[i] << "  , j=" << a[j] << endl; 
            }
        }
    }
    vector<vector<int> > res;
    for (int i = 0; i < n; i++) {
        res.push_back(vector<int>(1, i));
    }
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            if (i == k) break;
            int j = 0;
            for (j = 0; j < res[k].size(); j++) {
                if (!sign[i][res[k][j]]) break;
            }
            if (j == res[k].size()) res[k].push_back(i);
        }
    }
    int max = 1;
    for (int i = 0; i < res.size(); i++) {
        if (res[i].size() > max) max = res[i].size();
    }
    cout << max << endl;
    for (int i = 0; i < res.size(); i++) {
        for (int j = 0; j < res[i].size(); j++) {
            cout << res[i][j] << ",";
        }
        cout << endl;
    }
    return 0;
}