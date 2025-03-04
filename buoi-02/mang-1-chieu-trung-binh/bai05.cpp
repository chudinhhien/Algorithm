#include<bits/stdc++.h>
using namespace std;

int main() {
  int n;
  cin >> n;
  int a[n];
  for(int &x : a ) {
    cin >> x;
  }

  for(int i = 0 ; i < n ; i++) {
    if(i == 0) {
      cout << abs(a[1] - a[0]) << ' ' << abs(a[n-1]-a[0]) << endl;
    } else if( i == n -1 ) {
      cout << abs(a[i] - a[i-1]) << ' ' << abs(a[i] - a[0]) << endl;
    }
    else {
      cout << min(abs(a[i]-a[i-1]),abs(a[i]-a[i+1])) << ' ' << max(abs(a[n-1]-a[i]),abs(a[i]-a[0])) << endl;
    }
  }

  return 0;
}
