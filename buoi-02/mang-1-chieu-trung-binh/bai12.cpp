#include<bits/stdc++.h>
using namespace std;

bool isPrimeNumber(int n) {
  for(int i = 2 ; i <= sqrt(n) ; i++) {
    if(n % i == 0) return false;
  }
  return n > 1;
}

int main() {
  vector<int> v;
  int n;
  map<int, int> mp;
  while(cin >> n){
    if(isPrimeNumber(n)) {
      if(!mp[n]) {
        v.push_back(n);
      }
      mp[n]++;
    }
  }
  for(int x : v) {
    cout << x << ' ' << mp[x] << endl;
  }
}
