#include "utils.h"
#include <time.h>

#include <widgets/palette_manager.h>

void postToMainThread(const std::function<void()>& fun) {
    QObject signalSource;
    QObject::connect(&signalSource, &QObject::destroyed, qApp, [=](QObject*) {
        fun();
        });
}

QString loadFileFromBundle(const char* filename, QFile &file, bool& updated) {
    // Check if it exists in bundle resource
    QFile resFile(QStringLiteral(":/bundle/") + filename);

    updated = false;

    if (resFile.exists()) {
        if (!resFile.open(QIODevice::ReadOnly)) return QString();
        auto bytes = resFile.readAll();
        auto content = QString::fromUtf8(bytes);

        auto dir = QDir(file.fileName());
        dir.mkpath("..");

        if (file.open(QIODevice::WriteOnly)) {
            file.write(bytes);
            file.close();
        }

        updated = true;

        return content;
    }
    else
        return QString();
}

QString loadFile(const char* filename, bool force_update, bool& updated) {
    static bool resource_initialized;

    if(!resource_initialized) {
        Q_INIT_RESOURCE(theme_bundle);
        resource_initialized = true;

        QDir::addSearchPath("theme", pluginPath("theme/"));
    }

    auto absolutePath = pluginPath(filename);
    QFile file(absolutePath);

    updated = false;

    if (!file.exists()) {
        return loadFileFromBundle(filename, file, updated);
    }

    if (!file.open(QIODevice::ReadOnly))
        return QString();

    auto content = QString::fromUtf8(file.readAll());
    updated = true;

    return content;
}

QHash<QString, QJsonDocument> cached_json;

const QJsonObject json(const char* filename, bool force_update) {
	bool updated;
	const QString& content_str = loadFile(filename, force_update, updated);

    if (!updated)
        return cached_json[filename].object();

	const QByteArray& content = content_str.toUtf8();
	QJsonDocument json(QJsonDocument::fromJson(content));
	cached_json[filename] = json;

    return json.object();
}
