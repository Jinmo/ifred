#include "myfilter.h"

static QVector<int> costs; // used by distance(s1, s2)
static QHash<QString, QHash<QString, int>> distances;

int distance(const QString &s1, const QString &s2)
{
  const int m(s1.size());
  const int n(s2.size());

  if(distances.contains(s1) && distances[s1].contains(s2))
    return distances[s1][s2];

  if(s2.contains(s1)) {
    return -100000 + s2.indexOf(s1);
  }

  if (costs.size() < n + 1)
  {
    costs.resize(n + 1);
  }

  if (m == 0)
    return n;
  if (n == 0)
    return m;

  for (int k = 0; k <= n; k++)
    costs[k] = k;

  int i = 0;
  for (auto &it1 : s1)
  {
    costs[0] = i + 1;
    int corner = i;

    int j = 0;
    for (auto &it2 : s2)
    {
      int upper = costs[j + 1];
      if (it1 == it2)
      {
        costs[j + 1] = corner;
      }
      else
      {
        int t(upper < corner ? upper : corner);
        costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
      }

      corner = upper;
      j++;
    }
    i++;
  }

  //msg("%-50s %-50s %d", s1.toStdString().c_str(), s2.toStdString().c_str(), costs[n]);
  distances[s1][s2] = costs[n];

  return costs[n];
}

bool MyFilter::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
  const QString &filterText = filterRegExp().pattern();
  const QString &leftData = sourceModel()->data(left).toString(),
                &rightData = sourceModel()->data(right).toString();

  // return leftData < rightData;

  if (filterText.size() == 0)
    return leftData < rightData;

  // auto &leftUsed = g_last_used[leftData],
  //      &rightUsed = g_last_used[rightData];

  // if (leftUsed != rightUsed)
  //   return leftUsed > rightUsed;

  return distance(filterText, leftData) < distance(filterText, rightData);
}
