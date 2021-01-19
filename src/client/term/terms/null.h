// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include "../term.h"

class Null : public Term
{
    Q_OBJECT

public:
    Null(Screen* parent = nullptr);
    ~Null() override;

    const char* term_type  () const override;
    const char* term_report() const override;

public Q_SLOTS:
    void add_text(QString text) override;

Q_SIGNALS:

private:
};