#ifndef BASIC_SERVICE_H
#define BASIC_SERVICE_H

#include <palette/filter.h>

class BasicService : public SearchService {
  std::vector<int> indexes_, recent_indexes_;
  const QVector<Action> actions_;  // immutable
  QHash<QString, int> recent_actions_;
  QSettings storage_;
  bool canceled_;

  void search(const QString& keyword);

 public:
  BasicService(QObject* parent, const QString& palette_name,
               const QVector<Action>& actions);
  void cancel() override { canceled_ = true; }
  bool runInSeparateThread() override;
};

#endif