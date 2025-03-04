#include<bits/stdc++.h>
using namespace std;

bool check_mang_tang_chat(int a[], int n) {
  for(int i = 1 ; i < n ; i++) {
    if(a[i] <= a[i-1]) return false;
  }
  return true;
}

int main() {
  int n;
  cin >> n;
  int a[n];
  for(int &x : a) cin >> x;
  if(check_mang_tang_chat(a, n)) {
    cout << "YES";
  } else {
    cout << "NO";
  }
  return 0;
}
