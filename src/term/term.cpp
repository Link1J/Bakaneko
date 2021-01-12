// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "term.h"
#include "screen.h"

#include <unordered_map>
#include <string_view>

#include "term/terms/dumb.h"
#include "term/terms/linux.h"

Term::Term(Screen* parent)
    : QObject(parent)
{
    data.emplace_back().emplace_back();
}

Term* Term::create_term(Term::Types type, Screen* screen)
{
    switch (type)
    {
    case Term::Dumb : return new ::Dumb (screen);
    case Term::Linux: return new ::Linux(screen);
    }
    return nullptr;
}

const char* Term::term_type(Term::Types type)
{
    auto term = create_term(type, nullptr);
    if (!term) return nullptr;
    auto name = term->term_type();
    delete term;
    return name;
}

Term::Types Term::term_type(const char* type)
{
    static std::unordered_map<std::string_view, Term::Types> types = {
        { term_type(Term::Dumb ), Term::Dumb  },
        { term_type(Term::Linux), Term::Linux },
    };
    return types[type];
}