#include "mappingstore.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QRegularExpression>
#include <QSaveFile>
#include <QStandardPaths>
#include <QStringList>

namespace
{
QString stripQuotes(const QString &value)
{
    QString text = value.trimmed();
    if (text.size() >= 2 && text.startsWith(QLatin1Char('"')) && text.endsWith(QLatin1Char('"')))
    {
        text = text.mid(1, text.size() - 2);
        text.replace(QStringLiteral("\"\""), QStringLiteral("\""));
    }
    return text;
}

QString csvEscape(const QString &value)
{
    QString text = value;
    text.replace(QLatin1Char('"'), QStringLiteral("\"\""));
    return QStringLiteral("\"%1\"").arg(text);
}

bool splitLine(const QString &line, QString *from, QString *to)
{
    const QString separators[] = {QStringLiteral("->"), QStringLiteral("=>"), QStringLiteral("\t"),
                                  QStringLiteral(","), QStringLiteral("=")};
    for (const QString &separator : separators)
    {
        const int index = line.indexOf(separator);
        if (index > 0)
        {
            *from = stripQuotes(line.left(index));
            *to = stripQuotes(line.mid(index + separator.size()));
            return !from->isEmpty();
        }
    }

    const int spaceIndex = line.indexOf(QRegularExpression(QStringLiteral("\\s+")));
    if (spaceIndex > 0)
    {
        *from = line.left(spaceIndex).trimmed();
        *to = line.mid(spaceIndex).trimmed();
        return !from->isEmpty();
    }
    return false;
}
}

QString MappingStore::configFilePath()
{
    QString base = qEnvironmentVariable("APPDATA");
    if (base.isEmpty())
    {
        base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
    if (base.isEmpty())
    {
        base = QDir::homePath();
    }
    return QDir(base).filePath(QStringLiteral("ndd-desensitive/mappings.json"));
}

QVector<Mapping> MappingStore::load(QString *error)
{
    QVector<Mapping> mappings;
    const QString path = configFilePath();
    QFile file(path);
    if (!file.exists())
    {
        return mappings;
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        if (error != nullptr)
        {
            *error = QStringLiteral("无法读取配置：%1").arg(path);
        }
        return mappings;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();
    if (parseError.error != QJsonParseError::NoError || !document.isArray())
    {
        if (error != nullptr)
        {
            *error = QStringLiteral("配置文件格式错误：%1").arg(parseError.errorString());
        }
        return mappings;
    }

    const QJsonArray array = document.array();
    mappings.reserve(array.size());
    for (const QJsonValue &value : array)
    {
        if (!value.isObject())
        {
            continue;
        }
        const QJsonObject object = value.toObject();
        Mapping mapping;
        mapping.enabled = object.value(QStringLiteral("enabled")).toBool(true);
        mapping.from = object.value(QStringLiteral("from")).toString();
        mapping.to = object.value(QStringLiteral("to")).toString();
        mappings.append(mapping);
    }
    return mappings;
}

bool MappingStore::save(const QVector<Mapping> &mappings, QString *error)
{
    const QString path = configFilePath();
    QDir().mkpath(QFileInfo(path).absolutePath());

    QJsonArray array;
    for (const Mapping &mapping : mappings)
    {
        QJsonObject object;
        object.insert(QStringLiteral("enabled"), mapping.enabled);
        object.insert(QStringLiteral("from"), mapping.from);
        object.insert(QStringLiteral("to"), mapping.to);
        array.append(object);
    }

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        if (error != nullptr)
        {
            *error = QStringLiteral("无法写入配置：%1").arg(path);
        }
        return false;
    }
    file.write(QJsonDocument(array).toJson(QJsonDocument::Indented));
    if (!file.commit())
    {
        if (error != nullptr)
        {
            *error = QStringLiteral("保存配置失败：%1").arg(path);
        }
        return false;
    }
    return true;
}

bool MappingStore::exportTo(const QString &path, const QVector<Mapping> &mappings, QString *error)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        if (error != nullptr)
        {
            *error = QStringLiteral("无法写入文件：%1").arg(path);
        }
        return false;
    }

    const QString suffix = QFileInfo(path).suffix().toLower();
    if (suffix == QLatin1String("json"))
    {
        QJsonArray array;
        for (const Mapping &mapping : mappings)
        {
            QJsonObject object;
            object.insert(QStringLiteral("enabled"), mapping.enabled);
            object.insert(QStringLiteral("from"), mapping.from);
            object.insert(QStringLiteral("to"), mapping.to);
            array.append(object);
        }
        file.write(QJsonDocument(array).toJson(QJsonDocument::Indented));
    }
    else if (suffix == QLatin1String("csv"))
    {
        file.write("enabled,from,to\r\n");
        for (const Mapping &mapping : mappings)
        {
            const QString line = QStringLiteral("%1,%2,%3\r\n")
                                     .arg(mapping.enabled ? QStringLiteral("true") : QStringLiteral("false"),
                                          csvEscape(mapping.from), csvEscape(mapping.to));
            file.write(line.toUtf8());
        }
    }
    else
    {
        for (const Mapping &mapping : mappings)
        {
            const QString line = mapping.enabled ? QStringLiteral("%1->%2\r\n").arg(mapping.from, mapping.to)
                                                 : QStringLiteral("# %1->%2\r\n").arg(mapping.from, mapping.to);
            file.write(line.toUtf8());
        }
    }

    file.close();
    return true;
}

QVector<Mapping> MappingStore::importFrom(const QString &path, QString *error)
{
    QVector<Mapping> mappings;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        if (error != nullptr)
        {
            *error = QStringLiteral("无法读取文件：%1").arg(path);
        }
        return mappings;
    }

    const QByteArray data = file.readAll();
    file.close();

    const QByteArray trimmed = data.trimmed();
    if (trimmed.startsWith('['))
    {
        QJsonParseError parseError;
        const QJsonDocument document = QJsonDocument::fromJson(trimmed, &parseError);
        if (parseError.error != QJsonParseError::NoError || !document.isArray())
        {
            if (error != nullptr)
            {
                *error = QStringLiteral("JSON 格式错误：%1").arg(parseError.errorString());
            }
            return mappings;
        }
        const QJsonArray array = document.array();
        for (const QJsonValue &value : array)
        {
            if (!value.isObject())
            {
                continue;
            }
            const QJsonObject object = value.toObject();
            Mapping mapping;
            mapping.enabled = object.value(QStringLiteral("enabled")).toBool(true);
            mapping.from = object.value(QStringLiteral("from")).toString();
            mapping.to = object.value(QStringLiteral("to")).toString();
            if (!mapping.from.isEmpty())
            {
                mappings.append(mapping);
            }
        }
        return mappings;
    }

    int skipped = 0;
    mappings = parseBatchText(QString::fromUtf8(data), &skipped);
    if (mappings.isEmpty() && error != nullptr)
    {
        *error = QStringLiteral("未从文件中解析出任何映射。");
    }
    return mappings;
}

QVector<Mapping> MappingStore::parseBatchText(const QString &text, int *skipped)
{
    QVector<Mapping> mappings;
    int skippedCount = 0;

    const QStringList lines = text.split(QRegularExpression(QStringLiteral("[\\r\\n]+")), Qt::SkipEmptyParts);
    for (const QString &rawLine : lines)
    {
        const QString line = rawLine.trimmed();
        if (line.isEmpty() || line.startsWith(QLatin1Char('#')) || line.startsWith(QStringLiteral("//")))
        {
            continue;
        }
        const QString lower = line.toLower();
        if (lower == QLatin1String("from,to") || lower == QLatin1String("enabled,from,to")
            || lower == QLatin1String("from\tto") || lower == QLatin1String("enabled\tfrom\tto"))
        {
            continue;
        }

        bool enabled = true;
        QString working = line;
        if (lower.startsWith(QStringLiteral("false,")) || lower.startsWith(QStringLiteral("0,")))
        {
            enabled = false;
            working = working.mid(working.indexOf(QLatin1Char(',')) + 1);
        }

        QString from;
        QString to;
        if (splitLine(working, &from, &to))
        {
            Mapping mapping;
            mapping.enabled = enabled;
            mapping.from = from;
            mapping.to = to;
            mappings.append(mapping);
        }
        else
        {
            ++skippedCount;
        }
    }

    if (skipped != nullptr)
    {
        *skipped = skippedCount;
    }
    return mappings;
}
