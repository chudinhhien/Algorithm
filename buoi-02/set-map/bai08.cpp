#include<bits/stdc++.h>
using namespace std;

int main() {
  int n;
  cin >> n;
  int a[n];
  set<int> s;
  for(int &x : a){
    cin >> x;
    s.insert(x);
  }
  int q; cin >> q;
  while(q--) {
    int mode,p;
    cin >> mode;
    switch (mode)
    {
    case 1:
      cin >> p;
      s.insert(p);
      /* code */
      break;
    case 2:
      cin >> p;
      if(s.find(p)!=s.end()) {
        s.erase(p);
      }
      break;
    case 3:
      if (!s.empty()) {
        cout << *s.begin() << endl;
      } else {
        cout << "Set is empty" << endl;
      }
      break;
    case 4:
      if (!s.empty()) {
        cout << *s.rbegin() << endl;
      } else {
        cout << "Set is empty" << endl;
      }
      break;
    default:
      break;
    }
  }
  return 0;
}
