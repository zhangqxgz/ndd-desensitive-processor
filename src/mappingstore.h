#pragma once

#include <QString>
#include <QVector>
#include "desensitizer.h"

namespace MappingStore
{
QString configFilePath();
QVector<Mapping> load(QString *error = nullptr);
bool save(const QVector<Mapping> &mappings, QString *error = nullptr);
bool exportTo(const QString &path, const QVector<Mapping> &mappings, QString *error = nullptr);
QVector<Mapping> importFrom(const QString &path, QString *error = nullptr);
QVector<Mapping> parseBatchText(const QString &text, int *skipped = nullptr);
}
