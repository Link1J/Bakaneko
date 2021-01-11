// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "dumb.h"
#include "../screen.h"

Dumb::Dumb(Screen* parent)
    : Term(parent)
{}

const char* Dumb::term_type() const
{
	return "dumb";
}

Dumb::~Dumb()  = default;

int Dumb::line_count() const
{
    return line_wrap_count;
}

void Dumb::line_recount()
{
    Screen* screen = static_cast<Screen*>(parent());
    auto pre_line_count = line_count();
    auto col = screen->get_columns();

    line_wrap_count = data.size();

    for (auto& line : data)
    {
        int count = 0;
        for (auto& block : line)
        {
            for (auto& letter : block.text)
            {
                if (count > col)
                {
                    line_wrap_count++;
                    count = 0;
                }
                count++;
            }
        }
    }
    
    if (line_wrap_count != pre_line_count)
        Q_EMIT on_line_count_change();
}

void Dumb::add_text(QString text)
{
    std::string input = text.toUtf8().data();
    Screen* screen = static_cast<Screen*>(parent());
    bool lines_added = false;
    for (auto letter : input)
    {
        if (letter == '\n')
        {
            data.emplace_back().emplace_back();
            current_line++;
            current_block = 0;
            lines_added = true;
        }
        else if (letter == '\r')
        {
            current_block = 0;
            current_char  = 0;
        }
        else if (letter == '\b')
        {
            if (current_char > 0)
            {
                current_char--;
                if ((current_char % screen->get_columns()) == 0)
                    lines_added = true;
            }
        }
        else if (letter < ' ')
        {
        }
        else
        {
            auto& block = data[current_line][current_block];
            if (current_char == block.text.size())
                block.text += letter;
            else
                block.text[current_char] = letter;
            current_char++;
            if ((current_char % screen->get_columns()) == 0)
                lines_added = true;
        }
    }
    if (lines_added)
        line_recount();
    Q_EMIT new_text();
}

const std::vector<std::vector<TextBlock>>& Dumb::get_data() const
{
    return data;
}