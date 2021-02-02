// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <string>
#include <vector>
#include <string_view>
#include <fstream>
#include <filesystem>
#include <type_traits>
#include <memory>
#include <stdexcept>
#include <optional>

#include <boost/lexical_cast.hpp>

#include <ljh/memory_mapped_file.hpp>
#include <ljh/expected.hpp>
#include <ljh/string_utils.hpp>

class ini
{
    template<typename T>
    struct private_holder
    {
        T data;

        template<typename...Args>
        private_holder(Args... args) 
            : data{args...}
        {}

        T& operator* () { return                data ; }
        T* operator->() { return std::addressof(data); }
        const T& operator* () const { return                data ; }
        const T* operator->() const { return std::addressof(data); }
    };

public:
    class item
    {
        friend class ini;
        friend class section;
        friend private_holder<item>;

        std::string _key;
        std::optional<std::string> _value;
        
        item(std::string key)
            : _key(key)
        {}

        ~item()
        {}

        item(const item&) = default;
        item& operator=(const item&) = default;

    public:
        template<typename T>
        item& operator=(T data)
        {
            set(data);
            return *this;
        }

        template<typename T>
        operator T() const
        {
            return get<T>();
        }

        template<typename T>
        T get()
        {
            return boost::lexical_cast<T>(_value.value());
        }
        
        template<typename T>
        T get(const T& defualt)
        {
            if (!_value) return defualt;
            return boost::lexical_cast<T>(_value.value());
        }

        template<typename T>
        void set(T data)
        {
            if constexpr (std::is_same_v<T, std::nullptr_t>)
                remove();
            else
                _value = boost::lexical_cast<std::string>(data);
        }

        void remove()
        {
            _value = std::nullopt;
        }
    };

    class section
    {
        friend class ini;
        friend private_holder<section>;

        std::string _name;
        std::vector<private_holder<item>> _items;

        section(std::string name)
            : _name(name)
        {}

        ~section() {}

        section(const section&) = default;
        section& operator=(const section&) = default;

    public:
        item& operator[](const std::string& name)
        {
            if (name.empty())
                throw std::out_of_range{""};
            for (auto& item : _items)
                if (item->_key == name)
                    return *item;
            return *_items.emplace_back(name);
        }

        const item& operator[](const std::string& name) const
        {
            for (auto& item : _items)
                if (item->_key == name)
                    return *item;
            throw std::out_of_range{""};
        }

        bool has(const std::string& name) const
        {
            for (auto& item : _items)
                if (item->_key == name)
                    return true;
            return false;
        }
    };

private:
    std::vector<private_holder<section>> sections;


public:
    ini() = default;

    void load_file(std::filesystem::path filename)
    {
        ljh::memory_mapped::file file(std::forward<std::filesystem::path>(filename), ljh::memory_mapped::permissions::r);
        ljh::memory_mapped::view view(file, ljh::memory_mapped::permissions::r, 0, file.size());
        std::string_view file_data{view.as<char>(), file.size()};

        int line_count = 0;
        auto lines = ljh::split(file_data, '\n');
        section* current_section = nullptr;
        for (auto& line : lines)
        {
            auto data = ljh::split(line, ';', 2)[0];
            ljh::rtrim(data);
            if (data.empty()) continue;

            if (data.front() == '[')
            {
                if (auto end = data.find(']'); end != std::string_view::npos)
                {
                    current_section = &(*this)[std::string(data.data() + 1, end - 1)];
                    continue;
                }
            }
            else 
            {
                auto prop = ljh::split(data, '=', 2);
                if (prop.size() == 2)
                {
                    (*current_section)[std::string{prop[0].data(),prop[0].size()}] = prop[1];
                    continue;
                }
            }
            throw std::invalid_argument{""};
        }
    }

    section& operator[](const std::string& name)
    {
        for (auto& section : sections)
            if (section->_name == name)
                return *section;
        return *sections.emplace_back(name);
    }

    const section& operator[](const std::string& name) const
    {
        for (auto& section : sections)
            if (section->_name == name)
                return *section;
        throw std::out_of_range{""};
    }

    bool has(const std::string& name) const
    {
        for (auto& section : sections)
            if (section->_name == name)
                return true;
        return false;
    }
};