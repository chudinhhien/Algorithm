#include<bits/stdc++.h>
using namespace std;

int main() {
  int n;
  cin >> n;
  int a[n];
  set<int> s;
  for(int &x : a){
    cin >> x;
    if(s.find(x)==s.end()){
      cout << x << ' ';
      s.insert(x);
    }
  }
  return 0;
}
