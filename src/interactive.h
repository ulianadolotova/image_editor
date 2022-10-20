#pragma once

#include "editor.h"

namespace editor::interactive {

enum class CommandName {

    LS,
    CD,
    LOAD,
    UNDO,
    SAVE,
    DROP
};

const std::unordered_map<std::string, CommandName> STRING_TO_COMMAND = {

    {"ls",      CommandName::LS},
    {"cd",      CommandName::CD},
    {"load",    CommandName::LOAD},
    {"undo",    CommandName::UNDO},
    {"save",    CommandName::SAVE},
    {"drop",    CommandName::DROP}

};

void runInteractive();

}