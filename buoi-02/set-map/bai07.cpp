#include <bits/stdc++.h>
using namespace std;

int main() {
  string s;
  set<string> se;
  
  while (true) {
    if (cin.peek() == '\n') break; // Kiểm tra nếu Enter được bấm thì dừng lại
    if (!(cin >> s)) break; // Kiểm tra kết thúc nhập
    se.insert(s);
  }

  cout << se.size() << endl;
  if (!se.empty()) {
    cout << *se.begin() << ' ' << *se.rbegin();
  }
}
