#include <bits/stdc++.h>
using namespace std;

int main()
{
  int k;
  cout << "Nhap k: ";
  cin >> k;
  k = k % 26;

  string ban_ro;
  cout << "Nhap ban ro: ";
  cin.ignore();
  getline(cin, ban_ro);

  for (int i = 0; i < ban_ro.length(); i++)
  {
    if (isupper(ban_ro[i]))
    {
      ban_ro[i] = 'A' + (ban_ro[i] - 'A' + k) % 26;
    }
    else if (islower(ban_ro[i]))
    {
      ban_ro[i] = 'a' + (ban_ro[i] - 'a' + k) % 26;
    }
  }

  cout << "Ban ma: " << ban_ro << endl;
  return 0;
}
