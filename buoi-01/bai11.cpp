#include<bits/stdc++.h>
using namespace std;

using ll = long long;

// 93 là số fibo cuối cùng còn lưu trong long long
ll F[100];

void fibonacci() {
  F[0] = 0;
  F[1] = 1;
  for(int i = 2 ; i < 93 ; i++) {
    F[i] = F[i-1] + F[i-2];
  }
}

bool checkFibo(ll x) {
  for(int i = 0 ; i < 93 ; i++) {
    if(F[i] == x) return true;
  }
  return false;
}

int main() {
  fibonacci();
  int n; cin >> n;
  ll a[n];
  bool check = false;
  for(ll &x : a) {
    cin >> x;
    if(checkFibo(x)){
      cout << x << ' ';
      check = true;
    } 
  }
  if(!check) cout << "NONE";
  return 0;
}
