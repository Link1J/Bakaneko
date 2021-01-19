// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "term.h"
#include "screen.h"

#include <unordered_map>
#include <string_view>

#include "term/terms/dumb.h"
#include "term/terms/linux.h"
#include "term/terms/null.h"

Term::Term(Screen* parent)
    : QObject(parent)
{
    data.emplace_back();
}

Term* Term::create_term(int type, Screen* screen)
{
    switch (type)
    {
    case Term::Dumb : return new ::Dumb (screen);
    case Term::Linux: return new ::Linux(screen);
    }
    return new ::Null(screen);
}

const char* Term::term_type(int type)
{
    auto term = create_term(type, nullptr);
    if (!term) return nullptr;
    auto name = term->term_type();
    delete term;
    return name;
}

int Term::term_type(const char* type)
{
    static std::unordered_map<std::string_view, int> types = {
        { term_type(Term::Dumb ), Term::Dumb  },
        { term_type(Term::Linux), Term::Linux },
    };
    return types[type];
}

int Term::line_count() const
{
    return line_wrap_count;
}

void Term::line_recount()
{
    Screen* screen = static_cast<Screen*>(parent());
    auto pre_line_count = line_count();
    auto col = screen->get_columns();

    line_wrap_count = data.size();

    for (auto& line : data)
    {
        int count = 0;
        for (auto& symbol : line)
        {
            if (count > col)
            {
                line_wrap_count++;
                count = 0;
            }
            count++;
        }
    }
    
    if (line_wrap_count != pre_line_count)
        Q_EMIT on_line_count_change();
}

const std::vector<std::vector<Symbol>>& Term::get_data() const
{
    return data;
}