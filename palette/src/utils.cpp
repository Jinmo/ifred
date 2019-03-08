#include "ida_headers.h"
#include "utils.h"

bool static_updated;

const QString pluginPath(const char *filename) {
	static QString g_plugin_path;
	if (g_plugin_path.size())
		return g_plugin_path + filename;

	qDebug() << QDir::homePath();

	g_plugin_path = (QString(get_user_idadir()).replace("\\", "/") + QString("/plugins/palette/"));
	QDir plugin_dir(g_plugin_path);
	plugin_dir.mkpath(".");

	return g_plugin_path + filename;
}

QString loadFile(const char *filename, bool force_update, bool &updated) {
    static QHash<QString, QString> last_loaded;
	static time_t last_load_timer;

    auto absolutePath = pluginPath(filename);
    QFile file(absolutePath);
	time_t timestamp = time(NULL);

    updated = false;

    if (last_loaded.contains(absolutePath) && !force_update) {
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

QHash<QString, QJsonDocument> cached_json;

QJsonObject config(const char *filename, bool force_update) {
    bool updated;
    auto &content_str = loadFile(filename, force_update, updated);

    if (!updated)
        return cached_json[filename].object();

    auto &content = content_str.toUtf8();
    QJsonDocument json(QJsonDocument::fromJson(content));
    cached_json[filename] = json;

    return json.object();
}