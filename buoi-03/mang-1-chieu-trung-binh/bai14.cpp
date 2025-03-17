#include<bits/stdc++.h>
using namespace std;

bool check(int n) {
  while(n >= 10) {
    if(n % 10 < n / 10 % 10) return false;
    n = n / 10;
  }
  return true;
}

bool cmp(pair<int, int> p1, pair<int, int> p2) {
  if(p1.second != p2.second) {
    return p1.second > p2.second;
  }
  return p1.first < p2.first;
}

int main() {
  int n;
  map<int, int> mp;
  vector<pair<int, int>> v;
  while(cin >> n) {
    if(check(n)) {
      mp[n]++;
    }
  }
  for(auto it : mp) {
    v.push_back(it);
  }
  sort(v.begin(), v.end(), cmp);
  for(auto it : v) {
    cout << it.first << " " << it.second << endl;
  }
}
