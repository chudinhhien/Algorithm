#include<bits/stdc++.h>
using namespace std;

int gcd(int a, int b) {
  while(b!=0) {
    int r = a % b;
    a = b;
    b = r;
  }
  return a;
}

int main() {
  int n;
  cin >> n;
  int a[n];
  int cnt = 0;
  for(int &x : a) cin >> x;
  for(int  i = 0 ; i < n ; i++) {
    for(int j = i ; j < n ; j++) {
      if(gcd(a[i],a[j]) == 1) ++cnt;
    }
  }
  cout << cnt;
}
