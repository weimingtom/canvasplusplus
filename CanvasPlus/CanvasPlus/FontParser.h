#pragma once

#include <string>

struct Actions
{
    //Actions
    virtual void SetFontName(const std::string&) {}
    virtual void SetNormalStyle() {}
    virtual void SetItalic() {}
    virtual void SetOblique() {}
    virtual void SetNormalWeight() {}
    virtual void SetWeightBold() {}
    virtual void SetWeight100() {}
    virtual void SetWeight200() {}
    virtual void SetWeight300() {}
    virtual void SetWeight400() {}
    virtual void SetWeight500() {}
    virtual void SetWeight600() {}
    virtual void SetWeight700() {}
    virtual void SetWeight800() {}
    virtual void SetWeight900() {}
    virtual void SetSizePercent() {}
    virtual void SetSizePoints(int) {}
    virtual void SetSizePixels(int) {}
    virtual void SetSizeLarger() {}
    virtual void SetSizeSmaller() {}
    virtual void SetSizexxsmall() {}
    virtual void SetSizexsmall() {}
    virtual void SetSizesmall() {}
    virtual void SetSizemedium() {}
    virtual void SetSizelarge() {}
    virtual void SetSizexlarge() {}
    virtual void SetSizexxlarge() {}
};

//http://www.w3.org/TR/CSS2/fonts.html#font-shorthand
void ParseFont(const std::string& str, Actions& actions);

