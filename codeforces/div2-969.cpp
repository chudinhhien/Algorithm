#include<bits/stdc++.h>
using namespace std;

int gcd(int a, int b) {
  while(b != 0) {
    int r = a % b;
    a = b;
    b = r;
  }
  return a;
}

int main() {
  int t;
  cin >> t;
  while(t--) {
    int l, r;
    cin >> l >> r;
    int d[1005] = {0};
    int cnt = 0;
    for(int i = l ; i <= r ; i++) {
      for(int j = i + 1 ; j <= r; j++) {
        for(int k = j + 1 ; k <= r ; k++) {
          if(d[i] == 0 && d[j] == 0 && d[k] == 0 ){
            if(gcd(i,j) == 1 && gcd(j, k) == 1 && gcd(i, k) == 1) {
              cout << i << ' ' << j << ' ' << k << endl;
              ++cnt;
              d[i] = 1; d[j] = 1; d[k] = 1;
            }
          }
          
        }
      }
    }
    cout << cnt << endl;
  }
}
