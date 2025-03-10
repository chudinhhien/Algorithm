#include<bits/stdc++.h>
using namespace std;

//comparison function
// - trả về true nếu bạn muốn a đứng trước b sau khi sắp xếp
// - trả về false nếu bạn muốn a đứng sau b sau khi sắp xếp
bool cmp(int a, int b) {
  if(a < b) return true;
  else return false;
}

int main() {
  int n;
  cin >> n;
  int a[n];
  for(int &x : a) cin >> x;
  sort(a, a+n);
  sort(a, a+n, greater<int>());
  sort(a, a+n, cmp);
  stable_sort(a, a+n); // bảo toàn vị trí ban đầu các phần tử
}
