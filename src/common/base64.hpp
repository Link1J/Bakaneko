// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <string>
#include <array>
#include <bitset>
#include <stdexcept>

#include <ljh/get_index.hpp>

namespace Base64
{
    constexpr std::array table {
        'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
        'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
        'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
        'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/',
    };

    inline std::string encode(std::string message)
    {
        std::string output;
        for (int a = 0; a < message.size(); a += 3)
        {
            std::bitset<24> value = 0;
            int s;
            for (s = 0; s < 3; s++)
            {
                value <<= 8;
                if (s + a < message.size())
                {
                    value |= uint32_t(message[s + a]);
                }
                else
                {
                    value <<= 8 * (2 - s);
                    break;
                }
            }
            for (int b = 0; b < 4; b++)
            {
                if (b < (s == 1 ? 2 : s == 2 ? 3 : 4))
                    output += table[((value >> (6 * (3 - b))) & std::bitset<24>(0b111111)).to_ulong()];
                else
                    output += '=';
            }
        }
        return output;
    }

    inline std::string decode(std::string message)
    {
        std::string output;
        std::bitset<24> value = 0;
        int shifts = 0;
        for (auto& letter : message)
        {
            auto index = ljh::get_index(table.begin(), table.end(), letter);
            
            if (!(index > 0 && index < std::size(table)) && letter != '=')
                throw std::out_of_range("(Base64::decode) Unknown character");
            else if (letter == '=')
                index = 0;

            value <<= 6;
            value |= index;
            shifts++;

            if (shifts == 4)
            {
                for (int a = 0; a < 3; a++)
                    output += (char)(((value >> (8 * (2 - a))) & std::bitset<24>(0xFF)).to_ulong());
                value = 0;
                shifts = 0;
            }
        }
        return output.substr(0, output.find_last_not_of('\0') + 1);
    }
}