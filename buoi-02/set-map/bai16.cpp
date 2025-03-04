#include<bits/stdc++.h>
using namespace std;

int find_char(char c, string s) {
  for(int i = 0 ; i < s.length(); i++) {
    if(s[i] == c) return i;
  }
  return -1;
}

//Trước getline mà trước là cin thì phải cin.ignore()
int main() {
  int n;
  cin >> n;
  cin.ignore();
  map<string, vector<string>> mp;
  for(int i = 0 ; i < n ; i++) {
    string s;
    getline(cin,s);
    int pos = find_char('-', s);
    string team1 = s.substr(0, pos - 1);
    string team2 = s.substr(pos + 2);
    mp[team1].push_back(team2);
    mp[team2].push_back(team1);
  }
  for(auto it = mp.begin(); it != mp.end(); it++) {
    sort(it->second.begin(),it->second.end());
    cout << it->first << " : ";
    for(auto it1 = it->second.begin(); it1 != it->second.end() ; it1++) {
      if(it1 == it->second.end() - 1) {
        cout << *it1 << endl;
      } else {
        cout << *it1 << ", ";
      }
    }
  }
}
