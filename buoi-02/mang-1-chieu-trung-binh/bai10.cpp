#include<bits/stdc++.h>
using namespace std;

int main() {
  int n,m;cin >> n >> m;
  int a[n], b[m];
  map<int, int> ma, mb;
  set<int> hop;
  for(int &x : a) {
    cin >> x;
    ma[x]++;
    hop.insert(x);
  }
  for(int &x : b) {
    cin >> x;
    mb[x]++;
    hop.insert(x);
  }
  for(auto it : ma) {
    if(mb[it.first]) {
      cout << it.first << ' ';
    }
  }
  cout << endl;
  for(auto it:hop) {
    cout << it << ' ';
  }
}
