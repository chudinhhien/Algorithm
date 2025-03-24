#include<bits/stdc++.h>
using namespace std;

int main() {
  int n;
  cin >> n;
  int a[n];
  for(int &x : a) cin >> x;
  sort(a, a+n);
  cout << max(abs(a[0] + a[1]), abs(a[n-1] + a[n-2]));
  return 0;
}
