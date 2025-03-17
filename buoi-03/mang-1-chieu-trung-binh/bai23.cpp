#include<bits/stdc++.h>
using namespace std;

int main() {
  int n;
  cin >> n;
  int a[n], b[n];
  for(int &x : a) cin >> x;
  for(int &x : b) cin >> x;
  sort(a, a+n);
  sort(b, b+n, greater<int>());
  int i = 0;
  while(i < n) {
    cout << a[i] << ' ' << b[i] << ' ';
    ++i;
  }
}
