#include<bits/stdc++.h>
using namespace std;
using ll = long long;

int main() {
  int n;
  cin >> n;
  int a[n];
  for(int &x : a) cin >> x;
  string so1, so2;
  sort(a, a+n);
  for(int i = 0 ; i < n ; i++) {
    if(i % 2 == 0) so1 += to_string(a[i]);
    else so2 += to_string(a[i]);
  }
  ll num1 = stoll(so1);
  ll num2 = stoll(so2);
  cout << num1 + num2;
  return 0;
}
