#include<bits/stdc++.h>
using namespace std;

int main() {
  int n;
  cin >> n;
  int a[n];
  for(int &x : a) cin >> x;
  for(int i = 0 ; i < n ; i++){
    bool check = true;
    for(int j = 0 ; j <= i - 1 ; j++){
      if(a[i] == a[j]) {
        check = false;
        break;
      }
    }
    if(check) cout << a[i] << ' '; 
  }
  return 0;
}
