// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#include "linux.h"
#include "../screen.h"

#include <QApplication>

#include <vector>
#include <tuple>
#include <locale>
#include <codecvt>

std::vector<std::tuple<QColor, QColor>> color_palate {
    {{  0,  0,  0},{128,128,128}},
    {{128,  0,  0},{255,  0,  0}},
    {{  0,128,  0},{  0,255,  0}},
    {{128,128,  0},{255,255,  0}},
    {{  0,  0,128},{  0,  0,255}},
    {{128,  0,128},{255,  0,255}},
    {{  0,128,128},{  0,255,255}},
    {{192,192,192},{255,255,255}},
};

std::u32string to_utf32(const std::string& s)
{
    return std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(s);
}

Linux::Linux(Screen* parent)
    : Term(parent)
{
    reset(3);
}

const char* Linux::term_type() const
{
    return "Linux";
}

const char* Linux::term_report() const
{
    return "linux";
}

Linux::~Linux()  = default;

void Linux::add_text(QString text)
{
    std::string input = text.toUtf8().data();
    Screen* screen = static_cast<Screen*>(parent());
    bool lines_added = false;

    for (auto letter : input)
    {
        if (command_buffer[0] == ESC)
        {
            command_buffer += letter;

            if (std::isalpha(letter))
            {
                parse_buffer();
                command_buffer.clear();
            }
        }
        else if ((letter & 0x80) != 0)
        {
            command_buffer += letter;
        }
        else if (letter == ESC)
        {
            command_buffer += letter;
        }
        else if (letter == '\n')
        {
            current_line++;
            if (current_line == data.size())
            {
                data.emplace_back();
                lines_added = true;
            }
        }
        else if (letter == '\r')
        {
            current_char = 0;
        }
        else if (letter == '\b')
        {
            auto& line = data[current_line];
            if (current_char > 0)
            {
                if (current_char == line.size())
                {
                    line.pop_back();
                }
                else if (current_char < line.size())
                {
                    line[current_char].letter = ' ';
                }
                current_char--;
                if ((current_char % screen->get_columns()) == 0)
                    lines_added = true;
            }
        }
        else if (letter == '\a')
        {
            QApplication::beep();
        }
        else
        {
            command_buffer += letter;

            if ((letter & 0x80) == 0)
            {
                for (auto& _letter : to_utf32(command_buffer))
                    add_letter(_letter);
                command_buffer.clear();
            }
        }
    }
    if (lines_added)
        line_recount();
    Q_EMIT new_text();
}

#include <iostream>

void Linux::parse_buffer()
{
    Screen* screen = static_cast<Screen*>(parent());
    command_buffer = command_buffer.substr(1);
    std::cerr << command_buffer << "\n";
    if (command_buffer[0] == '[')
    {
        if (command_buffer == "[?2004h")
        {
        }
        else if (command_buffer == "[?2004l")
        {
        }
        else if (command_buffer.back() == 'H')
        {
            size_t offset = command_buffer.find(';');
            current_line = std::atoi(command_buffer.substr(1).c_str()         ) - 1;
            if (current_line == -1               ) current_line = 0;
            current_char = std::atoi(command_buffer.substr(1).c_str() + offset) - 1;
            if (current_char == -1 || offset == 0) current_char = 0;
        }
        else if (command_buffer.back() == 'J')
        {
            data.clear();
            data.emplace_back();
            line_recount();
        }
        else if (command_buffer.back() == 'm')
        {
            command_buffer = command_buffer.substr(1);
            for (size_t i = 0, o = 0; i < command_buffer.size(); i += o + 1)
            {
                int value = 0;
                
                if (command_buffer.size() > 1)
                    value = std::stoi(command_buffer.substr(i), &o);

                int color = value % 10;
                int group = value - color;

                if (swaped)
                {
                         if (group == 30) group = 40;
                    else if (group == 40) group = 30;
                }

                if (group == 0)
                {
                    if (color == 1)
                    {
                        light = true;
                        group = 30;
                        color = foreground_index;
                    }
                    else if (color == 2)
                    {
                        light = false;
                        group = 30;
                        color = foreground_index;
                    }
                    else if (color == 7)
                    {
                        std::swap(foreground, background);
                        swaped = true;
                    }
                    else if (color == 0)
                    {
                        reset(3);
                    }
                }
                if (group == 20)
                {
                    if (color == 7)
                    {
                        std::swap(foreground, background);
                        swaped = false;
                    }
                }
                if (group == 30)
                {
                    if (color == 9)
                    {
                        reset(1);
                    }
                    else if (color == 8)
                    {
                        break;
                    }
                    else
                    {
                        if (!light)
                            foreground = std::get<0>(color_palate[color]);
                        else
                            foreground = std::get<1>(color_palate[color]);
                        foreground_index = color;
                    }
                }
                if (group == 40)
                {
                    if (color == 9)
                    {
                        reset(2);
                    }
                    else if (color == 8)
                    {
                        break;
                    }
                    else
                    {
                        background = std::get<0>(color_palate[color]);
                    }
                }
                if (group == 90)
                {
                    foreground = std::get<1>(color_palate[color]);
                }
                if (group == 100)
                {
                    background = std::get<1>(color_palate[color]);
                }
            }
        }
        else if (command_buffer.back() == 'K')
        {
            int value = std::atoi(command_buffer.substr(1).c_str());
            int start, end, backup_char = current_char;

            if (value == 0)
            {
                start = current_char;
                end = screen->get_columns();
            }
            else if (value == 1)
            {
                start = 0;
                end = current_char;
            }
            else if (value == 2)
            {
                start = 0;
                end = screen->get_columns();
            }

            for (int a = start; a < end; a++)
            {
                current_char = a;
                add_letter(' ');
            }
            current_char = backup_char;
        }
        else if (command_buffer.back() == 'D')
        {
            int value = 1;
            if (command_buffer.size() > 2)
                value = std::stoi(command_buffer.substr(1));

            current_char -= value;
            if (current_char < 0)
                current_char = 0;
        }
        else if (command_buffer.back() == 'C')
        {
            int value = 1;
            if (command_buffer.size() > 2)
                value = std::stoi(command_buffer.substr(1));

            current_char += value;
            if (current_char > screen->get_columns())
                current_char = screen->get_columns();
        }
        else if (command_buffer.back() == 'A')
        {
            int cap = data.size() > screen->get_rows() ? data.size() - screen->get_rows() : 0;

            int value = 1;
            if (command_buffer.size() > 2)
                value = std::stoi(command_buffer.substr(1));

            current_line -= value;
            if (current_line < cap)
                current_line = cap;
        }
        else if (command_buffer.back() == 'B')
        {
            int cap = data.size() > screen->get_rows() ? data.size() : screen->get_rows();

            int value = 1;
            if (command_buffer.size() > 2)
                value = std::stoi(command_buffer.substr(1));

            current_line += value;
            if (current_line > cap)
                current_line = cap;
        }
        else
        {
            std::cerr << command_buffer << "\n";
        }
    }
    else
    {
        std::cerr << command_buffer << "\n";
    }
}

void Linux::add_letter(char32_t letter)
{
    if (current_line > data.size())
        return;

    auto& line = data[current_line];
    if (current_char == line.size())
    {
        line.push_back({
            letter,
            foreground,
            background,
            false,
            false,
            false
        });
    }
    else if (current_char < line.size())
    {
        line[current_char] = {
            letter,
            foreground,
            background,
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
                foreground,
                background,
                false,
                false,
                false
            });
        }
        line.push_back({
            letter,
            foreground,
            background,
            false,
            false,
            false
        });
    }
    current_char++;
}

void Linux::reset(int type)
{
    if (type == 1 || type == 3)
    {
        foreground = std::get<1>(color_palate[7]);
        foreground_index = 0;
        light = true;
    }
    if (type == 2 || type == 3)
    {
        background = std::get<0>(color_palate[0]);
    }
    if (type == 3)
    {
        swaped = false;
    }
}