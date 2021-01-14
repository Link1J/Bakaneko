// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include "../term.h"

class Linux : public Term
{
    Q_OBJECT

public:
    Linux(Screen* parent = nullptr);
    ~Linux() override;

    const char* term_type  () const override;
    const char* term_report() const override;

public Q_SLOTS:
    void add_text(QString text) override;

Q_SIGNALS:

private:
    void parse_buffer();
    void add_letter(char32_t letter);
    void reset(int type);

    std::string command_buffer;
    QColor foreground, background;
    bool light = true;
    bool swaped = false;
    int foreground_index;
};