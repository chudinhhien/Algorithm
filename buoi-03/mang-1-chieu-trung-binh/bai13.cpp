#include<bits/stdc++.h>
using namespace std;
using ll = long long;

int main() {
  ll N;
  cin >> N;
  ll res = 0;
  int a[] = {1, 2, 5, 10, 20, 50, 100, 200, 500, 1000};
  int i = 9;
  while( N != 0 ){
    res += N / a[i];
    N = N % a[i];
    --i;
  }
  cout << res;
}
