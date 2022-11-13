#include "edit_impl.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>

namespace editor::editimpl {

namespace {

void applyMatrix(std::vector<std::vector<RGB>>& buffer, const std::vector<std::vector<double>>& matrix)
{
    size_t width  = buffer.size();
    size_t height = buffer[0].size();

    std::vector<std::vector<RGB>> extBuffer(width + 2, std::vector<RGB>(height + 2, 0));

    for (size_t x = 1; x < width + 1; ++x)
    {
        for (size_t y = 1; y < height + 1; ++y)
        {
            extBuffer[x][y] = buffer[x - 1][y - 1];
        }
    }

    extBuffer[0]         = extBuffer[1];
    extBuffer[width + 1] = extBuffer[width];

    for (size_t x = 0; x < width + 2; ++x)
    {
        extBuffer[x][0]          = extBuffer[x][1];
        extBuffer[x][height + 1] = extBuffer[x][height];
    }

    extBuffer[0][0]                   = extBuffer[1][1];
    extBuffer[0][height + 1]          = extBuffer[1][height];
    extBuffer[width + 1][0]           = extBuffer[width][1];
    extBuffer[width + 1][height + 1]  = extBuffer[width][height];

    for (size_t x = 1; x < width + 1; ++x)
    {
        for (size_t y = 1; y < height + 1; ++y)
        {
            {
                double newBlue  = 0;
                double newRed   = 0;
                double newGreen = 0;

                for (size_t i = x - 1; i <= x + 1; ++i)
                {
                    for (size_t j = y - 1; j <= y + 1; ++j)
                    {
                        newBlue  += static_cast<double>(extBuffer[i][j].blue_)  * matrix[i - (x - 1)][j - (y - 1)];
                        newRed   += static_cast<double>(extBuffer[i][j].red_)   * matrix[i - (x - 1)][j - (y - 1)];
                        newGreen += static_cast<double>(extBuffer[i][j].green_) * matrix[i - (x - 1)][j - (y - 1)];
                    }
                }

                std::vector<double> colors = {newBlue, newRed, newGreen};

                for (auto& color : colors)
                {
                    if (color < 0)
                    {
                        color = 0;
                    }
                    else if (color > 255)
                    {
                        color = 255;
                    }
                }

                buffer[x - 1][y - 1] = RGB(colors[0], colors[1], colors[2]);
            }
        }
    }
}

} // namespace


RGB::RGB(uint8_t blue, uint8_t red, uint8_t green)
    : blue_{blue}
    , red_{red}
    , green_{green}
{
}

RGB::RGB(int colorNum)
{
    red_   = (colorNum >> (2 * bitsPerByte)) & 0xff;
    green_ = (colorNum >> (1 * bitsPerByte)) & 0xff;
    blue_  = (colorNum >> (0 * bitsPerByte)) & 0xff;
}

bool RGB::operator == (const RGB& obj)
{
    return blue_  == obj.blue_  &&
           green_ == obj.green_ &&
           red_   == obj.red_;
}


ImageManager::ImageManager(const std::string& input)
{
    input_  = input;
    output_ = input;

    std::ifstream image(input_, std::ios::binary);

    if (!image.is_open())
    {
        std::cout << "Image \"" << input_ << "\" could not be open :(" << std::endl;
        assert(!"OK");
    }

    image.read(reinterpret_cast<char*>(&fileHeader_),  sizeof(BMPFileHeader));
    image.read(reinterpret_cast<char*>(&infoHeader_),  sizeof(BMPInfoHeader));
    image.read(reinterpret_cast<char*>(&colorHeader_), sizeof(BMPColorHeader));


    size_t padSize = (infoHeader_.sizeImage - infoHeader_.width * infoHeader_.height * sizeof(RGB)) / infoHeader_.height;
    char padArray[padSize];

    buffer_.assign(infoHeader_.width, std::vector<RGB>(infoHeader_.height));

    image.seekg(fileHeader_.offsetData);

    for (int y = infoHeader_.height - 1; y >= 0; --y)
    {
        for (int x = 0; x < infoHeader_.width; ++x)
        {
            image.read(reinterpret_cast<char*> (&buffer_[x][y]), sizeof(RGB));
        }
        image.read(padArray, padSize);
    }
}

void ImageManager::setOutput(const std::string& output)
{
    output_ = output;
}

void ImageManager::applyNegative()
{
    for (auto& row : buffer_)
    {
        for (auto& elem : row)
        {
            elem = {
                maxColorNum - elem.blue_,
                maxColorNum - elem.green_,
                maxColorNum - elem.red_
            };
        }
    }
}

void ImageManager::replaceColor(int oldColor, int newColor)
{
    RGB toFind(oldColor);
    RGB toReplace(newColor);

    for (auto& row : buffer_)
    {
        for (auto& elem : row)
        {
            if (elem == toFind)
            {
                elem = toReplace;
            }
        }
    }
}

void ImageManager::improveSharpness()
{
    const static std::vector<std::vector<double>> matrix = {
                                            {-1, -1, -1},
                                            {-1,  9, -1},
                                            {-1, -1, -1}
                                        };
    applyMatrix(buffer_, matrix);
}

void ImageManager::applyGaussianBlur()
{
    std::vector<std::vector<double>> matrix = {
                                            {0.054901, 0.111345, 0.054901},
                                            {0.111345, 0.225821, 0.111345},
                                            {0.054901, 0.111345, 0.054901}
                                        };
    applyMatrix(buffer_, matrix);
}

void ImageManager::applyGreyFilter()
{
    for (auto& row : buffer_)
    {
        for (auto& elem : row)
        {
            uint8_t newColor = (elem.blue_ + elem.red_ + elem.green_) / 3;
            elem = RGB(newColor, newColor, newColor);
        }
    }
}

void ImageManager::highlightBorder()
{
    applyGreyFilter();

    auto tempBuffer = buffer_;

    const static std::vector<std::vector<double>> firstMatrix = {
                                        {-1, -2, -1},
                                        { 0,  0,  0},
                                        { 1,  2,  1}
                                    };

    const static std::vector<std::vector<double>> secondMatrix = {
                                        {-1,  0,  1},
                                        {-2,  0,  2},
                                        {-1, -1, -1}
                                    };

    applyMatrix(buffer_,    firstMatrix);
    applyMatrix(tempBuffer, secondMatrix);

    for (size_t x = 0; x < infoHeader_.width; ++x)
    {
        for (size_t y = 0; y < infoHeader_.height; ++y)
        {
            auto first  = buffer_[x][y];
            auto second = tempBuffer[x][y];

            uint32_t newBlue  = std::min(255, static_cast<int>(std::sqrt(first.blue_  * first.blue_  + second.blue_  * second.blue_)));
            uint32_t newRed   = std::min(255, static_cast<int>(std::sqrt(first.red_   * first.red_   + second.red_   * second.red_)));
            uint32_t newGreen = std::min(255, static_cast<int>(std::sqrt(first.green_ * first.green_ + second.green_ * second.green_)));

            const uint32_t limit = 255 * 0.3;

            if (newBlue < limit || newRed < limit || newGreen < limit)
            {
                buffer_[x][y] = RGB(0, 0, 0);
            }
            else
            {
                buffer_[x][y] = RGB(newBlue, newRed, newGreen);
            }
        }
    }
}

void ImageManager::reduceNoise()
{
    size_t width  = infoHeader_.width;
    size_t height = infoHeader_.height;

    std::vector<std::vector<RGB>> extBuffer(width + 2, std::vector<RGB>(height + 2, 0));

    for (size_t x = 1; x < width + 1; ++x)
    {
        for (size_t y = 1; y < height + 1; ++y)
        {
            extBuffer[x][y] = buffer_[x - 1][y - 1];
        }
    }

    extBuffer[0]         = extBuffer[1];
    extBuffer[width + 1] = extBuffer[width];

    for (size_t x = 0; x < width + 2; ++x)
    {
        extBuffer[x][0]          = extBuffer[x][1];
        extBuffer[x][height + 1] = extBuffer[x][height];
    }

    extBuffer[0][0]                   = extBuffer[1][1];
    extBuffer[0][height + 1]          = extBuffer[1][height];
    extBuffer[width + 1][0]           = extBuffer[width][1];
    extBuffer[width + 1][height + 1]  = extBuffer[width][height];

    for (size_t x = 1; x < width + 1; ++x)
    {
        for (size_t y = 1; y < height + 1; ++y)
        {
            {
                std::vector<uint32_t> tempBlue;
                std::vector<uint32_t> tempRed;
                std::vector<uint32_t> tempGreen;

                for (size_t i = x - 1; i <= x + 1; ++i)
                {
                    for (size_t j = y - 1; j <= y + 1; ++j)
                    {
                        tempBlue.push_back (extBuffer[i][j].blue_);
                        tempRed.push_back  (extBuffer[i][j].red_);
                        tempGreen.push_back(extBuffer[i][j].green_);
                    }
                }

                std::sort(tempBlue.begin(),  tempBlue.end());
                std::sort(tempRed.begin(),   tempRed.end());
                std::sort(tempGreen.begin(), tempGreen.end());

                size_t pos = (tempBlue.size() - 1) / 2;

                buffer_[x - 1][y - 1] = RGB(tempBlue[pos], tempRed[pos], tempGreen[pos]);
            }
        }
    }
}

void ImageManager::applyVignette()
{
    
}

void ImageManager::cutImage(int x, int y, int width, int height)
{
    if (x < 0 || y < 0 || width < 0 || height < 0)
    {
        std::cout << "Size can not be a negative number" << std::endl;
        assert(!"OK");
    }

    if (x + width > infoHeader_.width || y + height > infoHeader_.height)
    {
        std::cout << "Incorrect borders" << std::endl;
        assert(!"OK");
    }

    std::vector<std::vector<RGB>> newImage(width, std::vector<RGB>(height));

    for (size_t i = 0; i < width; ++i)
    {
        for (size_t j = 0; j < height; ++j)
        {
            newImage[i][j] = buffer_[i + x][j + y];
        }
    }

    buffer_ = std::move(newImage);

    infoHeader_.width  = width;
    infoHeader_.height = height;
}

void ImageManager::compressImage(int width, int height)
{

}

std::string ImageManager::saveChanges()
{
    char padArray[] = {'\0', '\0', '\0', '\0'};
    size_t padSize = (4 - infoHeader_.width * sizeof(RGB) % 4) % 4;

    infoHeader_.sizeImage = (infoHeader_.width * sizeof(RGB) + padSize) * infoHeader_.height;
    fileHeader_.fileSize = sizeof(BMPFileHeader) + infoHeader_.size + infoHeader_.sizeImage;

    std::ofstream image(output_, std::ios::binary | std::ios::trunc);

    if (!image.is_open())
    {
        std::cout << "Image \"" << output_ << "\" could not be open :(" << std::endl;
        assert(!"OK");
    }

    image.write(reinterpret_cast<const char*>(&fileHeader_),  sizeof(BMPFileHeader));
    image.write(reinterpret_cast<const char*>(&infoHeader_),  sizeof(BMPInfoHeader));
    image.write(reinterpret_cast<const char*>(&colorHeader_), sizeof(BMPColorHeader));

    for (int y = infoHeader_.height - 1; y >= 0; --y)
    {
        for (int x = 0; x < infoHeader_.width; ++x)
        {
            image.write(reinterpret_cast<char*> (&buffer_[x][y]), sizeof(RGB));
        }
        image.write(padArray, padSize);
    }

    return output_;
}

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
            case OptionName::INPUT:             break;

            case OptionName::OUTPUT:            if (!option.args.empty())
                                                {
                                                    imageManager.setOutput(option.args[0]);
                                                }
                                                break;

            case OptionName::NEGATIVE:          imageManager.applyNegative();
                                                break;

            case OptionName::REPLACE_COLOR:     if (option.args.size() >= 2)
                                                {
                                                    imageManager.replaceColor(
                                                        std::stoi(option.args[0], nullptr, 16),
                                                        std::stoi(option.args[1], nullptr, 16)
                                                    );
                                                }
                                                else
                                                {
                                                    std::cout << "Not enough arguments for replacing colors" << std::endl;
                                                }
                                                break;

            case OptionName::SHARPNESS_IMPROVE: imageManager.improveSharpness();
                                                break;

            case OptionName::GAUSSIAN_BLUR:     imageManager.applyGaussianBlur();
                                                break;

            case OptionName::GREY_FILTER:       imageManager.applyGreyFilter();
                                                break;

            case OptionName::BORDER_HIGHLIGHT:  imageManager.highlightBorder();
                                                break;

            case OptionName::NOISE_REDUCTION:   imageManager.reduceNoise();
                                                break;

            case OptionName::VIGNETTE:          imageManager.applyVignette();
                                                break;

            case OptionName::CUT:               if (option.args.size() >= 4)
                                                {
                                                    imageManager.cutImage(
                                                        std::stoi(option.args[0]),
                                                        std::stoi(option.args[1]),
                                                        std::stoi(option.args[2]),
                                                        std::stoi(option.args[3])
                                                    );
                                                }
                                                else
                                                {
                                                    std::cout << "Not enough arguments for image cut" << std::endl;
                                                }
                                                break;

            case OptionName::COMPRESS:          if (option.args.size() >= 2)
                                                {
                                                    imageManager.compressImage(
                                                        std::stoi(option.args[0]),
                                                        std::stoi(option.args[1])
                                                    );
                                                }
                                                else
                                                {
                                                    std::cout << "Not enough arguments for image compress" << std::endl;
                                                }
                                                break;

            default:                            assert(!"OK");
        }
    }

    std::cout << "Changes were saved to: " << imageManager.saveChanges() << std::endl;
}

} // namespace editor::editimpl