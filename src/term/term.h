// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>
#include <QColor>

#include <vector>

class Screen;

struct Symbol 
{
    char32_t letter;
    QColor foreground, background;
    bool bold, italic, underline;
};

constexpr char ESC = '\033';

class Term : public QObject
{
    Q_OBJECT

public:
    // When adding a term type make sure it come before _i_max
    // and update _i_max. When removing a term type, make sure to 
    // comment it out, and to not update _i_max. DO NOT REUSE NUMBERS!
    static constexpr int Null   = 0;
    static constexpr int Dumb   = 1;
    static constexpr int Linux  = 2;
    static constexpr int _i_max = 3;

    Term(Screen* parent = nullptr);
    virtual ~Term() = default;

    int line_count() const;
    const std::vector<std::vector<Symbol>>& get_data() const;

    virtual const char* term_type  () const = 0;
    virtual const char* term_report() const = 0;

    static Term* create_term(int type, Screen* screen);
    static const char* term_type(int type);
    static int term_type(const char* type);

public Q_SLOTS:
    virtual void add_text(QString text) = 0;
    virtual void line_recount();

Q_SIGNALS:
    void on_line_count_change();
    void new_text();

protected:
    std::vector<std::vector<Symbol>> data;
    int current_char = 0;
    int current_line = 0;
    int line_wrap_count = 0;
};