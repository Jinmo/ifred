#include "ida_headers.h"
#include "utils.h"

bool static_updated;

const QString pluginPath(const char *filename)
{
    return (QDir::homePath() + "/take/" + filename);
}

QString loadFile(const char *filename, bool &updated)
{
    static QHash<QString, std::pair<long, QString>> last_loaded;

    auto absolutePath = pluginPath(filename);
    QFile file(absolutePath);

    updated = false;

    if (!file.exists())
    {
        return QString();
    }

    long timestamp = QFileInfo(file).lastModified().toTime_t();

    if (last_loaded.contains(absolutePath))
    {
        auto &csspair = last_loaded[absolutePath];
        if (csspair.first >= timestamp)
        {
            return csspair.second;
        }
    }
    FILE *fp = qfopen(absolutePath.toStdString().c_str(), "r");

    if (fp == NULL)
    {
        return QString();
    }

    qfseek(fp, 0, SEEK_END);
    auto size = qftell(fp);
    char *buf = new char[size + 1];
    qfseek(fp, 0, SEEK_SET);
    qfread(fp, buf, size);
    qfclose(fp);

    buf[size] = '\0';

    auto csspair = std::pair<long, QString>(timestamp, buf);
    last_loaded[absolutePath] = csspair;

    updated = true;

    delete[] buf;

    return csspair.second;
}

QJsonObject cached_json;

QJsonObject config()
{
    bool updated;
    auto &content_str = loadFile("styles.json", updated);

    if (!updated)
        return cached_json;

    auto &content = content_str.toUtf8();
    QJsonDocument json(QJsonDocument::fromJson(content));
    cached_json = json.object();

    return cached_json;
}