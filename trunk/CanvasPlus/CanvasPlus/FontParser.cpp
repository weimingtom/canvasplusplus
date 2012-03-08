#include <cassert>
#include <string>
#include "FontParser.h"

namespace FontShorthand
{
    template < class TChar >
    struct StringTokenizerStream
    {
        typedef TChar CharType;
        const std::basic_string<CharType>& m_str;
        typedef typename std::basic_string<CharType>::const_iterator Iterator;
        Iterator m_it;
        CharType m_lastChar;

    public:

        StringTokenizerStream(typename const std::basic_string<CharType>& s) : m_str(s)
        {
            m_lastChar = 0;
            m_it = m_str.begin();
        }

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
                if (m_it == m_str.end())
                {
                    return false;
                }

                ch = *m_it++;
            }

            return true;
        }
    };


    template < class DFATraits, class TokenizerStreamT >
    struct Tokenizer
    {
        TokenizerStreamT& m_TokenizerStream;

        template<class T>
        bool NextTokenNoInterleave(T& lexeme, typename DFATraits::TokenType& tk)
        {
            lexeme.clear();
            int lastGoodState = -1;
            int currentState = 0;
            typename TokenizerStreamT::CharType ch;

            while (m_TokenizerStream.NextChar(ch))
            {
                currentState = DFATraits::GetNext(currentState, ch);

                if (currentState == -1)
                {
                    m_TokenizerStream.PutBack(ch);
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

        bool NextTokenNoInterleave(typename DFATraits::TokenType& tk)
        {
            int lastGoodState = -1;
            int currentState = 0;
            typename TokenizerStreamT::CharType ch;

            while (m_TokenizerStream.NextChar(ch))
            {
                currentState = DFATraits::GetNext(currentState, ch);

                if (currentState == -1)
                {
                    m_TokenizerStream.PutBack(ch);
                    break;
                }

                typename DFATraits::TokenType tk2;

                if (DFATraits::GetTokenFromState(currentState, tk2))
                {
                    tk = tk2;
                    lastGoodState = currentState;
                }
            }

            return (lastGoodState != -1);
        }

    public:

        typedef typename DFATraits::TokenType TokenType;

        Tokenizer(TokenizerStreamT& charEnumerator) : m_TokenizerStream(charEnumerator)
        {
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

        bool NextToken(typename DFATraits::TokenType& tk)
        {
            for (;;)
            {
                if (!NextTokenNoInterleave(tk))
                {
                    return false;
                }

                if (!DFATraits::IsInterleave(tk))
                {
                    return true;
                }
            }
        }
    };

    enum Tokens
    {
        TKBLANKS,
        TKNormal,
        TKItalic,
        TKOblique,
        TKBold,
        TKxxsmall,
        TKxsmall,
        TKsmall,
        TKmedium,
        TKlarge,
        TKxlarge,
        TKxxlarge,
        TKlarger,
        TKsmaller,
        TKA100,
        TKA200,
        TKA300,
        TKA400,
        TKA500,
        TKA600,
        TKA700,
        TKA800,
        TKA900,
        TKPercent,
        TKPoints,
        TKPixels,
        TKFontName,
    };

struct DFA
{
    typedef Tokens TokenType;

    static int GetNext(int state, wchar_t ch)
    {
        switch (state)
        {
            case 0:
            if (ch == L' ')
                return 1;
            else if (ch == L'"')
                return 2;
            else if (ch == L'-')
                return 3;
            else if (ch == L'0')
                return 4;
            else if (ch == L'1')
                return 5;
            else if (ch == L'2')
                return 6;
            else if (ch == L'3')
                return 7;
            else if (ch == L'4')
                return 8;
            else if (ch == L'5')
                return 9;
            else if (ch == L'6')
                return 10;
            else if (ch == L'7')
                return 11;
            else if (ch == L'8')
                return 12;
            else if (ch == L'9')
                return 13;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
            else if (ch == L'a')
                return 3;
            else if (ch == L'b')
                return 14;
        else if (ch >= L'c' && ch <= L'h')
          return 3;
            else if (ch == L'i')
                return 15;
        else if (ch >= L'j' && ch <= L'k')
          return 3;
            else if (ch == L'l')
                return 16;
            else if (ch == L'm')
                return 17;
            else if (ch == L'n')
                return 18;
            else if (ch == L'o')
                return 19;
        else if (ch >= L'p' && ch <= L'r')
          return 3;
            else if (ch == L's')
                return 20;
        else if (ch >= L't' && ch <= L'w')
          return 3;
            else if (ch == L'x')
                return 21;
        else if (ch >= L'y' && ch <= L'z')
           return 3;
    break; // 
            case 1:
            if (ch == L' ')
                return 1;
        //TKBLANKS
    break; // 
            case 2:
        if (ch >= L' ' && ch <= L'!')
          return 2;
            else if (ch == L'"')
                return 23;
        else if (ch >= L'#' && ch <= L'[')
          return 2;
            else if (ch == L'\\')
                return 24;
        else if (ch >= L']' && ch <= L'~')
           return 2;
    break; // 
            case 3:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 4:
            if (ch == L'%')
                return 25;
        else if (ch >= L'0' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 5:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 27;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 6:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 28;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 7:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 29;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 8:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 30;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 9:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 31;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 10:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 32;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 11:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 33;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 12:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 34;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 13:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 35;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 14:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'n')
          return 3;
            else if (ch == L'o')
                return 36;
        else if (ch >= L'p' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 15:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L's')
          return 3;
            else if (ch == L't')
                return 37;
        else if (ch >= L'u' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 16:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
            else if (ch == L'a')
                return 38;
        else if (ch >= L'b' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 17:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'd')
          return 3;
            else if (ch == L'e')
                return 39;
        else if (ch >= L'f' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 18:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'n')
          return 3;
            else if (ch == L'o')
                return 40;
        else if (ch >= L'p' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 19:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
            else if (ch == L'a')
                return 3;
            else if (ch == L'b')
                return 41;
        else if (ch >= L'c' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 20:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'l')
          return 3;
            else if (ch == L'm')
                return 42;
        else if (ch >= L'n' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 21:
            if (ch == L'-')
                return 43;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'w')
          return 3;
            else if (ch == L'x')
                return 44;
        else if (ch >= L'y' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 22:
  return -1;
    break; // 
            case 23:
        //TKFontName
    break; // 
            case 24:
  return 2; //any
    break; // 
            case 25:
        //TKPercent
    break; // 
            case 26:
            if (ch == L't')
                return 45;
        else if (ch == L'x')
          return 46;
    break; // 
            case 27:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 47;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 28:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 48;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 29:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 49;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 30:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 50;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 31:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 51;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 32:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 52;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 33:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 53;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 34:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 54;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 35:
            if (ch == L'%')
                return 25;
            else if (ch == L'0')
                return 55;
        else if (ch >= L'1' && ch <= L'9')
          return 4;
        else if (ch == L'p')
          return 26;
    break; // 
            case 36:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'k')
          return 3;
            else if (ch == L'l')
                return 56;
        else if (ch >= L'm' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 37:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
            else if (ch == L'a')
                return 57;
        else if (ch >= L'b' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 38:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'q')
          return 3;
            else if (ch == L'r')
                return 58;
        else if (ch >= L's' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 39:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'c')
          return 3;
            else if (ch == L'd')
                return 59;
        else if (ch >= L'e' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 40:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'q')
          return 3;
            else if (ch == L'r')
                return 60;
        else if (ch >= L's' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 41:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'k')
          return 3;
            else if (ch == L'l')
                return 61;
        else if (ch >= L'm' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 42:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
            else if (ch == L'a')
                return 62;
        else if (ch >= L'b' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 43:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'k')
          return 3;
            else if (ch == L'l')
                return 63;
        else if (ch >= L'm' && ch <= L'r')
          return 3;
            else if (ch == L's')
                return 64;
        else if (ch >= L't' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 44:
            if (ch == L'-')
                return 65;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 45:
        //TKPoints
    break; // 
            case 46:
        //TKPixels
    break; // 
            case 47:
            if (ch == L'%')
                return 25;
        else if (ch >= L'0' && ch <= L'9')
          return 4;
            else if (ch == L'p')
                return 26;
        //TKA100
    break; // 
            case 48:
            if (ch == L'%')
                return 25;
        else if (ch >= L'0' && ch <= L'9')
          return 4;
            else if (ch == L'p')
                return 26;
        //TKA200
    break; // 
            case 49:
            if (ch == L'%')
                return 25;
        else if (ch >= L'0' && ch <= L'9')
          return 4;
            else if (ch == L'p')
                return 26;
        //TKA300
    break; // 
            case 50:
            if (ch == L'%')
                return 25;
        else if (ch >= L'0' && ch <= L'9')
          return 4;
            else if (ch == L'p')
                return 26;
        //TKA400
    break; // 
            case 51:
            if (ch == L'%')
                return 25;
        else if (ch >= L'0' && ch <= L'9')
          return 4;
            else if (ch == L'p')
                return 26;
        //TKA500
    break; // 
            case 52:
            if (ch == L'%')
                return 25;
        else if (ch >= L'0' && ch <= L'9')
          return 4;
            else if (ch == L'p')
                return 26;
        //TKA600
    break; // 
            case 53:
            if (ch == L'%')
                return 25;
        else if (ch >= L'0' && ch <= L'9')
          return 4;
            else if (ch == L'p')
                return 26;
        //TKA700
    break; // 
            case 54:
            if (ch == L'%')
                return 25;
        else if (ch >= L'0' && ch <= L'9')
          return 4;
            else if (ch == L'p')
                return 26;
        //TKA800
    break; // 
            case 55:
            if (ch == L'%')
                return 25;
        else if (ch >= L'0' && ch <= L'9')
          return 4;
            else if (ch == L'p')
                return 26;
        //TKA900
    break; // 
            case 56:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'c')
          return 3;
            else if (ch == L'd')
                return 66;
        else if (ch >= L'e' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 57:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'k')
          return 3;
            else if (ch == L'l')
                return 67;
        else if (ch >= L'm' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 58:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'f')
          return 3;
            else if (ch == L'g')
                return 68;
        else if (ch >= L'h' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 59:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'h')
          return 3;
            else if (ch == L'i')
                return 69;
        else if (ch >= L'j' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 60:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'l')
          return 3;
            else if (ch == L'm')
                return 70;
        else if (ch >= L'n' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 61:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'h')
          return 3;
            else if (ch == L'i')
                return 71;
        else if (ch >= L'j' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 62:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'k')
          return 3;
            else if (ch == L'l')
                return 72;
        else if (ch >= L'm' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 63:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
            else if (ch == L'a')
                return 73;
        else if (ch >= L'b' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 64:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'l')
          return 3;
            else if (ch == L'm')
                return 74;
        else if (ch >= L'n' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 65:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'k')
          return 3;
            else if (ch == L'l')
                return 75;
        else if (ch >= L'm' && ch <= L'r')
          return 3;
            else if (ch == L's')
                return 76;
        else if (ch >= L't' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 66:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'z')
          return 3;
        //TKBold
        //TKBold
    break; // 
            case 67:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'h')
          return 3;
            else if (ch == L'i')
                return 77;
        else if (ch >= L'j' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 68:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'd')
          return 3;
            else if (ch == L'e')
                return 78;
        else if (ch >= L'f' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 69:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L't')
          return 3;
            else if (ch == L'u')
                return 79;
        else if (ch >= L'v' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 70:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
            else if (ch == L'a')
                return 80;
        else if (ch >= L'b' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 71:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'p')
          return 3;
            else if (ch == L'q')
                return 81;
        else if (ch >= L'r' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 72:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'k')
          return 3;
            else if (ch == L'l')
                return 82;
        else if (ch >= L'm' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 73:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'q')
          return 3;
            else if (ch == L'r')
                return 83;
        else if (ch >= L's' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 74:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
            else if (ch == L'a')
                return 84;
        else if (ch >= L'b' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 75:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
            else if (ch == L'a')
                return 85;
        else if (ch >= L'b' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 76:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'l')
          return 3;
            else if (ch == L'm')
                return 86;
        else if (ch >= L'n' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 77:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'b')
          return 3;
            else if (ch == L'c')
                return 87;
        else if (ch >= L'd' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 78:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'q')
          return 3;
            else if (ch == L'r')
                return 88;
        else if (ch >= L's' && ch <= L'z')
          return 3;
        //TKlarge
        //TKlarge
    break; // 
            case 79:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'l')
          return 3;
            else if (ch == L'm')
                return 89;
        else if (ch >= L'n' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 80:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'k')
          return 3;
            else if (ch == L'l')
                return 90;
        else if (ch >= L'm' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 81:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L't')
          return 3;
            else if (ch == L'u')
                return 91;
        else if (ch >= L'v' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 82:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'd')
          return 3;
            else if (ch == L'e')
                return 92;
        else if (ch >= L'f' && ch <= L'z')
          return 3;
        //TKsmall
        //TKsmall
    break; // 
            case 83:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'f')
          return 3;
            else if (ch == L'g')
                return 93;
        else if (ch >= L'h' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 84:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'k')
          return 3;
            else if (ch == L'l')
                return 94;
        else if (ch >= L'm' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 85:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'q')
          return 3;
            else if (ch == L'r')
                return 95;
        else if (ch >= L's' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 86:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
            else if (ch == L'a')
                return 96;
        else if (ch >= L'b' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 87:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'z')
          return 3;
        //TKItalic
        //TKItalic
    break; // 
            case 88:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'z')
          return 3;
        //TKlarger
        //TKlarger
    break; // 
            case 89:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'z')
          return 3;
        //TKmedium
        //TKmedium
    break; // 
            case 90:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'z')
          return 3;
        //TKNormal
        //TKNormal
    break; // 
            case 91:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'd')
          return 3;
            else if (ch == L'e')
                return 97;
        else if (ch >= L'f' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 92:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'q')
          return 3;
            else if (ch == L'r')
                return 98;
        else if (ch >= L's' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 93:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'd')
          return 3;
            else if (ch == L'e')
                return 99;
        else if (ch >= L'f' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 94:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'k')
          return 3;
            else if (ch == L'l')
                return 100;
        else if (ch >= L'm' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 95:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'f')
          return 3;
            else if (ch == L'g')
                return 101;
        else if (ch >= L'h' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 96:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'k')
          return 3;
            else if (ch == L'l')
                return 102;
        else if (ch >= L'm' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 97:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'z')
          return 3;
        //TKOblique
        //TKOblique
    break; // 
            case 98:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'z')
          return 3;
        //TKsmaller
        //TKsmaller
    break; // 
            case 99:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'z')
          return 3;
        //TKxlarge
        //TKxlarge
    break; // 
            case 100:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'z')
          return 3;
        //TKxsmall
        //TKxsmall
    break; // 
            case 101:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'd')
          return 3;
            else if (ch == L'e')
                return 103;
        else if (ch >= L'f' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 102:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'k')
          return 3;
            else if (ch == L'l')
                return 104;
        else if (ch >= L'm' && ch <= L'z')
          return 3;
        //TKFontName
    break; // 
            case 103:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'z')
          return 3;
        //TKxxlarge
        //TKxxlarge
    break; // 
            case 104:
            if (ch == L'-')
                return 3;
        else if (ch >= L'A' && ch <= L'Z')
          return 3;
        else if (ch >= L'a' && ch <= L'z')
          return 3;
        //TKxxsmall
        //TKxxsmall
    break; // 
        } //switch
        return -1;
    }

    static bool IsInterleave(Tokens tk)
    {
        return tk == TKBLANKS;
    }

    static bool GetTokenFromState(int state, Tokens& tk)
    {
        switch(state)
        {
            case 1: tk = TKBLANKS; break;
            case 3: tk = TKFontName; break;
            case 14: tk = TKFontName; break;
            case 15: tk = TKFontName; break;
            case 16: tk = TKFontName; break;
            case 17: tk = TKFontName; break;
            case 18: tk = TKFontName; break;
            case 19: tk = TKFontName; break;
            case 20: tk = TKFontName; break;
            case 21: tk = TKFontName; break;
            case 23: tk = TKFontName; break;
            case 25: tk = TKPercent; break;
            case 36: tk = TKFontName; break;
            case 37: tk = TKFontName; break;
            case 38: tk = TKFontName; break;
            case 39: tk = TKFontName; break;
            case 40: tk = TKFontName; break;
            case 41: tk = TKFontName; break;
            case 42: tk = TKFontName; break;
            case 43: tk = TKFontName; break;
            case 44: tk = TKFontName; break;
            case 45: tk = TKPoints; break;
            case 46: tk = TKPixels; break;
            case 47: tk = TKA100; break;
            case 48: tk = TKA200; break;
            case 49: tk = TKA300; break;
            case 50: tk = TKA400; break;
            case 51: tk = TKA500; break;
            case 52: tk = TKA600; break;
            case 53: tk = TKA700; break;
            case 54: tk = TKA800; break;
            case 55: tk = TKA900; break;
            case 56: tk = TKFontName; break;
            case 57: tk = TKFontName; break;
            case 58: tk = TKFontName; break;
            case 59: tk = TKFontName; break;
            case 60: tk = TKFontName; break;
            case 61: tk = TKFontName; break;
            case 62: tk = TKFontName; break;
            case 63: tk = TKFontName; break;
            case 64: tk = TKFontName; break;
            case 65: tk = TKFontName; break;
            case 66: tk = TKBold; break;
            case 67: tk = TKFontName; break;
            case 68: tk = TKFontName; break;
            case 69: tk = TKFontName; break;
            case 70: tk = TKFontName; break;
            case 71: tk = TKFontName; break;
            case 72: tk = TKFontName; break;
            case 73: tk = TKFontName; break;
            case 74: tk = TKFontName; break;
            case 75: tk = TKFontName; break;
            case 76: tk = TKFontName; break;
            case 77: tk = TKFontName; break;
            case 78: tk = TKlarge; break;
            case 79: tk = TKFontName; break;
            case 80: tk = TKFontName; break;
            case 81: tk = TKFontName; break;
            case 82: tk = TKsmall; break;
            case 83: tk = TKFontName; break;
            case 84: tk = TKFontName; break;
            case 85: tk = TKFontName; break;
            case 86: tk = TKFontName; break;
            case 87: tk = TKItalic; break;
            case 88: tk = TKlarger; break;
            case 89: tk = TKmedium; break;
            case 90: tk = TKNormal; break;
            case 91: tk = TKFontName; break;
            case 92: tk = TKFontName; break;
            case 93: tk = TKFontName; break;
            case 94: tk = TKFontName; break;
            case 95: tk = TKFontName; break;
            case 96: tk = TKFontName; break;
            case 97: tk = TKOblique; break;
            case 98: tk = TKsmaller; break;
            case 99: tk = TKxlarge; break;
            case 100: tk = TKxsmall; break;
            case 101: tk = TKFontName; break;
            case 102: tk = TKFontName; break;
            case 103: tk = TKxxlarge; break;
            case 104: tk = TKxxsmall; break;
            default:
                return false;
        }
        return true;
    }
};


    class Context
    {
        Actions& m_Actions;
    public:
        typedef Tokenizer<FontShorthand::DFA, StringTokenizerStream<char>> Scanner;
        Scanner m_scanner;

        FontShorthand::Tokens m_CurrentToken;
        std::string m_lexeme;
        std::string m_lexeme0;

    public:
        Context(StringTokenizerStream<char>& s, Actions& actions) : m_scanner(s) , m_Actions(actions)
        {
            if (!m_scanner.NextToken(m_lexeme, m_CurrentToken))
            {
            }
        }

        //returns the current token
        FontShorthand::Tokens Token() const
        {
            return m_CurrentToken;
        }

        //checks if Token() == tk and then move the current token
        void Match(FontShorthand::Tokens tk)
        {
            if (tk != m_CurrentToken)
            {
                throw std::runtime_error("token not expected");
            }

            m_lexeme0 = m_lexeme;

            if (!m_scanner.NextToken(m_lexeme, m_CurrentToken))
            {
            }
        }

        //Actions
        void Action_SetFontName()
        {
            m_Actions.SetFontName(m_lexeme0);
        }

        void Action_SetNormalStyle()
        {
            m_Actions.SetNormalStyle();
        }
        void Action_SetItalic()
        {
            m_Actions.SetItalic();
        }
        void Action_SetOblique()
        {
            m_Actions.SetOblique();
        }
        void Action_SetNormalWeight()
        {
            m_Actions.SetNormalWeight();
        }
        void Action_SetWeightBold()
        {
            m_Actions.SetWeightBold();
        }
        void Action_SetWeight100()
        {
            m_Actions.SetWeight100();
        }
        void Action_SetWeight200()
        {
            m_Actions.SetWeight200();
        }
        void Action_SetWeight300()
        {
            m_Actions.SetWeight300();
        }
        void Action_SetWeight400()
        {
            m_Actions.SetWeight400();
        }
        void Action_SetWeight500()
        {
            m_Actions.SetWeight500();
        }
        void Action_SetWeight600()
        {
            m_Actions.SetWeight600();
        }
        void Action_SetWeight700()
        {
            m_Actions.SetWeight700();
        }
        void Action_SetWeight800()
        {
            m_Actions.SetWeight800();
        }
        void Action_SetWeight900()
        {
            m_Actions.SetWeight900();
        }
        void Action_SetSizePercent()
        {
         m_Actions.SetSizePixels(atoi(m_lexeme0.c_str()));
        }
        void Action_SetSizePoints()
        {
         m_Actions.SetSizePoints(atoi(m_lexeme0.c_str()));
        }
        void Action_SetSizePixels()
        {
           m_Actions.SetSizePixels(atoi(m_lexeme0.c_str()));
        }
        void Action_SetSizeLarger()
        {
        }
        void Action_SetSizeSmaller()
        {
        }
        void Action_SetSizexxsmall()
        {
        }
        void Action_SetSizexsmall()
        {
        }
        void Action_SetSizesmall()
        {
        }
        void Action_SetSizemedium()
        {
        }
        void Action_SetSizelarge()
        {
        }
        void Action_SetSizexlarge()
        {
        }
        void Action_SetSizexxlarge()
        {
        }
    };

    //forward declarations
    void Parse_Main(Context&);
    void Parse_FontStyleGroup(Context&);
    void Parse_FontSize(Context&);
    void Parse_FontStyle(Context&);
    void Parse_FontWeight(Context&);
    void Parse_RelativeSize(Context&);
    void Parse_AbsoluteSize(Context&);


    void Parse_Main(Context& ctx)
    {
        if (ctx.Token() == FontShorthand::TKNormal ||
                ctx.Token() == FontShorthand::TKItalic ||
                ctx.Token() == FontShorthand::TKOblique ||
                ctx.Token() == FontShorthand::TKBold ||
                ctx.Token() == FontShorthand::TKxxsmall ||
                ctx.Token() == FontShorthand::TKlarge ||
                ctx.Token() == FontShorthand::TKxsmall ||
                ctx.Token() == FontShorthand::TKsmall ||
                ctx.Token() == FontShorthand::TKmedium ||
                ctx.Token() == FontShorthand::TKxlarge ||
                ctx.Token() == FontShorthand::TKxxlarge ||
                ctx.Token() == FontShorthand::TKlarger ||
                ctx.Token() == FontShorthand::TKsmaller ||
                ctx.Token() == FontShorthand::TKA100 ||
                ctx.Token() == FontShorthand::TKA200 ||
                ctx.Token() == FontShorthand::TKA300 ||
                ctx.Token() == FontShorthand::TKA400 ||
                ctx.Token() == FontShorthand::TKA500 ||
                ctx.Token() == FontShorthand::TKA600 ||
                ctx.Token() == FontShorthand::TKA700 ||
                ctx.Token() == FontShorthand::TKA800 ||
                ctx.Token() == FontShorthand::TKA900 ||
                ctx.Token() == FontShorthand::TKPercent ||
                ctx.Token() == FontShorthand::TKPoints ||
                ctx.Token() == FontShorthand::TKPixels)
        {
            //Main => FontStyleGroup FontSize FontName
            Parse_FontStyleGroup(ctx);
            Parse_FontSize(ctx);
            ctx.Match(FontShorthand::TKFontName);
            ctx.Action_SetFontName();
        }
        else
        {
            assert(false);
        }
    }

    void Parse_FontStyleGroup(Context& ctx)
    {
        if (ctx.Token() == FontShorthand::TKNormal ||
                ctx.Token() == FontShorthand::TKItalic ||
                ctx.Token() == FontShorthand::TKOblique ||
                ctx.Token() == FontShorthand::TKBold ||
                ctx.Token() == FontShorthand::TKxxsmall ||
                ctx.Token() == FontShorthand::TKlarge ||
                ctx.Token() == FontShorthand::TKxsmall ||
                ctx.Token() == FontShorthand::TKsmall ||
                ctx.Token() == FontShorthand::TKmedium ||
                ctx.Token() == FontShorthand::TKxlarge ||
                ctx.Token() == FontShorthand::TKxxlarge ||
                ctx.Token() == FontShorthand::TKlarger ||
                ctx.Token() == FontShorthand::TKsmaller ||
                ctx.Token() == FontShorthand::TKA100 ||
                ctx.Token() == FontShorthand::TKA200 ||
                ctx.Token() == FontShorthand::TKA300 ||
                ctx.Token() == FontShorthand::TKA400 ||
                ctx.Token() == FontShorthand::TKA500 ||
                ctx.Token() == FontShorthand::TKA600 ||
                ctx.Token() == FontShorthand::TKA700 ||
                ctx.Token() == FontShorthand::TKA800 ||
                ctx.Token() == FontShorthand::TKA900 ||
                ctx.Token() == FontShorthand::TKPercent ||
                ctx.Token() == FontShorthand::TKPoints ||
                ctx.Token() == FontShorthand::TKPixels)
        {
            //FontStyleGroup => FontStyle FontWeight
            Parse_FontStyle(ctx);
            Parse_FontWeight(ctx);
        }
        else
        {
            assert(false);
        }
    }

    void Parse_FontSize(Context& ctx)
    {
        if (ctx.Token() == FontShorthand::TKlarger ||
                ctx.Token() == FontShorthand::TKsmaller)
        {
            //FontSize => RelativeSize
            Parse_RelativeSize(ctx);
        }
        else if (ctx.Token() == FontShorthand::TKxxsmall ||
                 ctx.Token() == FontShorthand::TKlarge ||
                 ctx.Token() == FontShorthand::TKxsmall ||
                 ctx.Token() == FontShorthand::TKsmall ||
                 ctx.Token() == FontShorthand::TKmedium ||
                 ctx.Token() == FontShorthand::TKxlarge ||
                 ctx.Token() == FontShorthand::TKxxlarge)
        {
            //FontSize => AbsoluteSize
            Parse_AbsoluteSize(ctx);
        }
        else if (ctx.Token() == FontShorthand::TKPercent)
        {
            //FontSize => Percent
            ctx.Match(FontShorthand::TKPercent);
            ctx.Action_SetSizePercent();
        }
        else if (ctx.Token() == FontShorthand::TKPoints)
        {
            //FontSize => Points
            ctx.Match(FontShorthand::TKPoints);
            ctx.Action_SetSizePoints();
        }
        else if (ctx.Token() == FontShorthand::TKPixels)
        {
            //FontSize => Pixels
            ctx.Match(FontShorthand::TKPixels);
            ctx.Action_SetSizePixels();
        }
        else
        {
            assert(false);
        }
    }

    void Parse_FontStyle(Context& ctx)
    {
        if (ctx.Token() == FontShorthand::TKNormal)
        {
            //FontStyle => Normal
            ctx.Match(FontShorthand::TKNormal);
            ctx.Action_SetNormalStyle();
        }
        else if (ctx.Token() == FontShorthand::TKItalic)
        {
            //FontStyle => Italic
            ctx.Match(FontShorthand::TKItalic);
            ctx.Action_SetItalic();
        }
        else if (ctx.Token() == FontShorthand::TKOblique)
        {
            //FontStyle => Oblique
            ctx.Match(FontShorthand::TKOblique);
            ctx.Action_SetOblique();
        }
        else if (ctx.Token() == FontShorthand::TKNormal ||
                 ctx.Token() == FontShorthand::TKBold ||
                 ctx.Token() == FontShorthand::TKxxsmall ||
                 ctx.Token() == FontShorthand::TKlarge ||
                 ctx.Token() == FontShorthand::TKxsmall ||
                 ctx.Token() == FontShorthand::TKsmall ||
                 ctx.Token() == FontShorthand::TKmedium ||
                 ctx.Token() == FontShorthand::TKxlarge ||
                 ctx.Token() == FontShorthand::TKxxlarge ||
                 ctx.Token() == FontShorthand::TKlarger ||
                 ctx.Token() == FontShorthand::TKsmaller ||
                 ctx.Token() == FontShorthand::TKA100 ||
                 ctx.Token() == FontShorthand::TKA200 ||
                 ctx.Token() == FontShorthand::TKA300 ||
                 ctx.Token() == FontShorthand::TKA400 ||
                 ctx.Token() == FontShorthand::TKA500 ||
                 ctx.Token() == FontShorthand::TKA600 ||
                 ctx.Token() == FontShorthand::TKA700 ||
                 ctx.Token() == FontShorthand::TKA800 ||
                 ctx.Token() == FontShorthand::TKA900 ||
                 ctx.Token() == FontShorthand::TKPercent ||
                 ctx.Token() == FontShorthand::TKPoints ||
                 ctx.Token() == FontShorthand::TKPixels)
        {
            //FontStyle => Epsilon
            ctx.Action_SetNormalStyle();
        }
        else
        {
            assert(false);
        }
    }

    void Parse_FontWeight(Context& ctx)
    {
        if (ctx.Token() == FontShorthand::TKNormal)
        {
            //FontWeight => Normal
            ctx.Match(FontShorthand::TKNormal);
            ctx.Action_SetNormalWeight();
        }
        else if (ctx.Token() == FontShorthand::TKBold)
        {
            //FontWeight => Bold
            ctx.Match(FontShorthand::TKBold);
            ctx.Action_SetWeightBold();
        }
        else if (ctx.Token() == FontShorthand::TKA100)
        {
            //FontWeight => A100
            ctx.Match(FontShorthand::TKA100);
            ctx.Action_SetWeight100();
        }
        else if (ctx.Token() == FontShorthand::TKA200)
        {
            //FontWeight => A200
            ctx.Match(FontShorthand::TKA200);
            ctx.Action_SetWeight200();
        }
        else if (ctx.Token() == FontShorthand::TKA300)
        {
            //FontWeight => A300
            ctx.Match(FontShorthand::TKA300);
            ctx.Action_SetWeight300();
        }
        else if (ctx.Token() == FontShorthand::TKA400)
        {
            //FontWeight => A400
            ctx.Match(FontShorthand::TKA400);
            ctx.Action_SetWeight400();
        }
        else if (ctx.Token() == FontShorthand::TKA500)
        {
            //FontWeight => A500
            ctx.Match(FontShorthand::TKA500);
            ctx.Action_SetWeight500();
        }
        else if (ctx.Token() == FontShorthand::TKA600)
        {
            //FontWeight => A600
            ctx.Match(FontShorthand::TKA600);
            ctx.Action_SetWeight600();
        }
        else if (ctx.Token() == FontShorthand::TKA700)
        {
            //FontWeight => A700
            ctx.Match(FontShorthand::TKA700);
            ctx.Action_SetWeight700();
        }
        else if (ctx.Token() == FontShorthand::TKA800)
        {
            //FontWeight => A800
            ctx.Match(FontShorthand::TKA800);
            ctx.Action_SetWeight800();
        }
        else if (ctx.Token() == FontShorthand::TKA900)
        {
            //FontWeight => A900
            ctx.Match(FontShorthand::TKA900);
            ctx.Action_SetWeight900();
        }
        else if (ctx.Token() == FontShorthand::TKxxsmall ||
                 ctx.Token() == FontShorthand::TKlarge ||
                 ctx.Token() == FontShorthand::TKxsmall ||
                 ctx.Token() == FontShorthand::TKsmall ||
                 ctx.Token() == FontShorthand::TKmedium ||
                 ctx.Token() == FontShorthand::TKxlarge ||
                 ctx.Token() == FontShorthand::TKxxlarge ||
                 ctx.Token() == FontShorthand::TKlarger ||
                 ctx.Token() == FontShorthand::TKsmaller ||
                 ctx.Token() == FontShorthand::TKPercent ||
                 ctx.Token() == FontShorthand::TKPoints ||
                 ctx.Token() == FontShorthand::TKPixels)
        {
            //FontWeight => Epsilon
            ctx.Action_SetNormalWeight();
        }
        else
        {
            assert(false);
        }
    }

    void Parse_RelativeSize(Context& ctx)
    {
        if (ctx.Token() == FontShorthand::TKlarger)
        {
            //RelativeSize => larger
            ctx.Match(FontShorthand::TKlarger);
            ctx.Action_SetSizeLarger();
        }
        else if (ctx.Token() == FontShorthand::TKsmaller)
        {
            //RelativeSize => smaller
            ctx.Match(FontShorthand::TKsmaller);
            ctx.Action_SetSizeSmaller();
        }
        else
        {
            assert(false);
        }
    }

    void Parse_AbsoluteSize(Context& ctx)
    {
        if (ctx.Token() == FontShorthand::TKxxsmall)
        {
            //AbsoluteSize => xxsmall
            ctx.Match(FontShorthand::TKxxsmall);
            ctx.Action_SetSizexxsmall();
        }
        else if (ctx.Token() == FontShorthand::TKxsmall)
        {
            //AbsoluteSize => xsmall
            ctx.Match(FontShorthand::TKxsmall);
            ctx.Action_SetSizexsmall();
        }
        else if (ctx.Token() == FontShorthand::TKsmall)
        {
            //AbsoluteSize => small
            ctx.Match(FontShorthand::TKsmall);
            ctx.Action_SetSizesmall();
        }
        else if (ctx.Token() == FontShorthand::TKmedium)
        {
            //AbsoluteSize => medium
            ctx.Match(FontShorthand::TKmedium);
            ctx.Action_SetSizemedium();
        }
        else if (ctx.Token() == FontShorthand::TKlarge)
        {
            //AbsoluteSize => large
            ctx.Match(FontShorthand::TKlarge);
            ctx.Action_SetSizelarge();
        }
        else if (ctx.Token() == FontShorthand::TKxlarge)
        {
            //AbsoluteSize => xlarge
            ctx.Match(FontShorthand::TKxlarge);
            ctx.Action_SetSizexlarge();
        }
        else if (ctx.Token() == FontShorthand::TKxxlarge)
        {
            //AbsoluteSize => xxlarge
            ctx.Match(FontShorthand::TKxxlarge);
            ctx.Action_SetSizexxlarge();
        }
        else
        {
            assert(false);
        }
    }

} //namespace FontShorthand

/*
//LL(1) Grammar
module FontShorthand
{
    language FontShorthand
    {
        interleave BLANKS = ' '+;

        token Normal = "normal";
        token Italic = "italic";
        token Oblique = "oblique";
        token Bold = "bold";
        token xxsmall = "xx-small";
        token xsmall = "x-small";
        token small = "small";
        token medium  = "medium";
        token large = "large";
        token xlarge = "x-large";
        token xxlarge = "xx-large";

        token larger = "larger";
        token smaller = "smaller";

        token A100 = "100";
        token A200 = "200";
        token A300 = "300";
        token A400 = "400";
        token A500 = "500";
        token A600 = "600";
        token A700 = "700";
        token A800 = "800";
        token A900 = "900";
        token Percent = ('0'..'9')+ "%";
        token Points = ('0'..'9')+ "pt";
        token Pixels = ('0'..'9')+ "px";
        token FontName = ('A'..'Z' | 'a'..'z' | '-' )+;


    syntax Main = FontStyleGroup FontSize FontName {SetFontName};

    syntax FontStyleGroup = FontStyle FontWeight;
    syntax FontStyle = "normal" {SetNormalStyle} | "italic" {SetItalic} | "oblique" {SetOblique} | empty {SetNormalStyle};
    syntax FontWeight = "normal" {SetNormalWeight}
                        | "bold" {SetWeightBold}
                        | "100" {SetWeight100}
                        | "200" {SetWeight200}
                        | "300" {SetWeight300}
                        | "400" {SetWeight400}
                        | "500" {SetWeight500}
                        | "600" {SetWeight600}
                        | "700" {SetWeight700}
                        | "800" {SetWeight800}
                        | "900" {SetWeight900}
                        | empty {SetNormalWeight};


    syntax FontSize = RelativeSize | AbsoluteSize | Percent {SetSizePercent}| Points {SetSizePoints}| Pixels {SetSizePixels};
    syntax RelativeSize = "larger" {SetSizeLarger}| "smaller" {SetSizeSmaller};
    syntax AbsoluteSize = "xx-small" {SetSizexxsmall}|
                          "x-small" {SetSizexsmall}|
                          "small" {SetSizesmall}|
                          "medium" {SetSizemedium}|
                          "large" {SetSizelarge}|
                          "x-large" {SetSizexlarge}|
                          "xx-large" {SetSizexxlarge};


    }
}
*/

void ParseFont(const std::string& str, Actions& actions)
{
 try
 {
    FontShorthand::StringTokenizerStream<char> ss(str);
    FontShorthand::Context ctx(ss, actions);
    FontShorthand::Parse_Main(ctx);
 }
 catch(...)
 {
   //ops
 }
}

