#include "edit_impl.h"

#include <algorithm>
#include <iostream>
#include <cassert>

namespace editor::editimpl {

void applyChanges(const Options& options)
{
	auto itInput = std::find_if(options.begin(), options.end(), [](const Option& option)
															{
																return option.name == OptionName::INPUT;
															});
	if (itInput == options.end() || (*itInput).args.empty())
	{
		std::cout << "Input image was not specified" << std::endl;
		assert(!"OK");
	}

	std::string input  = (*itInput).args[0];

	ImageManager imageManager(input);

	for (const auto& option : options)
	{
		switch (option.name)
		{
			case OptionName::INPUT: 			break;

			case OptionName::OUTPUT:			if (!option.args.empty())
												{
													imageManager.setOutput(option.args[0]);
												}
												break;

			case OptionName::NEGATIVE:			imageManager.applyNegative();
												break;

			case OptionName::REPLACE_COLOR:		if (option.args.size() >= 2)
												{
													imageManager.replaceColor(
														option.args[0],
														option.args[1]
													);
												}
												else
												{
													std::cout << "Not enough arguments for replacing colors" << std::endl;
												}
												break;

			case OptionName::SHARPNESS_IMPROVE:	imageManager.improveSharpness();
												break;

			case OptionName::GAUSSIAN_BLUR:		imageManager.applyGaussianBlur();
												break;

			case OptionName::BORDER_HIGHLIGHT:	imageManager.highlightBorder();
												break;

			case OptionName::NOISE_REDUCTION:	imageManager.reductNoise();
												break;

			case OptionName::VIGNETTE:			imageManager.applyVignette();
												break;

			case OptionName::CUT:		 		if (option.args.size() >= 4)
												{
													imageManager.cutImage(
														option.args[0],
														option.args[1],
														option.args[2],
														option.args[3]
													);
												}
												break;

			default: 							assert(!"OK");
		}
	}

	imageManager.saveChanges();

}

}