#include<bits/stdc++.h>
using namespace std;

int main() {
  int n;
  cin >> n;
  int A[n];
  for(int &x : A) cin >> x;
  bool check = true;
  for(int i = 0 ; i < n ; i+=2){
    if(A[i] % 2 == 0) {
      check = false;
      cout << A[i] << ' ';
    }
  }
  if(check) cout << "NONE" << endl;
  return 0;
}
