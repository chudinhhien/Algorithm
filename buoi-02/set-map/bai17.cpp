#include<bits/stdc++.h>
using namespace std;

int main() {
  map<string,pair<int,int>> mp;
  string line;
  while(getline(cin, line)) {
    if(line.empty()) break;
    stringstream ss(line);
    string s;
    int x,y;
    ss >> s >> x >> y;
    mp[s].first += x*y;
    mp[s].second += x;
  }

  for(auto it = mp.rbegin(); it != mp.rend() ; ++it) {
    cout << it->first << " : " << fixed << setprecision(2) << (float) it->second.first / it->second.second << endl;
  }
}
