#include<bits/stdc++.h>
using namespace std;

bool check(int a[], int n) {
  for(int i = 0 ; i < n ; i++) {
    if(a[i] <= 2 * max(i, n - 1 - i)) return false;
  }
  return true;
}

int main() {
  int t; cin >> t;
  while(t--) {
    int n;
    cin >> n;
    int a[n];
    for(int &x : a) cin >> x;
    if(check(a, n)) {
      cout << "YES" << endl;
    }else {
      cout << "NO" << endl;
    }
  }
}
