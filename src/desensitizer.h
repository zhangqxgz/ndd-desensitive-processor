#pragma once

#include <QString>
#include <QStringList>
#include <QVector>
#include <QHash>

struct Mapping
{
    bool enabled = true;
    QString from;
    QString to;
};

struct ValidationResult
{
    QStringList errors;
    QStringList warnings;
};

class Desensitizer
{
public:
    static QString apply(const QString &input,
                         const QVector<Mapping> &mappings,
                         bool caseSensitive,
                         bool reverse,
                         QHash<int, int> *hitCounts = nullptr,
                         int *totalHits = nullptr);

    static ValidationResult validate(const QVector<Mapping> &mappings, bool caseSensitive);
};
