#include<bits/stdc++.h>
using namespace std;

using ll = long long;

int main() {
  int n,q;
  cin >> n;
  int a[n];
  for(int &x : a) cin >> x;
  ll pre[n];
  pre[0] = a[0];
  for(int i = 1 ; i < n ; i++) {
    pre[i] = pre[i-1] + a[i];
  }
  cin >> q;
  while(q--) {
    int l, r;
    cin >> l >> r;
    --l, --r;
    if(l == 0) cout << pre[r] << endl;
    else cout << pre[r] - pre[l-1] << endl;
  }
}
