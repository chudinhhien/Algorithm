#include<bits/stdc++.h>
using namespace std;

int main() {
  int n;
  cin >> n;
  int a[n];
  for(int &x : a) cin >> x;
  sort(a, a+n);
  int cnt = 0, res = INT_MAX;
  for(int i = 1 ; i < n ; i++) {
    if(a[i] - a[i-1] < min) {
      cnt = 1;
      res = a[i] - a[i-1];
    }else {
      ++cnt;
    }
  }
  cout << res << ' ' << cnt;
  return 0; 
}
