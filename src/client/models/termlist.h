// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <QStringList>

class TermList : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit TermList(QObject* parent = nullptr);
    ~TermList() override;

    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

public Q_SLOT:
};