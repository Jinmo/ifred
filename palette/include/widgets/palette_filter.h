#ifndef PALETTE_FILTER_H
#define PALETTE_FILTER_H

#include <QVector>
#include <QtGui>
#include <QtWidgets>
#include <QFutureWatcher>

#include <QtConcurrent/QtConcurrent>

#include <action.h>

class SearchService;

class PaletteFilter : public QAbstractItemModel {
    Q_OBJECT;

    std::vector<int> shown_items_;
    int shown_items_count_;

    QString keyword_;

    QThread* searcher_;
    SearchService* search_service_;

public:
    PaletteFilter(QWidget* parent, const QString& palette_name, const QVector<Action>& items);

    // Public interface
    void setFilter(const QString& keyword);
    SearchService* search_service() {
        return search_service_;
    }

    // Implementations
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QModelIndex	parent(const QModelIndex& index) const override { return {}; }

    int	columnCount(const QModelIndex& parent) const override { return 1; }
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    void onDoneSearching(std::vector<int>* indexes_, int count, int preferred_index);

signals:
    void filteringDone(int preferred_index);
};

class SearchService : public QObject {
    Q_OBJECT;

    std::vector<int> indexes_, recent_indexes_;
    QVector<Action> actions_;
    QHash<QString, int> recent_actions_;

    QSettings storage_;

    bool canceled_;

    void search(const QString& keyword);

public:
    using QObject::moveToThread;
    SearchService(QObject* parent, const QString& palette_name, const QVector<Action>& actions);

    const QVector<Action>& actions() { return actions_; }

    void cancel() {
        canceled_ = true;
    }

signals:
    // Request
    void startSearching(const QString& keyword);
    void reportAction(const QString& action);
    // Response
    void doneSearching(std::vector<int>* indexes, int count, int preferred_index);
};

#endif // PALETTE_FILTER_H
