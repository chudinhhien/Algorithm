#include<bits/stdc++.h>
using namespace std;

int main() {
  int n,d;
  cin >> n >> d;
  int a[n];
  for(int &x : a) cin >> x;
  int res = 0;
  for(int i = 1 ; i < n ; i++) {
    if(a[i] <= a[i-1]) {
      int tmp = floor((double) (a[i-1] - a[i]) / d) + 1;
      res += tmp;
      a[i] += tmp * d;
    }
  }
  cout << res << endl;
  for(int x : a) cout << x << ' ';
}
