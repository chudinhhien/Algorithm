#include<bits/stdc++.h>
using namespace std;

int main() {
  int n;
  cin >> n;
  while(n--) {
    string s;
    cin >> s;
    if(s.length() <= 10) {
      cout << s << endl;
    } else {
      cout << s[0] << s.length() - 2 << s[s.length() - 1] << endl;
    }
  }
}
