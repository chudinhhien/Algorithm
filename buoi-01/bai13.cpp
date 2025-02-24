#include<bits/stdc++.h>
using namespace std;

using ll = long long;

int mod = 1e9 + 7;

int main() {
  int n;
  cin >> n;
  int a[n];
  ll sum = 0, tich = 1;
  for(int &x : a) cin >> x;
  for(int i = 0 ; i < n ; i++) {
    // sum = ((sum % mod) + (a[i] % mod)) % mod;
    // Tuy nhiên, mỗi bước sum lúc nào cũng % mod => sum < mod
    // => sum = (sum + a[i]) % mod
    sum = ( sum + a[i] ) % mod;
    tich = ( tich * a[i] ) % mod;
  }
  cout << sum << endl;
  cout << tich << endl;
  return 0;
}
