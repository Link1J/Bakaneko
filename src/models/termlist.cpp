// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "termlist.h"
#include "../managers/terminfo.h"
#include "../term/term.h"

TermList::TermList(QObject* parent)
    : QAbstractListModel(parent)
{
}

TermList::~TermList() = default;

QVariant TermList::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto value = TermInfo::Instance().avalible_to_emun(index.row());
    if (role == Qt::UserRole + 1) return value;
    return QString(Term::term_type(value));
}

int TermList::rowCount(const QModelIndex& parent) const
{
    return TermInfo::Instance().avalible_terms_count();
}

QHash<int, QByteArray> TermList::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[Qt::UserRole + 1] = "value";
    return roles;
}