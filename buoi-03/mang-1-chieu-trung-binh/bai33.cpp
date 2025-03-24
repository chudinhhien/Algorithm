#include<bits/stdc++.h>
using namespace std;

int main() {
  int n, k;
  cin >> n >> k;
  int a[n];
  for(int &x : a) cin >> x;
  int tmp = a[0];
  int l = 0, r = 0;
  int res = INT_MAX;
  while(r < n) {
    tmp += a[r];
    r++;

    while(tmp >= k) {
      if(tmp == k) res = min(res, r - l);
      tmp -= a[l];
      l++;
    }
  }
  res = res == INT_MAX ? -1 : res;
  cout << res;
}
