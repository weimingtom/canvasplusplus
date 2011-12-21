#include "StdAfx.h"
#include "CanvasRenderingContext2D.h"
#include <Windows.h>
#include <string>
#include <sstream>
#include "CanvasParserDFA.h"

template < class DFATraits >
class Tokenizer
{
    typedef char CharType;

    const CharType* m_str;
    const CharType* m_it;
    CharType m_lastChar;

    void PutBack(CharType ch)
    {
        m_lastChar = ch;
    }

    bool NextChar(CharType& ch)
    {
        if (m_lastChar != 0)
        {
            ch = m_lastChar;
            m_lastChar  = 0;
        }
        else
        {
            if (*m_it == 0)
            {
                return false;
            }

            ch = *m_it++;
        }

        return true;
    }

public:

    typedef typename DFATraits::TokenType TokenType;

    Tokenizer(const CharType* psz) : m_str(psz)
    {
        m_lastChar = 0;
        m_it = psz;
    }

    template<class T>
    bool NextToken(T& lexeme, typename DFATraits::TokenType& tk)
    {
        for (;;)
        {
            if (!NextTokenNoInterleave(lexeme, tk))
            {
                return false;
            }

            if (!DFATraits::IsInterleave(tk))
            {
                return true;
            }
        }
    }

    template<class T>
    bool NextTokenNoInterleave(T& lexeme, typename DFATraits::TokenType& tk)
    {
        lexeme.clear();
        int lastGoodState = -1;
        int currentState = 0;
        CharType ch;

        while (NextChar(ch))
        {
            currentState = DFATraits::GetNext(currentState, ch);

            if (currentState == -1)
            {
                PutBack(ch);
                break;
            }

            typename DFATraits::TokenType tk2;

            if (DFATraits::GetTokenFromState(currentState, tk2))
            {
                tk = tk2;
                lastGoodState = currentState;
            }

            lexeme += ch;
        }

        return (lastGoodState != -1);
    }
};

typedef Tokenizer<CanvasParser::DFA> CanvasTokenizer;

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

int ParsePositiveInt(const std::wstring& ws)
{
  return ParsePositiveInt(ws.c_str());
}

void CanvasPlus::ParserColor(const char* psz, CanvasPlus::Color& color)
{
    //TODO Better code! Handle Errors


    //ctx.fillStyle = "orange";  
    //ctx.fillStyle = "#FFA500";  
    //ctx.fillStyle = "rgb(255,165,0)";  
    //ctx.fillStyle = "rgba(255,165,0,1)"; 

    //rgb just to test now...

    CanvasTokenizer tk(psz);
    std::wstring ws;
    CanvasParser::Tokens token;
    tk.NextToken(ws, token); //rgb rgba
    bool hasa = (token == (CanvasParser::TokenTKRGBA));
    tk.NextToken(ws, token); //(
    tk.NextToken(ws, token); //r
    color.r = ParsePositiveInt(ws);
    tk.NextToken(ws, token); //,
    tk.NextToken(ws, token); //g
    color.g = ParsePositiveInt(ws);
    tk.NextToken(ws, token); //,
    tk.NextToken(ws, token); //b
    color.b = ParsePositiveInt(ws);

    if (hasa)
    {
        tk.NextToken(ws, token); //,
        tk.NextToken(ws, token); //a
        color.a = ParsePositiveInt(ws);
    }

    tk.NextToken(ws, token); //)
}

