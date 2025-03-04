#include<bits/stdc++.h>
using namespace std;


int main() {
 int n,m;
 cin >> n >> m;
 int a[n], b[m];
 for(int &x : a) cin >> x;
 for(int &x : b) cin >> x;
 int i1 = 0, i2 = 0;
 while(i1 < n && i2 < m) {
  if(a[i1] >= b[i2]) {
    cout << b[i2] << ' ';
    i2++;
  }else {
    cout << a[i1] << ' ';
    i1++;
  }
 }
 while( i1 < n ) {
  cout << a[i1] << ' ';
  i1++;
 }
 while( i2 < m ) {
  cout << b[i2] << ' ';
  i2++;
 }
}
