
#pragma once
#include <stdext/stdext.h>
#include <sstream>
#include <string>
#include <locale>
#include <vector>
#include <algorithm>

namespace stdext
{
    namespace string
    {
        //! poor man's lexical_cast
        template<typename OutType, typename InType>
        OutType ConvertTo(const InType& in)
        {
            OutType out;
            std::stringstream stream;
            static auto fail = [](const char* msg)
            {
                throw std::runtime_error(msg);
            };
            if(!(stream << in))
            {
                fail("input type cannot be used with std::stringstream");
            }
            if(!(stream >> out))
            {
                fail("output type not supported by std::stringstream");
            }
            return out;
        }

        //! Concat() can be used to, you guessed it, concatenate things.
        //! that is, anything that can be used with std::stringstream.
        template<typename StreamType>
        inline void concatnext(StreamType&)
        {
        }

        template<typename StreamType, typename First, typename... Rest>
        inline void concatnext(StreamType& out, const First& value, const Rest&... rest)
        {
            out << value;
            concatnext(out, rest...);
        }

        template<typename... Args>
        inline std::string concat(const Args&... args)
        {
            std::stringstream out;
            concatnext(out, args...);
            return out.str();
        }

        //! convert $ch to uppercase.
        template<typename CharT>
        CharT toupper(CharT ch)
        {
            return std::use_facet<std::ctype<CharT>>(std::locale()).toupper(ch);
        }

        //! convert $ch to lowercase.
        template<typename CharT>
        CharT tolower(CharT ch)
        {
            return std::use_facet<std::ctype<CharT>>(std::locale()).tolower(ch);
        }

        //! convert string $src to uppercase.
        template<typename CharT>
        std::basic_string<CharT> toupper(const std::basic_string<CharT>& src)
        {
            std::basic_string<CharT> result;
            std::transform(src.begin(), src.end(), std::back_inserter(result), toupper<CharT>);
            return result;
        }

        //! convert string $src to lowercase.
        template<typename CharT>
        std::basic_string<CharT> tolower(const std::basic_string<CharT>& src)
        {
            std::basic_string<CharT> result;
            std::transform(src.begin(), src.end(), std::back_inserter(result), tolower<CharT>);
            return result;
        }

        template<typename CharT>
        bool startswith(const std::basic_string<CharT>& inp, const std::basic_string<CharT>& search)
        {
            size_t srchsz = search.size();
            return (0 == inp.compare(0, srchsz, search));
        }

        template<typename CharT>
        bool endswith(const std::basic_string<CharT>& inp, const std::basic_string<CharT>& search)
        {
            if(inp.size() >= search.size())
            {
                return (0 == inp.compare(
                    inp.size() - search.size(),
                    search.size(),
                    search));
            }
            return false;
        }

        template<typename CharT>
        bool startswith(const std::basic_string<CharT>& inp, CharT what)
        {
            if(inp.size() > 0)
            {
                return (inp.at(0) == what);
            }
            return false;
        }

        template<typename CharT>
        bool endswith(const std::basic_string<CharT>& inp, CharT what)
        {
            const size_t sz = inp.size();
            if(sz > 0)
            {
                return (inp.at(sz - 1) == what);
            }
            return false;
        }

        //! reverse $str inplace. binary-safe.
        template<typename CharT>
        inline std::basic_string<CharT>& reverse(std::basic_string<CharT>& str)
        {
            std::reverse(str.begin(), str.end());
            return str;
        }

        template<typename CharT>
        std::basic_string<CharT>& replace(std::basic_string<CharT>& str, CharT findme, CharT repl)
        {
            std::replace(str.begin(), str.end(), findme, repl);
            return str;
        }

        template<typename CharT>
        std::basic_string<CharT>& replace(
            std::basic_string<CharT>& inp,
            const std::basic_string<CharT>& srch,
            const std::basic_string<CharT>& repl)
        {
            size_t pos = 0;
            static const auto npos = std::basic_string<CharT>::npos;
            if(inp.find(srch) != npos)
            {
                pos = 0;
                while((pos = inp.find(srch, pos)) != npos)
                {
                    inp.replace(pos, srch.size(), repl);
                    pos++;
                }
            }
            return inp;
        }

        template<typename CharT>
        std::vector<std::basic_string<CharT> > split(
            const std::basic_string<CharT>& inp,
            const std::basic_string<CharT>& delim,
            bool keep_empty = false)
        {
            std::vector<std::basic_string<CharT> > vals;
            if(delim.empty())
            {
                return vals;
            }
            auto substart = inp.begin();
            while(true)
            {
                auto subend = std::search(substart, inp.end(), delim.begin(), delim.end());
                std::string temp(substart, subend);
                if(keep_empty || !temp.empty())
                {
                    vals.push_back(temp);
                }
                if(subend == inp.end())
                {
                    break;
                }
                substart = (subend + delim.size());
            }
            return vals;
        }

        class formatter
        {
            private:
                std::string m_result;
                size_t m_currentindex = 0;

            public:
                formatter()
                {
                }

                formatter(const std::string& fmt): m_result(fmt)
                {
                }

                template<typename... Args>
                formatter(const std::string& fmt, Args&&... valargs): m_result(fmt)
                {
                    fmtVariadic(valargs...);
                }

                template<typename Type>
                formatter& arg(size_t idx, const Type& val)
                {
                    auto idxpat = string::concat('%', idx);
                    auto repl = string::concat(val);
                    string::replace(m_result, idxpat, repl);
                    m_currentindex = idx;
                    return *this;
                }

                template<typename Type>
                formatter& arg(const Type& val)
                {
                    arg(m_currentindex+1, val);
                    return *this;
                }

                template<typename... Args>
                formatter& args(Args&&... valargs)
                {
                    // this is only necessary, becaue GCC doesn't seem
                    // to understand `(arg(valargs)...)` (i.e., without a function call)
                    // maybe fix this some day?
                    force_cast<void*>(arg(valargs)...);
                    return *this;
                }

                void fmtVariadicNext(const size_t)
                {
                }

                template<typename Type, typename... Args>
                void fmtVariadicNext(
                    const size_t curidx,
                    Type&& val,
                    Args&&... valargs)
                {
                    arg(curidx, val);
                    fmtVariadicNext(
                        curidx + 1,
                        std::forward<Args>(valargs)...
                    );
                }

                template<typename... Args>
                void fmtVariadic(Args&&... valargs)
                {
                    fmtVariadicNext(1, std::forward<Args>(valargs)...);
                }

                std::string str() const
                {
                    return this->m_result;
                }
        };

        template<typename... Args>
        std::string format(const std::string& fmt, Args&&... args)
        {
            return formatter(fmt, args...).str();
        }
    }
}
