#include <editor.h>
#include <parser.h>
#include <interactive.h>
#include <edit_impl.h>

#include <iostream>
#include <fstream>
#include <cassert>

namespace {

void printHelp(const std::string& helpFileName = "help.txt")
{
	std::ifstream helpFile(helpFileName);

    if (helpFile.is_open())
    {
        std::cout << helpFile.rdbuf();
    }
    else
    {
    	std::cout << "File with documentation could not be open :( " << std::endl;
    	assert(!"OK");
    }
}

}


int main(int argc, char* argv[])
{
	editor::Parser parser(argc, argv);

	switch (parser.getMode())
	{
		case editor::Mode::HELP: 		printHelp();
										break;

		case editor::Mode::INTERACTIVE: editor::interactive::runInteractive();
										break;

		case editor::Mode::DEFAULT:		editor::editimpl::applyChanges(parser.getOptions());
										break;

		default:						assert(!"OK");
	}
}