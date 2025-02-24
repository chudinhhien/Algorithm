#include<bits/stdc++.h>
using namespace std;

using ll = long long;

int main() {
  int n;
  cin >> n;
  int A[n];
  int sum_odd = 0, sum_even = 0;
  int odd = 0 , even = 0;
  for(int &x : A) {
    cin >> x;
    if(x % 2 == 0){
      even++;
      sum_even += x;
    }else {
      odd++;
      sum_odd += x;
    }
  }
  cout << even << endl << odd << endl << sum_even << endl << sum_odd;
  return 0;
}
