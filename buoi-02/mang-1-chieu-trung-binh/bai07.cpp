#include<bits/stdc++.h>
using namespace std;

int main() {
  int n;
  cin >> n;
  int a[n];
  vector<int> vc,vl;
  for(int &x : a) {
    cin >> x;
    if(x % 2 == 0) {
      vc.push_back(x);
    }else {
      vl.push_back(x);
    }
  }
  sort(vc.begin(), vc.end());
  sort(vl.begin(), vl.end());
  for(auto it = vl.rbegin() ; it != vl.rend(); ++it) {
    cout << *it << ' ';
  }
  for(int x : vc) {
    cout << x << ' ';
  }
}
