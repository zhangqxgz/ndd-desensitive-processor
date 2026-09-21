#include "desensitizer.h"

#include <QRegularExpression>
#include <QSet>
#include <algorithm>

namespace
{
struct Item
{
    QString key;
    QString value;
    int index = -1;
};

QStringList buildPatterns(const QVector<Item> &items)
{
    QStringList patterns;
    patterns.reserve(items.size());
    for (const Item &item : items)
    {
        patterns << QRegularExpression::escape(item.key);
    }
    return patterns;
}
}

QString Desensitizer::apply(const QString &input,
                            const QVector<Mapping> &mappings,
                            bool caseSensitive,
                            bool reverse,
                            QHash<int, int> *hitCounts,
                            int *totalHits)
{
    if (totalHits != nullptr)
    {
        *totalHits = 0;
    }
    if (input.isEmpty() || mappings.isEmpty())
    {
        return input;
    }

    QVector<Item> items;
    items.reserve(mappings.size());
    for (int i = 0; i < mappings.size(); ++i)
    {
        const Mapping &mapping = mappings.at(i);
        if (!mapping.enabled)
        {
            continue;
        }

        Item item;
        item.key = reverse ? mapping.to : mapping.from;
        item.value = reverse ? mapping.from : mapping.to;
        item.index = i;
        if (!item.key.isEmpty())
        {
            items.append(item);
        }
    }

    if (items.isEmpty())
    {
        return input;
    }

    std::stable_sort(items.begin(), items.end(), [](const Item &left, const Item &right) {
        return left.key.size() > right.key.size();
    });

    QHash<QString, int> exactIndex;
    QHash<QString, int> lowerIndex;
    exactIndex.reserve(items.size());
    lowerIndex.reserve(items.size());
    for (int i = 0; i < items.size(); ++i)
    {
        if (!exactIndex.contains(items.at(i).key))
        {
            exactIndex.insert(items.at(i).key, i);
        }
        const QString lowerKey = items.at(i).key.toLower();
        if (!lowerIndex.contains(lowerKey))
        {
            lowerIndex.insert(lowerKey, i);
        }
    }

    QRegularExpression re(buildPatterns(items).join(QLatin1Char('|')));
    if (!caseSensitive)
    {
        re.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    }

    QString output;
    output.reserve(input.size() + 64);

    int last = 0;
    int total = 0;
    QRegularExpressionMatchIterator iterator = re.globalMatch(input);
    while (iterator.hasNext())
    {
        const QRegularExpressionMatch match = iterator.next();
        const int start = match.capturedStart();
        const int end = match.capturedEnd();
        if (start < last)
        {
            continue;
        }

        output += input.mid(last, start - last);

        const QString captured = match.captured(0);
        const QString lookup = caseSensitive ? captured : captured.toLower();
        const int itemIndex = caseSensitive ? exactIndex.value(lookup, -1) : lowerIndex.value(lookup, -1);

        if (itemIndex >= 0)
        {
            output += items.at(itemIndex).value;
            if (hitCounts != nullptr)
            {
                (*hitCounts)[items.at(itemIndex).index] += 1;
            }
            ++total;
        }
        else
        {
            output += captured;
        }

        last = end;
    }
    output += input.mid(last);

    if (totalHits != nullptr)
    {
        *totalHits = total;
    }
    return output;
}

ValidationResult Desensitizer::validate(const QVector<Mapping> &mappings, bool caseSensitive)
{
    ValidationResult result;

    QSet<QString> seenFrom;
    QSet<QString> seenTo;
    QStringList fromKeys;
    fromKeys.reserve(mappings.size());

    for (const Mapping &mapping : mappings)
    {
        if (!mapping.enabled)
        {
            continue;
        }
        if (mapping.from.isEmpty())
        {
            result.errors << QStringLiteral("存在“原文”为空的映射，请填写或删除该行。");
            continue;
        }

        const QString fromKey = caseSensitive ? mapping.from : mapping.from.toLower();
        const QString toKey = caseSensitive ? mapping.to : mapping.to.toLower();

        if (seenFrom.contains(fromKey))
        {
            result.errors << QStringLiteral("原文重复：%1").arg(mapping.from);
        }
        else
        {
            seenFrom.insert(fromKey);
            fromKeys << fromKey;
        }

        if (!mapping.to.isEmpty())
        {
            if (seenTo.contains(toKey))
            {
                result.errors << QStringLiteral("替换文重复：%1（还原时无法区分）。").arg(mapping.to);
            }
            else
            {
                seenTo.insert(toKey);
            }
        }
    }

    for (const Mapping &mapping : mappings)
    {
        if (!mapping.enabled || mapping.from.isEmpty() || mapping.to.isEmpty())
        {
            continue;
        }
        const QString fromKey = caseSensitive ? mapping.from : mapping.from.toLower();
        const QString toKey = caseSensitive ? mapping.to : mapping.to.toLower();

        if (!caseSensitive && fromKey == toKey)
        {
            result.warnings << QStringLiteral("原文与替换文相同（忽略大小写）：%1").arg(mapping.from);
            continue;
        }

        if (seenFrom.contains(toKey) && toKey != fromKey)
        {
            result.warnings << QStringLiteral("替换文“%1”会与其它映射的原文冲突，还原时可能产生误还原。")
                                   .arg(mapping.to);
        }

        for (const QString &otherFrom : fromKeys)
        {
            if (otherFrom == fromKey)
            {
                continue;
            }
            if (otherFrom.contains(toKey))
            {
                result.warnings << QStringLiteral("替换文“%1”是原文“%2”的一部分，脱敏后再次操作可能被二次替换。")
                                       .arg(mapping.to, otherFrom);
                break;
            }
        }
    }

    result.warnings.removeDuplicates();
    return result;
}
