#include<bits/stdc++.h>
using namespace std;

int main() {
  int n,q;
  cin >> n;
  cin.ignore();
  map<string, string> mp;
  for(int i = 0 ; i < n ; i++) {
    string mssv, name;
    cin >> mssv;
    cin.ignore();
    getline(cin, name);
    mp[mssv] = name;
  }
  cin >> q;
  cin.ignore();
  while(q--) {
    string query;
    cin >> query;
    if(mp.count(query)) {
      cout << mp[query] << endl;
    } else {
      cout << "NOT FOUND" << endl;
    }
  }
}
