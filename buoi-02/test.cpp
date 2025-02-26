#include<bits/stdc++.h>
using namespace std;

int main() {
  set<int> se;
  multiset<int> ms;
  int a[] = {3, 1, 2, 2, 1, 4};

  for(int x : a) {
    se.insert(x); //O(logN)
  }
  for(int x : se) {
    cout << x << ' ';
  }
  cout << endl;
  for(auto it = se.begin(); it != se.end(); it++) {
    cout << *it << ' ';
  }
  se.erase(4);
  cout << endl;
  for(auto it = se.rbegin(); it != se.rend(); it++) {
    cout << *it << ' ';
  }
  cout << endl;
  cout << se.size();
  return 0;
}
