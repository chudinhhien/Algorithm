#include<bits/stdc++.h>
using namespace std;

int main() {
  int n,k;
  cin >> n;
  int a[n];
  int d[1001] = {0};
  for(int &x : a) {
    cin >> x;
    d[x]++;
  }
  cin >> k;
  int cnt = 0;
  for(int x = 0 ; x <= k/2 ; x++) {
    int y = k - x;
    if(x != y) cnt+= d[x] * d[y];
    else cnt += d[x] * (d[x] - 1) / 2;
  }
  cout << cnt;
}
