#include<bits/stdc++.h>
using namespace std;

using ll = long long;

int main() {
  int n, q;
  cin >> n >> q;
  int a[n];
  for(int &x : a) cin >> x;
  int d[n + 2] = {0};
  while(q--) {
    int l, r;
    cin >> l >> r;
    --l; --r;
    d[l] += 1;
    d[r + 1] -= 1;
  }
  int f[n + 2];
  f[0] = d[0];
  for(int i = 1 ; i < n ; i++) {
    f[i] = f[i - 1] + d[i];
  }
  sort(f, f + n, greater<int>());
  sort(a, a + n, greater<int>());
  ll res = 0;
  for(int i = 0 ; i < n ; i++) {
    res += f[i] * a[i];
  }
  cout << res;
}
