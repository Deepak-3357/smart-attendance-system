#pragma once

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace CsvUtils
{
    inline std::string trim(const std::string& value)
    {
        const auto begin = std::find_if_not(value.begin(), value.end(), [](unsigned char ch) {
            return std::isspace(ch) != 0;
        });
        const auto end = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) {
            return std::isspace(ch) != 0;
        }).base();

        if (begin >= end)
        {
            return "";
        }

        return std::string(begin, end);
    }

    inline std::string escape(const std::string& value)
    {
        bool needsQuotes = value.find_first_of(",\"\n\r") != std::string::npos;
        std::string escaped;
        escaped.reserve(value.size());

        for (char ch : value)
        {
            if (ch == '"')
            {
                escaped += "\"\"";
            }
            else
            {
                escaped += ch;
            }
        }

        return needsQuotes ? "\"" + escaped + "\"" : escaped;
    }

    inline std::vector<std::string> parseLine(const std::string& line)
    {
        std::vector<std::string> fields;
        std::string field;
        bool inQuotes = false;

        for (size_t i = 0; i < line.size(); ++i)
        {
            char ch = line[i];

            if (ch == '"')
            {
                if (inQuotes && i + 1 < line.size() && line[i + 1] == '"')
                {
                    field += '"';
                    ++i;
                }
                else
                {
                    inQuotes = !inQuotes;
                }
            }
            else if (ch == ',' && !inQuotes)
            {
                fields.push_back(trim(field));
                field.clear();
            }
            else
            {
                field += ch;
            }
        }

        fields.push_back(trim(field));
        return fields;
    }

    inline void ensureHeader(const std::string& path, const std::string& header)
    {
        std::ifstream input(path);
        if (input.good() && input.peek() != std::ifstream::traits_type::eof())
        {
            return;
        }

        std::ofstream output(path, std::ios::app);
        if (!output)
        {
            throw std::runtime_error("Unable to create CSV file: " + path);
        }

        output << header << '\n';
    }
}
