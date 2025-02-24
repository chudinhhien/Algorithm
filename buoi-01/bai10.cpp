#include<bits/stdc++.h>
using namespace std;

bool isPrimeNumber(int n) {
  for(int i = 2 ; i <= sqrt(n) ; i++) {
    if(n%i==0) return false;
  }
  return n > 1;
}

int main() {
  int n;
  cin >> n;
  int a[n];
  int sum = 0;
  for(int &x : a){
    cin >> x;
    sum+=x;
  }
  int sum_left = a[0];
  int sum_right = sum - a[0] - a[1];
  for(int i = 1 ; i < n - 1 ; i++) {
    if(isPrimeNumber(sum_left) && isPrimeNumber(sum_right)) {
      cout << i << ' ';
    }
    sum_left += a[i];
    sum_right -= a[i];
  }
  return 0;
}
