#include <QCoreApplication>
#include <QDebug>
#include <QHash>
#include <QList>
#include <QPair>
#include <QTextStream>

#include "../../src/desensitizer.h"

static int failures = 0;

static void check(const QString &name, const QString &actual, const QString &expected)
{
    const bool ok = actual == expected;
    if (!ok)
    {
        ++failures;
    }
    QTextStream out(stdout);
    out << (ok ? "[PASS] " : "[FAIL] ") << name << Qt::endl;
    if (!ok)
    {
        out << "  expected: " << expected << Qt::endl;
        out << "  actual  : " << actual << Qt::endl;
    }
}

static QVector<Mapping> makeMappings(const QList<QPair<QString, QString>> &pairs)
{
    QVector<Mapping> mappings;
    for (const auto &pair : pairs)
    {
        Mapping mapping;
        mapping.enabled = true;
        mapping.from = pair.first;
        mapping.to = pair.second;
        mappings.append(mapping);
    }
    return mappings;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    {
        const QVector<Mapping> m = makeMappings({{"abc", "XYZ"}});
        check(QStringLiteral("simple"), Desensitizer::apply(QStringLiteral("abc xx abc"), m, true, false), QStringLiteral("XYZ xx XYZ"));
        check(QStringLiteral("simple-reverse"), Desensitizer::apply(QStringLiteral("XYZ xx XYZ"), m, true, true), QStringLiteral("abc xx abc"));
    }
    {
        const QVector<Mapping> m = makeMappings({{"ab", "1"}, {"abc", "2"}});
        check(QStringLiteral("longest-match"), Desensitizer::apply(QStringLiteral("abc ab"), m, true, false), QStringLiteral("2 1"));
    }
    {
        const QVector<Mapping> m = makeMappings({{"a", "b"}, {"b", "c"}});
        check(QStringLiteral("no-cascade"), Desensitizer::apply(QStringLiteral("a b"), m, true, false), QStringLiteral("b c"));
        check(QStringLiteral("no-cascade-reverse"), Desensitizer::apply(QStringLiteral("b c"), m, true, true), QStringLiteral("a b"));
    }
    {
        const QVector<Mapping> m = makeMappings({{"abc", "X"}});
        check(QStringLiteral("case-sensitive"), Desensitizer::apply(QStringLiteral("AbC abc"), m, true, false), QStringLiteral("AbC X"));
        check(QStringLiteral("case-insensitive"), Desensitizer::apply(QStringLiteral("AbC abc"), m, false, false), QStringLiteral("X X"));
        check(QStringLiteral("case-insensitive-reverse"), Desensitizer::apply(QStringLiteral("x X"), m, false, true), QStringLiteral("abc abc"));
    }
    {
        const QVector<Mapping> m = makeMappings({{"张三", "张*三"}, {"13800138000", "138****8000"}});
        check(QStringLiteral("chinese"), Desensitizer::apply(QStringLiteral("张三的电话是13800138000。"), m, true, false),
              QStringLiteral("张*三的电话是138****8000。"));
        check(QStringLiteral("chinese-reverse"), Desensitizer::apply(QStringLiteral("张*三的电话是138****8000。"), m, true, true),
              QStringLiteral("张三的电话是13800138000。"));
    }
    {
        const QVector<Mapping> m = makeMappings({{"ab", "1"}, {"abc", "2"}});
        QHash<int, int> hits;
        int total = 0;
        Desensitizer::apply(QStringLiteral("abc ab abc"), m, true, false, &hits, &total);
        check(QStringLiteral("hit-total"), QString::number(total), QStringLiteral("3"));
        check(QStringLiteral("hit-ab"), QString::number(hits.value(0)), QStringLiteral("1"));
        check(QStringLiteral("hit-abc"), QString::number(hits.value(1)), QStringLiteral("2"));
    }
    {
        Mapping disabled;
        disabled.enabled = false;
        disabled.from = QStringLiteral("abc");
        disabled.to = QStringLiteral("X");
        QVector<Mapping> m = makeMappings({{"aa", "Y"}});
        m.append(disabled);
        check(QStringLiteral("disabled-skipped"), Desensitizer::apply(QStringLiteral("abc aa"), m, true, false), QStringLiteral("abc Y"));
    }
    {
        const QVector<Mapping> m = makeMappings({{"abc", "X"}, {"def", "X"}});
        const ValidationResult result = Desensitizer::validate(m, true);
        check(QStringLiteral("duplicate-to-error"), result.errors.isEmpty() ? QStringLiteral("empty") : QStringLiteral("error"), QStringLiteral("error"));
    }
    {
        const QVector<Mapping> m = makeMappings({{"abc", "X"}, {"X", "Y"}});
        const ValidationResult result = Desensitizer::validate(m, true);
        check(QStringLiteral("conflict-warning"), result.warnings.isEmpty() ? QStringLiteral("empty") : QStringLiteral("warn"), QStringLiteral("warn"));
    }

    QTextStream out(stdout);
    out << (failures == 0 ? "ALL TESTS PASSED" : QStringLiteral("%1 TESTS FAILED").arg(failures)) << Qt::endl;
    return failures == 0 ? 0 : 1;
}
