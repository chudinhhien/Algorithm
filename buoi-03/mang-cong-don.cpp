#include<bits/stdc++.h>
using namespace std;
using ll = long long;

int main() {
  int n, q;
  cin >> n >> q;
  int a[n];
  ll prefix_sum[n];
  for(int &x : a) cin >> x;
  prefix_sum[0] = a[0];
  for(int i = 1 ; i < n ; i++) {
    prefix_sum[i] = prefix_sum[i - 1] + a[i];
  }
  while(q--) {
    int l, r;
    cin >> l >> r;
    if(l ==0 ) cout << prefix_sum[r] << endl;
    else cout << prefix_sum[r] - prefix_sum[l-1] << endl;
  }
}
