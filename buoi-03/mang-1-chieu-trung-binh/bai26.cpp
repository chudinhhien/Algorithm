#include<bits/stdc++.h>
using namespace std;

int main() {
  int n; cin >> n;
  int a[n];
  for(int &x : a) cin >> x;
  int res = 0;
  for(int i = 1 ; i < n ; i++) {
    if(a[i-1] >= a[i]) {
      res += a[i-1] - a[i] + 1;
      a[i] = a[i-1] + 1;
    }
  }
  for(int x : a) cout << x << ' ';
  cout << endl;
  cout << res;
}
