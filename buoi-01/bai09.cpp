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
  }
  for(int x : a) {
    if(d[x]!=0) {
      cout << x << ' ' << d[x] << endl;
      d[x] = 0; 
    }
  }
}
