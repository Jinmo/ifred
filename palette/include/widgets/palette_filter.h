#ifndef PALETTE_FILTER_H
#define PALETTE_FILTER_H

#include <QVector>
#include <QtGui>
#include <QtWidgets>
#include <QFutureWatcher>

#include <QtConcurrent/QtConcurrent>

#include <action.h>

class PaletteFilter : public QAbstractItemModel {
    Q_OBJECT;

    QVector<Action> items_;
    std::vector<int> shown_items_;
    std::vector<int> shown_items_temp_;

    int shown_items_count_;
    QString keyword_;

public:
    PaletteFilter(QWidget* parent, const QVector<Action>& items);

    // Public interface
    void setFilter(const QString& keyword);

    bool match(const QString& keyword, Action& action);
    bool lessThan(const QString& keyword, Action& left, Action& right) const;

    // Implementations
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QModelIndex	parent(const QModelIndex& index) const override { return {}; }

    int	columnCount(const QModelIndex& parent) const override { return 1; }
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

signals:
    void filteringDone(int preferred_index);
};

#endif // PALETTE_FILTER_H
