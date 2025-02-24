#include<bits/stdc++.h>
using namespace std;

using ll = long long;

int main() {
  int n;
  cin >> n;
  int a[n], d[1001] = {0};
  for(int &x : a) {
    cin >> x;
    d[x]++;
    if(d[x] == 1) cout << x << ' ';
  }
}
