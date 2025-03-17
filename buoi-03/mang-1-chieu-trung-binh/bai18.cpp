#include<bits/stdc++.h>
using namespace std;
using ll = long long;

int d[1000005] = {0};

int main() {
  int n;
  cin >> n;
  int a[n];
  for(int &x : a){
    cin >> x;
    d[x]++;
  }
  ll cnt = 0;
  for(int i = 1 ; i <= 1e6 ; i++) {
    if(d[i]) {
      cnt += 1ll * d[i] * (d[i] - 1) / 2;
    }
  }
  cout << cnt ;
  return 0;
}
