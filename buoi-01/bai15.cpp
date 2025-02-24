#include<bits/stdc++.h>
using namespace std;

using ll = long long;

int main() {
  int n;
  cin >> n;
  int a[n];
  for(int &x : a) cin >> x;
  int max1 = 0, max2 = 0;
  for(int i = 1 ; i < n ; i++) {
    if(a[i] >= max1) {
      max2 = max1;
      max1 = a[i];
    } else if(a[i] > max2) {
      max2 = a[i];
    }
  }
  cout << max1 << ' ' << max2;
  return 0;
}
