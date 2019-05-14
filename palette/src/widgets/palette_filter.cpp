#include <widgets/palette_filter.h>

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include "fts_fuzzy_match.h"

typedef QHash<QPair<QString, QString>, int> DistanceHash;
#define SAME_THREAD_THRESHOLD 5000

int distance(const QString& s1_, const QString& s2_) {
	static QThreadStorage<DistanceHash*> distances;

	QString s1 = s1_.toLower();
	QPair<QString, QString> pair(s1, s2_);

	if (!distances.hasLocalData()) {
		distances.setLocalData(new DistanceHash());
	}

	if (distances.localData()->contains(pair))
		return (*distances.localData())[pair];

	QByteArray s1b = s1.toUtf8();
	QByteArray s2b = s2_.toUtf8();

	int score;
	fts::fuzzy_match(s1b.data(), s2b.data(), score);

	distances.localData()->insert(pair, -score);
	return -score;
}


void PaletteFilter::setFilter(const QString & keyword) {
	keyword_ = keyword;
	search_service_->cancel();
	emit search_service_->startSearching(keyword);
}

void PaletteFilter::onDoneSearching(std::vector<int> *indexes_, int count, int preferred_index) {
	emit layoutAboutToBeChanged();

	std::copy(indexes_->begin(), indexes_->begin() + count, shown_items_.begin());
	shown_items_count_ = count;

	emit filteringDone(preferred_index);
	emit layoutChanged();
}

QModelIndex PaletteFilter::index(int row, int column, const QModelIndex & parent) const {
	return createIndex(row, column);
}

QVariant PaletteFilter::data(const QModelIndex & index, int role) const {
	if (role == Qt::DisplayRole)
		return QVariant::fromValue(items_[shown_items_[index.row()]]);
	else if (role == Qt::UserRole)
		return keyword_;
	return QVariant();
}

int PaletteFilter::rowCount(const QModelIndex & parent) const {
	return shown_items_count_;
}

PaletteFilter::PaletteFilter(QWidget * parent, const QVector <Action> & items)
	: QAbstractItemModel(parent), items_(items), shown_items_count_(0),
	shown_items_(items.size()) {
	search_service_ = new SearchService(nullptr, &items_);

	if (items.count() >= SAME_THREAD_THRESHOLD) {
		searcher_ = new QThread(this);
		searcher_->start();
		search_service_->moveToThread(searcher_);
	}
	else {
		searcher_ = nullptr;
		search_service_->setParent(this);
	}
	connect(search_service_, &SearchService::doneSearching, this, &PaletteFilter::onDoneSearching);
	setFilter(QString());
}

bool SearchService::lessThan(const QString & keyword, const Action & left, const Action & right) const {
	return distance(keyword, left.name) < distance(keyword, right.name);
}

bool SearchService::match(const QString & keyword, Action & action) {
	if (keyword.isEmpty())
		return true;

	return fts::fuzzy_match_simple(keyword, action.name);
}

void SearchService::search(const QString & keyword) {
	long count = 0, preferred_index = 0;
	auto&& actions = *actions_;

	canceled_ = false;

	/* Filter the items with fuzzy matching: see PaletteFilter::fuzzy_match_simple,
	   which is substr with non-neighbor characters support
	*/
	for (long i = 0; i < indexes_.size(); i++) {
		if (canceled_)
			return;
		if (match(keyword, actions[i])) {
			indexes_[count++] = i;
		}
	}

	/* Sort by fuzzy matching if keyword is longer than 1 character
	   This sorts indexes_ which is array of indexes, and copies to shown_items_.
	   The size/capacity of each vector is not changed, just member variable shown_items_count_ is changed.

	   If the job is cancelled during sort, the compare function raises exception to abort sorting.

	   TODO: set preferred_index from command history to focus on the recently executed action
	*/
	try {
		if (keyword.size() > 1)
			std::sort(indexes_.begin(), indexes_.begin() + count, [=](int lhs, int rhs) -> bool {
			if (canceled_)
				throw std::exception();
			return lessThan(keyword, actions[lhs], actions[rhs]);
				});
	}
	catch (std::exception) {
		return;
	}

	emit doneSearching(&indexes_, count, preferred_index);
}

SearchService::SearchService(QObject * parent, QVector <Action> * actions) : QObject(parent), actions_(actions), indexes_(actions->size()), canceled_(false) {
	connect(this, &SearchService::startSearching, this, &SearchService::search);
}
