#include "parser.h"

#include <vector>
#include <string>

#include <utility>
#include <cassert>
#include <iostream>

namespace editor {

Parser::Parser(int argc, char* argv[])
{
    int pos = 1;
    while (pos < argc)
    {
        std::string str(argv[pos]);
        ArgType prefix = readPrefix(str);

        if (prefix == ArgType::MODE)
        {
            mode_ = readMode(str);
            ++pos;
        }
        else if (prefix == ArgType::OPTION)
        {
            Option option{readOption(str), {}};
            ++pos;

            std::vector<std::string> args;

            while (pos < argc &&
                   readPrefix(str = argv[pos]) == ArgType::OPTION_ARG)
            {
                args.push_back(std::move(str));
                ++pos;
            }

            option.args = std::move(args);
            options_.push_back(std::move(option));
        }
        else
        {
            assert(!"OK");
        }
    }
}

Parser::ArgType Parser::readPrefix(const std::string& str) const
{
    if (str.empty())
    {
        assert(!"OK");
    }
    else if (str[0] == '-')
    {
        if (str.size() > 1 && str[1] == '-')
        {
            return ArgType::MODE;
        }

        return ArgType::OPTION;
    }

    return ArgType::OPTION_ARG;
}

OptionName Parser::readOption(const std::string& str) const
{
    assert(!str.empty() && str[0] == '-');

    std::string temp = str.substr(1);
    if (STRING_TO_OPTION.count(temp) == 0)
    {
        std::cout << "Unknown option: " << temp << std::endl;
        assert(!"OK");
    }

    return STRING_TO_OPTION.at(temp);
}

Mode Parser::readMode(const std::string& str) const
{

    assert(str.size() > 1 && str.substr(0, 2) == "--");

    std::string temp = str.substr(2);
    if (STRING_TO_MODE.count(temp) == 0)
    {
        std::cout << "Unknown mode: " << temp << std::endl;
        assert(!"OK");
    }

    return STRING_TO_MODE.at(temp);
}

Mode Parser::getMode() const
{
    return mode_;
}

const Options& Parser::getOptions() const
{
    return options_;
}

} // namespace editor