#include<bits/stdc++.h>
using namespace std;

int main() {
  int n;
  int le = 0, chan = 0;
  while(cin >> n) {
    if(n % 2 == 0) ++chan;
    else ++le;
  }
  int cnt = le + chan;
  if((cnt%2==0 && chan > le) || (cnt%2==1 && le > chan)) {
    cout << "YES";
  }else {
    cout << "NO";
  }
  return 0;
}
