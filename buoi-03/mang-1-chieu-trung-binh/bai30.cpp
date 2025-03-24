#include<bits/stdc++.h>
using namespace std;

using ll = long long;

int main() {
  int n, k;
  cin >> n >> k;
  int a[n];
  for(int &x : a) cin >> x;
  k = min(k, n-k);
  sort(a, a + n);
  ll num1 = 0, num2 = 0;
  for(int i = 0 ; i < k ; i++){
    num1 += 1ll * a[i];
  }
  for(int i = k ; i < n ; i++){
    num2 += 1ll * a[i];
  }
  cout << num2 - num1;
  return 0;
}
