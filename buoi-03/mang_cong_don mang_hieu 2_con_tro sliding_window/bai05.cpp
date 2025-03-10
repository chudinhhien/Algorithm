#include<bits/stdc++.h>
using namespace std;
using ll = long long;

int main() {
  int n,q;
  cin >> n >> q;
  ll a[n];
  ll D[n+1];
  for(ll &x : a) cin >> x;
  D[0] = a[0];
  for(int i = 1 ; i < n ; i++) {
    D[i] = a[i] - a[i-1];
  }
  while(q--) {
    int l, r, k;
    cin >> l >> r >> k;
    D[l] += k;
    D[r+1] -= k;
  }
  a[0] = D[0];
  cout << a[0] << ' ';
  for(int i = 1 ; i < n ; i++) {
    a[i] = D[i] + a[i - 1];
    cout << a[i] << ' ';
  }
}
