// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include "../term.h"

class Dumb : public Term
{
    Q_OBJECT

public:
    Dumb(Screen* parent = nullptr);
    ~Dumb() override;

    int line_count() const override;
    const std::vector<std::vector<TextBlock>>& get_data() const override;
    const char* term_type() const override;

public Q_SLOTS:
    void add_text(QString text) override;
    void line_recount() override;

Q_SIGNALS:

private:
};