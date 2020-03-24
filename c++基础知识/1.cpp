#include <iostream>

using namespace std;

int main() {
    int n, m;
    cin >> n;
    int *deap = new int[n + 1]; deap[0] = 0;
    for (int i = 1; i < n + 1; i++) {
        cin >> deap[i];
        deap[i] += deap[i - 1];
    }
    cin >> m;
    for (int i = 0; i < m; i++) {
        int q; cin >> q;
        int left = 0, right = n;
        while (left < right) {
            int mid = (left + right) / 2;
            if (deap[mid] == q) left = right = mid;
            else if (q < deap[mid]) right = mid;
            else left = mid + 1;
        }
        cout << left << endl;
    }
    return 0;
}