// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>
#include <QVariant>

#include "../term/term.h"

class TermInfo : public QObject
{
    Q_OBJECT

private:
    Q_PROPERTY(QVariantList terms READ get_terms NOTIFY changed_terms)

public:
    static TermInfo& Instance();

public Q_SLOTS:
    Q_INVOKABLE QVariantList get_terms();
    Q_INVOKABLE int avalible_terms_count();

    Q_INVOKABLE int avalible_to_emun(int index);
    Q_INVOKABLE int emun_to_avalible(int index);


Q_SIGNALS:
    void changed_terms();

private:
};

using TermInfoPointer = TermInfo*;
Q_DECLARE_METATYPE(TermInfoPointer);
