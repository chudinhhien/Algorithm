#include<bits/stdc++.h>
using namespace std;

int main() {
  int n,m;
  cin >> n >> m;
  int a[n], b[m];
  set<int> sb;
  for(int &x : a){
    cin >> x;
  }
  for(int &x : b){
    cin >> x;
    sb.insert(x);
  }
  for(int x : a) {
    if(sb.count(x) == 1) {
      cout << x << ' ';
      sb.erase(x);
    }
  }
  return 0;
}
