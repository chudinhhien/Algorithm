#include<bits/stdc++.h>
using namespace std;

int d[3];

int main() {
  int n; cin >> n;
  int a[n];
  for(int &x : a) {
    cin >> x;
    d[x]++;
  }
  for(int i = 1 ; i <= d[0] ; i++) {
    cout << 0 << ' ';
  }
  for(int i = 1 ; i <= d[1] ; i++) {
    cout << 1 << ' ';
  }
  for(int i = 1 ; i <= d[2] ; i++) {
    cout << 2 << ' ';
  }
}
