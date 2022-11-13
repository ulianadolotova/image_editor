#include "interactive.h"
#include "edit_impl.h"

#include <filesystem>
#include <iostream>
#include <cassert>
#include <sstream>
#include <utility>

namespace editor::interactive {

namespace {

void listSubdirectories()
{
    for (auto const& dir_entry : std::filesystem::directory_iterator(std::filesystem::current_path()))
    {
        std::cout << dir_entry.path().filename() << std::endl;
    }
}

void changeDirectory()
{
    std::string newDir;
    std::cin >> newDir;

    std::filesystem::current_path(newDir);
}

void processLoad()
{
    Options options;

    std::string inputImage;
    std::cin >> inputImage;

    options.push_back({OptionName::INPUT, {inputImage}});

    std::string str;
    std::string curCommand;

    while (std::getline(std::cin, str))
    {
        if (str.empty())
        {
            continue;
        }

        std::istringstream curLine(str);
        curLine >> curCommand;

        if (STRING_TO_COMMAND.count(curCommand) != 0)
        {
            CommandName commandName = STRING_TO_COMMAND.at(curCommand);

            switch (commandName)
            {
                case CommandName::UNDO:     if (options.size() > 1)
                                            {
                                                options.pop_back();
                                            }
                                            break;

                case CommandName::SAVE:     {
                                                std::string outputImage;
                                                if (curLine >> outputImage)
                                                {
                                                    options.push_back({OptionName::OUTPUT, {outputImage}});
                                                }
                                                editimpl::applyChanges(options);
                                                return;
                                            }

                case CommandName::DROP:     return;

                case CommandName::LS:
                case CommandName::CD:
                case CommandName::LOAD:
                case CommandName::QUIT:     std::cout << "To use this command \"" << curCommand
                                                      << "\" you should save current changes or drop them first" << std::endl;
                                            break;

                default:                    assert(!"OK");
            }
        }
        else if (STRING_TO_OPTION.count(curCommand) != 0)
        {
            Option option {STRING_TO_OPTION.at(curCommand), {}};
            std::string arg;
            while (curLine >> arg)
            {
                option.args.push_back(std::move(arg));
            }
            options.push_back(std::move(option));
        }
        else
        {
            std::cout << "Unknown command or option: \"" << curCommand << "\"" << std::endl;
            assert(!"OK");
        }
    }
}

} // namespace

void runInteractive()
{
    std::string curCommand;

    while (std::cin >> curCommand)
    {
        if (STRING_TO_COMMAND.count(curCommand) == 0)
        {
            std::cout << "Unknown command in interactive mode: \"" << curCommand << "\"" << std::endl;
            std::cout << "Please, enter another one" << std::endl;
            continue;
        }

        CommandName commandName = STRING_TO_COMMAND.at(curCommand);

        switch (commandName)
        {
            case CommandName::LS:       listSubdirectories();
                                        break;

            case CommandName::CD:       changeDirectory();
                                        break;

            case CommandName::LOAD:     processLoad();
                                        break;

            case CommandName::QUIT:     return;

            case CommandName::UNDO:
            case CommandName::SAVE:
            case CommandName::DROP:     std::cout << "To use this command: \"" << curCommand
                                                  << "\" you should load the picture first" << std::endl;
                                        break;

            default:                    assert(!"OK");
        }
    }
}

} // namespace editor::interactive