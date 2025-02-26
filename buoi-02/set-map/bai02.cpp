#include<bits/stdc++.h>
using namespace std;

int main() {
  int n,q;
  cin >> n;
  int a[n];
  set<int> s;
  for(int &x : a){
    cin >> x;
    s.insert(x);
  }
  cin >> q;
  while(q--) {
    int tmp;
    cin >> tmp;
    if(s.count(tmp) == 1) {
      cout << "YES" << endl;
    }else {
      cout << "NO" << endl;
    }
  }  
}
