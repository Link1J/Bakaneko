// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "term.h"
#include "screen.h"

// Yes this is all this file is for
// It is important to have a good file structure
Term::Term(Screen* parent)
    : QObject(parent)
{
    data.emplace_back().emplace_back();
}