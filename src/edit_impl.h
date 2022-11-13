#pragma once

#include "editor.h"

namespace editor::editimpl {

const uint8_t maxColorNum = 255;
const uint8_t bitsPerByte = 8;

struct RGB {

    RGB(uint8_t blue = 0, uint8_t red = 0, uint8_t green = 0);

    explicit RGB(int colorNum);

    uint8_t blue_ {0};
    uint8_t red_  {0};
    uint8_t green_{0};

    bool operator == (const RGB& obj);

} __attribute ((__packed__));


class ImageManager {

public:

    ImageManager(const std::string& input);

    void setOutput(const std::string& output);

    void applyNegative    ();
    void replaceColor     (int oldColor, int newColor);
    void improveSharpness ();
    void applyGaussianBlur();
    void applyGreyFilter  ();
    void highlightBorder  ();
    void reduceNoise      ();
    void applyVignette    ();
    void cutImage         (int x, int y, int width, int height);
    void compressImage    (int width, int height);

    std::string saveChanges();

private:

    struct BMPFileHeader {

        uint16_t fileType  {0x4D42};
        uint32_t fileSize  {0};
        uint16_t reserved1 {0};
        uint16_t reserved2 {0};
        uint32_t offsetData{0};
    } __attribute ((__packed__)) fileHeader_;


    struct BMPInfoHeader {

        uint32_t size           {0};
        int32_t  width          {0};
        int32_t  height         {0};
        uint16_t planes         {1};
        uint16_t bitCount       {0};
        uint32_t compression    {0};
        uint32_t sizeImage      {0};
        int32_t  xPixelsPerMeter{0};
        int32_t  yPixelsPerMeter{0};
        uint32_t colorsUsed     {0};
        uint32_t colorsImportant{0};
    } __attribute ((__packed__)) infoHeader_;


    struct BMPColorHeader {

        uint32_t redMask       {0x00ff0000}; // Bit mask for the red channel
        uint32_t greenMask     {0x0000ff00}; // Bit mask for the green channel
        uint32_t blueMask      {0x000000ff}; // Bit mask for the blue channel
        uint32_t alphaMask     {0xff000000}; // Bit mask for the alpha channel
        uint32_t colorSpaceType{0x73524742}; // Default "sRGB" (0x73524742)
        uint32_t unused[16]    {0};          // Unused data for sRGB color space
    } __attribute ((__packed__)) colorHeader_;

    std::string input_;
    std::string output_;

    std::vector<std::vector<RGB>> buffer_;
};

void applyChanges(const Options& options);

} // namespace editor::editimpl