#ifndef BASIC_SERVICE_H
#define BASIC_SERVICE_H

#include <palette/filter.h>

class BasicService : public SearchService {
  const QVector<Action> actions_;  // immutable

  std::vector<int> indexes_, recent_indexes_;
  QHash<QString, int> recent_actions_;
  QSettings storage_;
  bool canceled_;

  void search(QString keyword);

 public:
  BasicService(QObject* parent, const QString& palette_name,
               const QVector<Action>& actions);
  void cancel() override { canceled_ = true; }
  bool runInSeparateThread() override;
};

#endif