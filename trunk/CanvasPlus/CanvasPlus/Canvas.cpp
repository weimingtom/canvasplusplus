
#include "Canvas.h"
#include <Windows.h>
#include <string>
#include <sstream>
//#include "CanvasParserDFA.h"


CanvasPlus::Color::Color(const char*)
{
}

static CanvasPlus::Color backcolor = "rgb(255,0,0)";


int ParsePositiveInt(const wchar_t* psz)
{
  int result = 0; 
  while ((*psz >= '0') && (*psz <= '9'))
  { 
    result = result * 10 + (*psz - '0'); 
    psz++; 
  } 
  return result;
}


int ParseAndMovePositiveInt(const char*& psz)
{
  int result = 0; 
  while ((*psz >= '0') && (*psz <= '9'))
  { 
    result = result * 10 + (*psz - '0'); 
    psz++; 
  } 
  return result;
}


static bool Skip(const char*& psz)
{
   //"rgb()"
   while ((*psz < '0') || (*psz > '9'))
    {
        if(*psz == 0)
            return false; //terminou
        psz++;
    }
   return true;
}
void CanvasPlus::ParserColor(const char* psz, CanvasPlus::Color& color)
{
  if (!Skip(psz))
      return;
    
  color.r = ParseAndMovePositiveInt(psz);

  if (!Skip(psz))
      return;

  color.g = ParseAndMovePositiveInt(psz);

  if (!Skip(psz))
      return;

  color.b = ParseAndMovePositiveInt(psz);

  if (!Skip(psz))
      return;
  
  color.a = ParseAndMovePositiveInt(psz);

  if (!Skip(psz))
      return;
}


CanvasPlus::TextBaselineEnum CanvasPlus::ParseTextBaseline(const char* psz)
{  
  if (strcmp(psz, "top") == 0)
  {
    return CanvasPlus::TextBaselineTop;
  }
  else if (strcmp(psz, "hanging") == 0)
  {
    return CanvasPlus::TextBaselineHanging;
  }
  else if (strcmp(psz, "middle") == 0)
  {
    return CanvasPlus::TextBaselineMiddle;
  }
  else if (strcmp(psz, "alphabetic") == 0)
  {
    return CanvasPlus::TextBaselineAlphabetic;
  }
  else if (strcmp(psz, "ideographic") == 0)
  {
    return CanvasPlus::TextBaselineIdeographic;
  }
  else if (psz == "bottom" || strcmp(psz, "bottom") == 0)
  {
    return CanvasPlus::TextBaselineBottom;
  }

  //assert
  return CanvasPlus::TextBaselineTop;
}


 CanvasPlus::TextAlignEnum CanvasPlus::ParseTextAlign(const char* psz)
 {
    if (strcmp(psz, "start") == 0)
  {
    return CanvasPlus::TextAlignStart;
  }
  else if (strcmp(psz, "end") == 0)
  {
    return CanvasPlus::TextAlignEnd;
  }
     else if (strcmp(psz, "left") == 0)
  {
    return CanvasPlus::TextAlignLeft;
  }
   else if (strcmp(psz, "right") == 0)
  {
    return CanvasPlus::TextAlignRight;
  }
   else if (strcmp(psz, "center") == 0)
  {
    return CanvasPlus::TextAlignCenter;
  }

    //assert
    return CanvasPlus::TextAlignLeft;
 }