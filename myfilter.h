#ifndef MYFILTER_H
#define MYFILTER_H

#include <QVector>
#include <QtGui>
#include <QtWidgets>

#include "common_defs.h"

class MyFilter : public QSortFilterProxyModel
{
  public:
    MyFilter() : QSortFilterProxyModel(nullptr)
    {
    }

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
    {
        auto model = sourceModel();
        auto str = model->data(model->index(source_row, 0, source_parent)).toString();
        bool result = filterRegExp().pattern().size() == 0 || g_search.contains(str);
        //        msg("%s: (%d) %d\n", str.toUtf8().toStdString().c_str(), filterRegExp().pattern().size(), result);
        return result;
    }
    bool lessThan(const QModelIndex &left,
                  const QModelIndex &right) const override;

    void setFilter(QString &keyword)
    {
        setFilterFixedString(keyword);
    }
};

#endif // MYFILTER_H
