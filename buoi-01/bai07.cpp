#include<bits/stdc++.h>
using namespace std;

int main() {
  int n;
  cin >> n;
  int a[n];
  for(int &x : a) cin >> x;
  sort(a, a + n);
  int min_val = INT_MAX;
  for(int i = 1 ; i < n ; i++) {
    min_val = min(min_val,abs(a[i]-a[i-1]));
  }
  cout << min_val;
}
