#include<bits/stdc++.h>
using namespace std;

bool isPrimeNumber(int n) {
  for(int i = 2 ; i <= sqrt(n) ; i++) {
    if(n % i ==0) return false;
  }
  return n > 1;
}

int main() {
  int n;
  cin >> n;
  int A[n];
  float res = 0;
  int count = 0;
  for(int &x : A) {
    cin >> x;
    if(isPrimeNumber(x)) {
      ++count;
      res += x;
    }
  }
  cout << setprecision(3) << fixed << (double) res / count;
}
