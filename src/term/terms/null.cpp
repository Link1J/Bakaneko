// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "null.h"
#include "../screen.h"

Null::Null(Screen* parent)
    : Term(parent)
{}

const char* Null::term_type() const
{
    return "Null";
}

const char* Null::term_report() const
{
    return "dumb";
}

Null::~Null()  = default;

void Null::add_text(QString text)
{
}