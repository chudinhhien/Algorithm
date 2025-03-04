#include<bits/stdc++.h>
using namespace std;

int main() {
  int n;
  cin >> n;
  int a[n];
  for(int &x : a) cin >> x;
  int max_val = -INT_MAX;
  for(int i = 0 ; i < n ; i++) {
    if(a[i] > max_val) {
      cout << a[i] << ' ';
      max_val = a[i];
    }
  }
  return 0;
}
