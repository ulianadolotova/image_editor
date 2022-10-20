#pragma once

#include "editor.h"

namespace editor {

class Parser {

public:

	Parser(int argc, char* argv[]);

	Mode           getMode   () const;
	const Options& getOptions() const;

private:

	enum class ArgType { MODE, OPTION, OPTION_ARG };

	Mode mode_ = Mode::DEFAULT;
	Options options_;

	ArgType    readPrefix(const std::string& str) const;
	Mode       readMode  (const std::string& str) const;
	OptionName readOption(const std::string& str) const;
};

}