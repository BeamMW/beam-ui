// Copyright 2018 The Beam Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sortfilterproxymodel.h"

SortFilterProxyModel::SortFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
    , m_complete(false)
{
    connect(this, &QAbstractItemModel::rowsInserted, this, &SortFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &SortFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::modelReset, this, &SortFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::layoutChanged, this, &SortFilterProxyModel::countChanged);
}

int SortFilterProxyModel::count() const
{
    return rowCount();
}

QObject* SortFilterProxyModel::source() const
{
    return sourceModel();
}

void SortFilterProxyModel::setSource(QObject* source)
{
    setSourceModel(qobject_cast<QAbstractItemModel*>(source));
}

QByteArray SortFilterProxyModel::sortRole() const
{
    return m_sortRole;
}

void SortFilterProxyModel::setSortRole(const QByteArray& role)
{
    if (m_sortRole != role)
    {
        m_sortRole = role;
        if (m_complete)
        {
            QSortFilterProxyModel::setSortRole(roleKey(role));
        }
    }
}

void SortFilterProxyModel::setSortOrder(Qt::SortOrder order)
{
    QSortFilterProxyModel::sort(0, order);
}

QByteArray SortFilterProxyModel::filterRole() const
{
    return m_filterRole;
}

void SortFilterProxyModel::setFilterRole(const QByteArray& role)
{
    if (m_filterRole != role)
    {
        m_filterRole = role;
        if (m_complete)
        {
            QSortFilterProxyModel::setFilterRole(roleKey(role));
        }
    }
}

QString SortFilterProxyModel::filterString() const
{
    return m_filterPattern.pattern();
}

void SortFilterProxyModel::setFilterString(const QString& filter)
{
    m_filterPattern = QRegularExpression(filter, filterOptions());
    invalidateFilter();
}

SortFilterProxyModel::FilterSyntax SortFilterProxyModel::filterSyntax() const
{
    return m_filterSyntax;
}

void SortFilterProxyModel::setFilterSyntax(SortFilterProxyModel::FilterSyntax syntax)
{
    if (m_filterSyntax != syntax)
    {
        m_filterSyntax = syntax;
        QRegularExpression::PatternOptions options = filterOptions();
        m_filterPattern.setPatternOptions(options);
        invalidateFilter();
    }
}

QVariantMap SortFilterProxyModel::get(int idx) const
{
    QVariantMap map;
    if (idx >= 0 && idx < count())
    {
        const auto& roles = roleNames();
        for (auto it = roles.begin(); it != roles.end(); ++it)
        {
            map[QString::fromUtf8(it.value())] = data(index(idx, 0), it.key());
        }
    }
    return map;
}

QVariant SortFilterProxyModel::getRoleValue(int idx, const QByteArray& roleName) const
{
    const auto& roles = roleNames();
    auto it = std::find_if(roles.begin(), roles.end(), [&roleName](const QByteArray& name) {
        return name == roleName;
        });

    if (it != roles.end())
    {
        return data(index(idx, 0), it.key());
    }
    return QVariant();
}

void SortFilterProxyModel::classBegin()
{
}

void SortFilterProxyModel::componentComplete()
{
    m_complete = true;
    if (!m_sortRole.isEmpty())
    {
        QSortFilterProxyModel::setSortRole(roleKey(m_sortRole));
    }
    if (!m_filterRole.isEmpty())
    {
        QSortFilterProxyModel::setFilterRole(roleKey(m_filterRole));
    }
}

int SortFilterProxyModel::roleKey(const QByteArray& role) const
{
    const auto roles = roleNames();
    for (auto it = roles.begin(); it != roles.end(); ++it)
    {
        if (it.value() == role)
        {
            return it.key();
        }
    }
    return -1;
}

QHash<int, QByteArray> SortFilterProxyModel::roleNames() const
{
    if (const auto& source = sourceModel())
    {
        return source->roleNames();
    }
    return {};
}

bool SortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    const auto model = sourceModel();
    if (!model)
    {
        return false;
    }

    const QModelIndex sourceIndex = model->index(sourceRow, 0, sourceParent);
    if (!sourceIndex.isValid())
    {
        return false;
    }

    const auto& regex = m_filterPattern;
    if (!regex.isValid() || regex.pattern().isEmpty())
    {
        return true;
    }

    if (m_filterRole.isEmpty())
    {
        const auto roles = roleNames();
        for (auto it = roles.begin(); it != roles.end(); ++it)
        {
            const QString value = model->data(sourceIndex, it.key()).toString();
            if (regex.match(value).hasMatch())
            {
                return true;
            }
        }
        return false;
    }

    const QString value = model->data(sourceIndex, roleKey(m_filterRole)).toString();
    return regex.match(value).hasMatch();
}

QRegularExpression::PatternOptions SortFilterProxyModel::filterOptions() const
{
    switch (m_filterSyntax)
    {
    case FilterSyntax::FixedString:
        return QRegularExpression::NoPatternOption;
    case FilterSyntax::Wildcard:
        return QRegularExpression::NoPatternOption;// : WildcardOption;
    case FilterSyntax::RegExp:
    default:
        return QRegularExpression::NoPatternOption;
    }
}
