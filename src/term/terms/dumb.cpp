// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "dumb.h"
#include "../screen.h"

Dumb::Dumb(Screen* parent)
    : Term(parent)
{}

const char* Dumb::term_type() const
{
    return "Dumb";
}

const char* Dumb::term_report() const
{
    return "dumb";
}

Dumb::~Dumb()  = default;

void Dumb::add_text(QString text)
{
    std::string input = text.toUtf8().data();
    Screen* screen = static_cast<Screen*>(parent());
    bool lines_added = false;

    for (auto letter : input)
    {
        if (letter == '\n')
        {
            data.emplace_back();
            lines_added = true;
            current_line++;
        }
        else if (letter == '\r')
        {
            current_char = 0;
        }
        else if (letter == '\b')
        {
            if (current_char > 0)
            {
                current_char--;
                if ((current_char % screen->get_columns()) == 0)
                    lines_added = true;
            }
            continue;
        }
        else
        {
            auto& line = data[current_line];
            if (current_char == line.size())
            {
                line.push_back({
                    letter,
                    QColor{ 85,255, 85},
                    QColor{  0,  0,  0},
                    false,
                    false,
                    false
                });
            }
            else if (current_char < line.size())
            {
                line[current_char] = {
                    letter,
                    QColor{ 85,255, 85},
                    QColor{  0,  0,  0},
                    false,
                    false,
                    false
                };
            }
            else if (current_char > line.size())
            {
                while (line.size() != current_char)
                {
                    line.push_back({
                        ' ',
                        QColor{ 85,255, 85},
                        QColor{  0,  0,  0},
                        false,
                        false,
                        false
                    });
                }
                line.push_back({
                    letter,
                    QColor{ 85,255, 85},
                    QColor{  0,  0,  0},
                    false,
                    false,
                    false
                });
            }
            current_char++;
        }
    }
    if (lines_added)
        line_recount();
    Q_EMIT new_text();
}