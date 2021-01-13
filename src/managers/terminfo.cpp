// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "terminfo.h"

TermInfo& TermInfo::Instance()
{
    static TermInfo instance;
    return instance;
}

QVariantList TermInfo::get_terms()
{
    QVariantList data;
    for (int a = 0; a < Term::_i_max; a++)
    {
        QString text = Term::term_type(a);
        data.append(QVariant{text});
    }
    return data;
}

int TermInfo::avalible_terms_count()
{
    static int count = [](){
        int count, offset;
        for (count = offset = 0; count < Term::_i_max; count++)
        {
            if (Term::term_type(count) == Term::term_type(Term::Null))
                offset++;
        }
        return count - offset;
    }();
    return count;
}

int TermInfo::avalible_to_emun(int index)
{
    int count, offset;
    for (count = offset = 0; count - offset <= index; count++)
    {
        if (Term::term_type(count) == Term::term_type(Term::Null))
            offset++;
    }
    return count - 1;
}

int TermInfo::emun_to_avalible(int index)
{
    int count, offset;
    for (count = offset = 0; count < index; count++)
    {
        if (Term::term_type(count) == Term::term_type(Term::Null))
            offset++;
    }
        return count - offset;
}