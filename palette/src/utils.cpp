#include "ida_headers.h"
#include "utils.h"

bool static_updated;

const QString pluginPath(const char *filename) {
	static QString g_plugin_path;
	if (g_plugin_path.size())
		return g_plugin_path + filename;

	g_plugin_path = (QDir::homePath() + "/take/theme/");
	return g_plugin_path + filename;
}

QString loadFile(const char *filename, bool &updated) {
    static QHash<QString, QString> last_loaded;
	static time_t last_load_timer;

    auto absolutePath = pluginPath(filename);
    QFile file(absolutePath);
	int timestamp = time(NULL);

    updated = false;

    if (last_loaded.contains(absolutePath) && last_load_timer >= timestamp - 2) {
        auto &content = last_loaded[absolutePath];
        return content;
    }

	if (!file.exists()) {
		return QString();
	}

	FILE *fp = qfopen(absolutePath.toStdString().c_str(), "rb");

    if (fp == nullptr) {
        return QString();
    }

    qfseek(fp, 0, SEEK_END);
    auto size = qftell(fp);
    char *buf = new char[size + 1];
    qfseek(fp, 0, SEEK_SET);
    qfread(fp, buf, size);
    qfclose(fp);

    buf[size] = '\0';

    auto content = QString(buf);
    last_loaded[absolutePath] = content;
	last_load_timer = timestamp;

    updated = true;

    qDebug() << buf;

    delete[] buf;

    return content;
}

QJsonDocument cached_json;

QJsonObject config() {
    bool updated;
    auto &content_str = loadFile("styles.json", updated);

    if (!updated)
        return cached_json.object();

    auto &content = content_str.toUtf8();
    QJsonDocument json(QJsonDocument::fromJson(content));
    cached_json = json;

    return cached_json.object();
}