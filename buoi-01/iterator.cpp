#include<bits/stdc++.h>
using namespace std;

using ll = long long;

//auto: tự động

int main() {
  vector<int> v = {1,2,3,4,5};
  v.insert(v.begin() + 1 , 1);
  for(auto it = v.begin(); it != v.end(); it++) {
    cout << *it << ' ';
  }
  pair<int,int> p = {100,200};
  cout << p.first << ' '  << p.second << endl;
  return 0;
}
