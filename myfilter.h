#ifndef MYFILTER_H
#define MYFILTER_H

#include <QVector>
#include <QtGui>
#include <QtWidgets>

#include "common_defs.h"

class MyFilter : public QSortFilterProxyModel
{
    QRegularExpression expression_;
    QString keyword_;
  public:
    MyFilter() : QSortFilterProxyModel(nullptr)
    {
    }

    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
    {
        if(keyword_.isEmpty())
            return true;

        auto model = sourceModel();
        auto str = model->data(model->index(source_row, 0, source_parent)).toString();
        bool result = str.contains(expression_);
        qDebug() << expression_.pattern();
        return result;
    }

    bool lessThan(const QModelIndex &left,
                  const QModelIndex &right) const override;

    void setMyFilter(QString &keyword)
    {
        keyword_ = keyword;

        qDebug() << keyword;

        QStringList regexp_before_join = {
            ".*"
        };

        for(auto &x: keyword)
            regexp_before_join.push_back(x);

        regexp_before_join.push_back(".*");
        expression_ = QRegularExpression(regexp_before_join.join(".*"),
        QRegularExpression::CaseInsensitiveOption);

        setFilterRegExp(keyword);
    }
};

#endif // MYFILTER_H
