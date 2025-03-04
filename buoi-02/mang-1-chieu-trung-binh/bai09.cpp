#include<bits/stdc++.h>
using namespace std;


int d[10000005] = {0};

int main() {
  int n;
  cin >> n;
  int a[n];
  for(int &x : a) {
    cin >> x;
    d[x]++;
  }
  for(int i = 0 ; i <= 10000000 ; i++) {
    if(d[i]) {
      cout << i << ' ' << d[i] << endl;
    }
  }
  cout << endl;
  for(int x : a) {
    if(d[x]) {
      cout << x << ' ' << d[x] << endl;
      d[x] = 0;
    }
  }
}
