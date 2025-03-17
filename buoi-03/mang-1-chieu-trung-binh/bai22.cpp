#include<bits/stdc++.h>
using namespace std;


int d[1000005];
int main(){
  int n;
  cin >> n;
  int a[n];
  for(int &x : a) cin >> x;
  for(int x : a) {
    if(d[x] == 1) {
      cout << x << endl;
      return 0;
    }
    d[x] = 1;
  }
  cout << -1;
  return 0;
}
