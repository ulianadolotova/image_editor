#pragma once

#include "editor.h"

namespace editor::editimpl {

class ImageManager {

public:

	ImageManager(const std::string& input);

	void setOutput(const std::string& output);
	void applyNegative();
	void replaceColor(int oldColor, int newColor);
	void improveSharpness();
	void applyGaussianBlur();
	void highlightBorder();
	void reductNoise();
	void applyVignette();
	void cutImage(int x, int y, int width, int height);

private:

	std::string input;
	std::string output;

};

void applyChanges(const Options& options);

}