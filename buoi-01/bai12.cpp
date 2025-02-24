#include<bits/stdc++.h>
using namespace std;

using ll = long long;

int main() {
  int n;
  cin >> n;
  int a[n];
  int min = INT_MAX, max = 0;
  for(int &x : a) {
    cin >> x;
  }
  int res_min, res_max;
  for(int i = 0 ; i < n ; i++) {
    if(a[i] <= min) {
      min = a[i];
      res_min = i;
    }
    if(a[i] > max) {
      max = a[i];
      res_max = i;
    }
  }
  cout << res_min << ' ' << res_max << endl;
  return 0;
}
