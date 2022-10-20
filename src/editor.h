#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace editor {

enum class Mode {

	HELP,
	INTERACTIVE,
	DEFAULT
};

enum class OptionName {

	INPUT,
	OUTPUT,
	NEGATIVE,
	REPLACE_COLOR,
	SHARPNESS_IMPROVE,
	GAUSSIAN_BLUR,
	BORDER_HIGHLIGHT,
	NOISE_REDUCTION,
	VIGNETTE,
	CUT
};

struct Option {

	OptionName name;
	std::vector<std::string> args;
};

using Options = std::vector<Option>;

const std::unordered_map<std::string, OptionName> STRING_TO_OPTION = {

	{"i",                   OptionName::INPUT},
	{"o",                   OptionName::OUTPUT},
	{"negative",            OptionName::NEGATIVE},
	{"replace_color",       OptionName::REPLACE_COLOR},
	{"sharpness_improve",   OptionName::SHARPNESS_IMPROVE},
	{"gaussian_blur",       OptionName::GAUSSIAN_BLUR},
	{"border_highlight",	OptionName::BORDER_HIGHLIGHT},
	{"noise_reduction",     OptionName::NOISE_REDUCTION},
	{"vignette",            OptionName::VIGNETTE},
	{"cut",                 OptionName::CUT}
};

const std::unordered_map<std::string, Mode> STRING_TO_MODE = {

	{"help",        Mode::HELP},
	{"interactive", Mode::INTERACTIVE}
};

}
