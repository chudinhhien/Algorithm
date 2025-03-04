#include<bits/stdc++.h>
using namespace std;

int main() {
  int n; cin >> n;
  int a[n];
  for(int &x : a) cin >> x;
  int tien_25 = 0, tien_50 = 0;
  for(int x : a) {
    if(x == 25) {
      tien_25 += 1;
    } else if(x == 50) {
      if(tien_25 >= 1) {
        tien_50 += 1;
        tien_25 -= 1;
      }else {
        cout << "NO";
        return 0;
      }
    } else {
      if(tien_50 >= 1 && tien_25 >=1) {
        tien_50 -= 1;
        tien_25 -= 1;
      } else if(tien_25 >= 3) {
        tien_25 -= 3;
      } else {
        cout << "NO";
        return 0;
      }
    }
  }

  cout << "YES";
  return 0;
}
