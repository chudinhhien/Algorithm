#include <bits/stdc++.h>
using namespace std;

int main()
{
  int t;
  cin >> t;

  while (t--)
  {
    int n, k;
    cin >> n >> k;

    unordered_map<int, int> freq;
    int a[n];

    for (int i = 0; i < n; i++)
    {
      cin >> a[i];
      freq[a[i]]++;
    }

    int cnt = 0;

    // Duyệt qua các số từ 1 đến k/2 để tìm cặp (i, k-i)
    for (int i = 1; i <= k / 2; i++)
    {
      if (freq[i] > 0 && freq[k - i] > 0)
      {
        if (i == k - i)
        {
          cnt += freq[i] / 2; // Trường hợp i == k-i, cặp sẽ được chọn theo từng cặp
        }
        else
        {
          cnt += min(freq[i], freq[k - i]);
        }
      }
    }

    cout << cnt << endl;
  }

  return 0;
}
