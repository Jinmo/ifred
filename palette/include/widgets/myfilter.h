#ifndef MYFILTER_H
#define MYFILTER_H

#include <QVector>
#include <QtGui>
#include <QtWidgets>
#include <QFutureWatcher>

#include <QtConcurrent/QtConcurrent>

#include <action.h>

class MyFilter : public QAbstractItemModel {
    Q_OBJECT;
    QVector<Action> items_;
    std::vector<int> shown_items_;
    std::vector<int> shown_items_temp_;

    int shown_items_count_;

    QFutureWatcher<bool>* watcher_;
    QFuture<bool> future_;

    QString keyword_;
    bool canceled_;

    long preferred_index_;

public:
    MyFilter(QWidget* parent, const QVector<Action>& items);

    // Public interface
    bool filterAcceptsRow(const QString& keyword, int source_row);

    bool lessThan(const QString& keyword, const QString& left, const QString& right) const;

    void setFilter(const QString& keyword);

    void doneFiltering();

    bool update_filter(const QString& keyword);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    int	columnCount(const QModelIndex & parent) const override { return 1; }

    QModelIndex	parent(const QModelIndex & index) const override { return {}; }

    int rowCount(const QModelIndex & parent = QModelIndex()) const override;
signals:
    void filteringDone(int preferred_index);
};

#endif // MYFILTER_H
