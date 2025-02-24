#include<bits/stdc++.h>
using namespace std;

int main() {
  int n,x;
  cin >> n;
  int A[n];
  for(int &i : A) cin >> i;
  cin >> x;
  int lon_hon = 0, nho_hon = 0;
  for(int i : A) {
    if(i > x) lon_hon++;
    else if(i < x) nho_hon++;
  }
  cout << nho_hon << endl;
  cout << lon_hon << endl;
}
