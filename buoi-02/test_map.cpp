#include<bits/stdc++.h>
using namespace std;

int main() {
  map<int, int> mp;
  mp.insert({1, 2});
  mp.insert({3, 4});
  mp.insert({1, 4});
  mp.insert({3, 2});
  mp.insert({4, 1});
  //map[key] = value
  mp[5] = 10;
  cout << mp.size() << endl;
}
