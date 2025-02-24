#include<bits/stdc++.h>
using namespace std;

int main() {
  int n; cin >> n;
  int A[n];
  for(int &x : A) cin >> x;
  int cnt = 0, res = 1001;
  for(int x : A) {
    if(x < res) {
      cnt = 1;
      res = x;
    } else if(res == x) {
      cnt++;
    }
  }
  cout << cnt << endl;
  return 0;
}
