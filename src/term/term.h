// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>
#include <QColor>

#include <vector>

class Screen;

struct TextBlock {
    std::string text;
    QColor foreground;
    QColor background;
};

class Term : public QObject
{
    Q_OBJECT

public:
    Term(Screen* parent = nullptr);
    virtual ~Term() = default;

    virtual int line_count() const = 0;
    virtual const std::vector<std::vector<TextBlock>>& get_data() const = 0;
    virtual const char* term_type() const = 0;

public Q_SLOTS:
    virtual void add_text(QString text) = 0;
    virtual void line_recount() = 0;

Q_SIGNALS:
    void on_line_count_change();
    void new_text();

protected:
    std::vector<std::vector<TextBlock>> data;
    int current_line = 0, current_block = 0, current_char = 0;
    int line_wrap_count = 0;
};