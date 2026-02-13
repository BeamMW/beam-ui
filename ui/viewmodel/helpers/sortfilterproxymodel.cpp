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

SortFilterProxyModel::SortFilterProxyModel(QObject *parent) 
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

QObject *SortFilterProxyModel::source() const
{
    return sourceModel();
}

void SortFilterProxyModel::setSource(QObject *source)
{
    setSourceModel(qobject_cast<QAbstractItemModel *>(source));
    
    // In Qt6, when the source model changes, we need to re-apply the filter/sort roles
    // because roleKey() depends on the source model's roleNames()
    if (m_complete && sourceModel()) {
        if (!m_sortRole.isEmpty())
            QSortFilterProxyModel::setSortRole(roleKey(m_sortRole));
        if (!m_filterRole.isEmpty())
            QSortFilterProxyModel::setFilterRole(roleKey(m_filterRole));
        endFilterChange();
    }
}

QByteArray SortFilterProxyModel::sortRole() const
{
    return m_sortRole;
}

void SortFilterProxyModel::setSortRole(const QByteArray &role)
{
    if (m_sortRole != role) {
        m_sortRole = role;
        if (m_complete)
            QSortFilterProxyModel::setSortRole(roleKey(role));
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

void SortFilterProxyModel::setFilterRole(const QByteArray &role)
{
    if (m_filterRole != role) {
        m_filterRole = role;
        if (m_complete)
            QSortFilterProxyModel::setFilterRole(roleKey(role));
    }
}

QString SortFilterProxyModel::filterString() const
{
    return filterRegularExpression().pattern();
}

void SortFilterProxyModel::setFilterString(const QString &filter)
{
    QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption;
    if (filterCaseSensitivity() == Qt::CaseInsensitive)
        options |= QRegularExpression::CaseInsensitiveOption;

    QString pattern = filter;
    if (filterSyntax() == Wildcard) {
        pattern = QRegularExpression::wildcardToRegularExpression(filter);
        // Qt6's wildcardToRegularExpression adds anchors (\A and \z) which don't work
        // with QString::contains(). We need to remove them for compatibility.
        // The pattern looks like: \A(?:...)\z or similar
        if (pattern.startsWith("\\A")) {
            pattern = pattern.mid(2);  // Remove \A
        }
        if (pattern.endsWith("\\z")) {
            pattern.chop(2);  // Remove \z
        }
        if (pattern.endsWith("\\Z")) {
            pattern.chop(2);  // Remove \Z (alternate end anchor)
        }
    }
    else if (filterSyntax() == FixedString)
        pattern = QRegularExpression::escape(filter);

    setFilterRegularExpression(QRegularExpression(pattern, options));
}

SortFilterProxyModel::FilterSyntax SortFilterProxyModel::filterSyntax() const
{
    return m_filterSyntax;
}

void SortFilterProxyModel::setFilterSyntax(SortFilterProxyModel::FilterSyntax syntax)
{
    m_filterSyntax = syntax;
    setFilterString(filterString());
}

QVariantMap SortFilterProxyModel::get(int idx) const
{
    QVariantMap map;
    if (idx >= 0 && idx < count()) {
        QHash<int, QByteArray> roles = roleNames();
        QHashIterator<int, QByteArray> it(roles);
        while (it.hasNext()) {
            it.next();
            auto roleName = QString::fromUtf8(it.value());
            map[roleName] = data(index(idx, 0), it.key());
        }
    }
    return map;
}

QVariant SortFilterProxyModel::getRoleValue(int idx, QByteArray roleName) const
{
    QHash<int, QByteArray> roles = roleNames();
    QHashIterator<int, QByteArray> it(roles);
    while (it.hasNext()) {
        it.next();
        if (roleName == it.value())
        {
            return data(index(idx, 0), it.key());
        }
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
        QSortFilterProxyModel::setSortRole(roleKey(m_sortRole));
    if (!m_filterRole.isEmpty())
        QSortFilterProxyModel::setFilterRole(roleKey(m_filterRole));
}

int SortFilterProxyModel::roleKey(const QByteArray &role) const
{
    QHash<int, QByteArray> roles = roleNames();
    QHashIterator<int, QByteArray> it(roles);
    while (it.hasNext()) {
        it.next();
        if (it.value() == role)
            return it.key();
    }
    return -1;
}

QHash<int, QByteArray> SortFilterProxyModel::roleNames() const
{
    if (QAbstractItemModel *source = sourceModel())
        return source->roleNames();
    return QHash<int, QByteArray>();
}

bool SortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QRegularExpression rx = filterRegularExpression();
    if (!rx.isValid() || rx.pattern().isEmpty())
        return true;
    QAbstractItemModel *model = sourceModel();
    if (filterRole().isEmpty()) {
        QHash<int, QByteArray> roles = roleNames();
        QHashIterator<int, QByteArray> it(roles);
        while (it.hasNext()) {
            it.next();
            QModelIndex sourceIndex = model->index(sourceRow, 0, sourceParent);
            QString key = model->data(sourceIndex, it.key()).toString();
            if (key.contains(rx))
                return true;
        }
        return false;
    }
    QModelIndex sourceIndex = model->index(sourceRow, 0, sourceParent);
    if (!sourceIndex.isValid())
        return true;
    QString key = model->data(sourceIndex, roleKey(filterRole())).toString();
    return key.contains(rx);
}
