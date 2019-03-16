#include "utils.h"
#include <time.h>

#include <widgets/palette_manager.h>

QString loadFile(const char* filename, bool force_update, bool& updated) {
	static QHash<QString, QString> last_loaded;

	auto absolutePath = pluginPath(filename);
	QFile file(absolutePath);
	time_t timestamp = time(NULL);

	updated = false;

	if (last_loaded.contains(absolutePath) && !force_update) {
		auto& content = last_loaded[absolutePath];
		return content;
	}

	if (!file.exists()) {
		return QString();
	}

	if (!file.open(QIODevice::ReadOnly))
		return QString();

	auto content = QString::fromUtf8(file.readAll());
	last_loaded[absolutePath] = content;

	updated = true;

	return content;
}

QHash<QString, QJsonDocument> cached_json;

QJsonObject json(const char* filename, bool force_update) {
	bool updated;
	auto& content_str = loadFile(filename, force_update, updated);

	if (!updated)
		return cached_json[filename].object();

	auto& content = content_str.toUtf8();
	QJsonDocument json(QJsonDocument::fromJson(content));
	cached_json[filename] = json;

	return json.object();
}