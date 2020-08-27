#include "stdafx.h"


#include "SimpleJSON.hpp"

#include <cstdint>
#include <cmath>
#include <cctype>
#include <string>
#include <deque>
#include <map>
#include <type_traits>
#include <initializer_list>
#include <ostream>
#include <iostream>
#include <sstream>

namespace enscape
{
    using std::map;
    using std::deque;
    using std::string;
    using std::enable_if;
    using std::initializer_list;
    using std::is_same;
    using std::is_convertible;
    using std::is_integral;
    using std::is_floating_point;

    namespace
    {
        string json_escape( const string &str )
        {
            string output;
            for (unsigned i = 0; i < str.length(); ++i)
            {
                switch (str[i])
                {
                case '\"': output += "\\\""; break;
                case '\\': output += "\\\\"; break;
                case '\b': output += "\\b";  break;
                case '\f': output += "\\f";  break;
                case '\n': output += "\\n";  break;
                case '\r': output += "\\r";  break;
                case '\t': output += "\\t";  break;
                default: output += str[i]; break;
                }
            }
            return std::move( output );
        }
    }

    EJSON::EJSON() : Internal(), Type(Class::Null) {}

    EJSON::EJSON(std::initializer_list<EJSON> list)
        : EJSON() 
    {
        SetType( Class::Object );
        for( auto i = list.begin(), e = list.end(); i != e; ++i, ++i )
            operator[]( i->ToString() ) = *std::next( i );
    }

    EJSON::EJSON(EJSON&& other)
        : Internal( other.Internal )
        , Type( other.Type )
    {
        other.Type = Class::Null;
        other.Internal.List     = nullptr;
        other.Internal.Map      = nullptr;
        other.Internal.Key      = nullptr;
        other.Internal.String   = nullptr;
    }

    EJSON& EJSON::operator=(EJSON&& other)
    {
        ClearInternal();
        Internal    = other.Internal;
        Type        = other.Type;
        other.Type              = Class::Null;
        other.Internal.List     = nullptr;
        other.Internal.Map      = nullptr;
        other.Internal.Key      = nullptr;
        other.Internal.String   = nullptr;
        return *this;
    }

    EJSON::EJSON(const EJSON &other)
    {
        switch( other.Type )
        {
        case Class::Object:
            Internal.Map = 
                new map<string,EJSON>( other.Internal.Map->begin(),
                                        other.Internal.Map->end() );
            Internal.Key = 
                new deque<string>( other.Internal.Key->begin(),
                                    other.Internal.Key->end() );
            break;
        case Class::Array:
            Internal.List = 
                new deque<EJSON>( other.Internal.List->begin(),
                                    other.Internal.List->end() );
            break;
        case Class::String:
            Internal.String = 
                new string( *other.Internal.String );
            break;
        default:
            Internal = other.Internal;
        }
        Type = other.Type;
    }

    EJSON& EJSON::operator=(const EJSON &other)
    {
        ClearInternal();
        switch( other.Type )
        {
        case Class::Object:
            Internal.Map = new map<string,EJSON>( other.Internal.Map->begin(),
                                                    other.Internal.Map->end() );
            Internal.Key = new deque<string>( other.Internal.Key->begin(),
                                                other.Internal.Key->end() );
            break;
        case Class::Array:
            Internal.List = new deque<EJSON>( other.Internal.List->begin(),
                                                other.Internal.List->end() );
            break;
        case Class::String:
            Internal.String = 
                new string( *other.Internal.String );
            break;
        default:
            Internal = other.Internal;
        }
        Type = other.Type;
        return *this;
    }

    EJSON::~EJSON()
    {
        switch( Type )
        {
        case Class::Array:
            delete Internal.List;
            break;
        case Class::Object:
            delete Internal.Map;
            delete Internal.Key;
            break;
        case Class::String:
            delete Internal.String;
            break;
        default:;
        }
    }

    EJSON::EJSON(std::nullptr_t) : Internal(), Type(Class::Null){}

    EJSON EJSON::Make(Class type)
    {
        EJSON ret; ret.SetType( type );
        return ret;
    }

    EJSON EJSON::Load(std::istream & in)
    {
        std::stringstream ss;
        ss << in.rdbuf();
        std::string str = ss.str();

        return EJSON::Load(str);
    }

    EJSON& EJSON::operator[](const string &key)
    {
        SetType( Class::Object );
        if (std::find(Internal.Key->begin(), Internal.Key->end(), key) == Internal.Key->end())
            Internal.Key->push_back(key);
        return Internal.Map->operator[]( key );
    }

    EJSON& EJSON::operator[](unsigned index)
    {
        SetType( Class::Array );
        if( index >= Internal.List->size() ) Internal.List->resize( index + 1 );
        return Internal.List->operator[]( index );
    }

    EJSON& EJSON::at(const string &key)
    {
        return operator[]( key );
    }

    const EJSON& EJSON::at(const string &key) const
    {
        return Internal.Map->at( key );
    }

    EJSON& EJSON::at(unsigned index)
    {
        return operator[]( index );
    }

    const EJSON& EJSON::at(unsigned index) const
    {
        return Internal.List->at( index );
    }

    void EJSON::remove(const string& key)
    {
        Internal.Map->erase(key);
        deque<string>::iterator it = Internal.Key->begin();
        deque<string>::iterator end = Internal.Key->end();
        for (; it != end; ++it)
        {
            if (key == *it)
            {
                Internal.Key->erase(it);
                break;
            }
        }
    }

    int EJSON::length() const
    {
        if( Type == Class::Array )
            return Internal.List->size();
        else
            return -1;
    }

    bool EJSON::hasKey(const string &key) const
    {
        if( Type == Class::Object )
            return Internal.Map->find( key ) != Internal.Map->end();
        return false;
    }

    int EJSON::size() const
    {
        if( Type == Class::Object )
            return Internal.Map->size();
        else if( Type == Class::Array )
            return Internal.List->size();
        else
            return -1;
    }

    string EJSON::ToString(bool &ok) const
    {
        ok = (Type == Class::String);
        return ok ? std::move( json_escape( *Internal.String ) ): string("");
    }

    double EJSON::ToFloat(bool &ok) const
    {
        ok = (Type == Class::Floating);
        return ok ? Internal.Float : 0.0;
    }

    long EJSON::ToInt(bool &ok) const
    {
        ok = (Type == Class::Integral);
        return ok ? Internal.Int : 0;
    }

    bool EJSON::ToBool(bool &ok) const
    {
        ok = (Type == Class::Boolean);
        return ok ? Internal.Bool : false;
    }

    EJSON::JSONWrapper<map<string, EJSON>> EJSON::ObjectRange()
    {
        if( Type == Class::Object )
            return JSONWrapper<map<string,EJSON>>( Internal.Map );
        return JSONWrapper<map<string,EJSON>>( nullptr );
    }

    EJSON::JSONWrapper<deque<EJSON>> EJSON::ArrayRange()
    {
        if( Type == Class::Array )
            return JSONWrapper<deque<EJSON>>( Internal.List );
        return JSONWrapper<deque<EJSON>>( nullptr );
    }

    EJSON::JSONConstWrapper<map<string, EJSON>> EJSON::ObjectRange() const
    {
        if( Type == Class::Object )
            return JSONConstWrapper<map<string,EJSON>>( Internal.Map );
        return JSONConstWrapper<map<string,EJSON>>( nullptr );
    }

    EJSON::JSONConstWrapper<deque<EJSON>> EJSON::ArrayRange() const
    { 
        if( Type == Class::Array )
            return JSONConstWrapper<deque<EJSON>>( Internal.List );
        return JSONConstWrapper<deque<EJSON>>( nullptr );
    }

    string EJSON::dump(int depth) const
    {
        // 오브젝트는 시작/끝 중괄호는 depth-1 만큼 들여쓰고, 내부는 depth만큼 들여쓴다
        string pad = "";
        string pad_inner = "";
        for (int i = 0; i < depth-1; ++i, pad += TAB);
        pad_inner = pad + TAB;

        switch( Type )
        {
            case Class::Null:
                return "null";

            case Class::Object:
            {
                string s = "{" + NEWLINE;
                bool skip = true;
                for (const string& key : *Internal.Key)
                {
                    auto& it = Internal.Map->find(key);
                    if (it == Internal.Map->end()) continue;
                    auto& p = *it;
                    bool SecondIsObject = (p.second.Type == Class::Object); // 태그명 뒷부분이 오브젝트인가 아닌가
                    if (!skip)
                    {
                        s += ("," + NEWLINE);
                        if (SecondIsObject && spacing_option_1) // 태그명 다음에 중괄호가 시작되는 경우에는 태그명 앞에 개행을 추가
                            s += NEWLINE;
                    }
                    s += (pad_inner + "\"" + p.first + "\"" + SPACE + ":" + SPACE); // 태그명 처리
                    if (SecondIsObject && spacing_option_2) // 태그명 다음에 중괄호가 시작되는 경우에는 중괄호 앞에 개행을 추가
                    {
                        s += NEWLINE;
                        s += (TAB + pad);
                    }

                    s += p.second.dump(depth + 1); // 태그명 뒷 부분 처리
                    skip = false;
                }
                s += (NEWLINE + pad + "}");
                return s;
            }

            case Class::Array:
            {
                string s = "";
                bool skip = true;
                bool ItemIsObject = (Internal.List->size()>0 && Internal.List->at(0).Type == Class::Object);

                // 리스트 시작부분 처리 (패딩, 개행)
                if (ItemIsObject)
                {
                    if (spacing_option_3)
                        s += (NEWLINE + pad);
                    s += ("[" + NEWLINE + pad_inner);
                }
                else {
                    s += ("[" + SPACE);
                }

                for( auto &p : *Internal.List )
                {
                    if (!skip)
                    {
                        // 리스트 내부 아이템 사이 처리 (컴마, 개행)
                        s += ",";
                        if (p.Type == Class::Object)    s += (NEWLINE + pad_inner);
                        else                            s += (SPACE);
                    }

                    s += p.dump( depth + 1 );
                    skip = false;
                }

                // 리스트 끝 처리 (개행, 패딩))
                if (ItemIsObject)   s += (NEWLINE + pad + "]");
                else                s += (SPACE+"]");
                return s;
            }
            case Class::String:
                return "\"" + json_escape( *Internal.String ) + "\"";
            case Class::Floating:
            {
                std::string str = std::to_string(Internal.Float);
                str.erase(str.find_last_not_of('0') + 1, std::string::npos);
                if (str.find_last_of(".")) str += "0";
                return str;
            }
            case Class::Integral:
                return std::to_string( Internal.Int );
            case Class::Boolean:
                return Internal.Bool ? "true" : "false";
            default:
                return "";
        }
        return "";
    }

    void EJSON::SetType(Class type)
    {
        if( type == Type )
            return;

        ClearInternal();
          
        switch( type )
        {
        case Class::Null:      Internal.Map    = nullptr;                break;
        case Class::Object:    Internal.Map    = new map<string,EJSON>();
                                Internal.Key    = new deque<string>();    break;
        case Class::Array:     Internal.List   = new deque<EJSON>();      break;
        case Class::String:    Internal.String = new string();           break;
        case Class::Floating:  Internal.Float  = 0.0;                    break;
        case Class::Integral:  Internal.Int    = 0;                      break;
        case Class::Boolean:   Internal.Bool   = false;                  break;
        }

        Type = type;
    }

    void EJSON::SetOutputFormat(int level)
    {
        switch (level)
        {
        default:
        case 0: // normal
            TAB              = "    ";
            SPACE            = " ";
            NEWLINE          = "\n";
            spacing_option_1 = true;
            spacing_option_2 = true;
            spacing_option_3 = true;
            break;
        case 1: // condensed
            TAB              = "    ";
            SPACE            = " ";
            NEWLINE          = "\n";
            spacing_option_1 = false;
            spacing_option_2 = false;
            spacing_option_3 = false;
            break;
        case 2: // shortest
            TAB              = "";
            SPACE            = "";
            NEWLINE          = "";
            spacing_option_1 = false;
            spacing_option_2 = false;
            spacing_option_3 = false;
            break;
        }
    }

    void EJSON::ClearInternal()
    {
        switch( Type )
        {
            case Class::Object: delete Internal.Map;
                                delete Internal.Key;    break;
            case Class::Array:  delete Internal.List;   break;
            case Class::String: delete Internal.String; break;
            default:;
        }
    }

    std::string EJSON::TAB = "    ";
    std::string EJSON::SPACE = " ";
    std::string EJSON::NEWLINE = "\n";
    bool EJSON::spacing_option_1 = false;
    bool EJSON::spacing_option_2 = false;
    bool EJSON::spacing_option_3 = false;


    EJSON Array()
    {
        return std::move( EJSON::Make( EJSON::Class::Array ) );
    }

    EJSON Object()
    {
        return std::move( EJSON::Make( EJSON::Class::Object ) );
    }

    std::ostream& operator<<( std::ostream &os, const EJSON &enscape )
    {
        os << enscape.dump();
        return os;
    }

    // parsing utilities
    namespace
    {
        EJSON parse_next( const string &, size_t & );

        void consume_ws( const string &str, size_t &offset )
        {
            while( isspace( str[offset] ) ) ++offset;
        }

        EJSON parse_object( const string &str, size_t &offset )
        {
            EJSON Object = EJSON::Make( EJSON::Class::Object );

            ++offset;
            consume_ws( str, offset );
            if( str[offset] == '}' )
            {
                ++offset; return std::move( Object );
            }

            while( true )
            {
                EJSON _Key = parse_next( str, offset );
                consume_ws( str, offset );
                if( str[offset] != ':' )
                {
                    std::cerr << "Error: Object: Expected colon, found '" << str[offset] << "'\n";
                    break;
                }
                consume_ws( str, ++offset );
                EJSON Value = parse_next( str, offset );
                Object[_Key.ToString()] = Value;
            
                consume_ws( str, offset );
                if( str[offset] == ',' )
                {
                    ++offset; continue;
                }
                else if( str[offset] == '}' )
                {
                    ++offset; break;
                }
                else
                {
                    std::cerr << "ERROR: Object: Expected comma, found '" << str[offset] << "'\n";
                    break;
                }
            }

            return std::move( Object );
        }

        EJSON parse_array( const string &str, size_t &offset )
        {
            EJSON Array = EJSON::Make( EJSON::Class::Array );
            unsigned index = 0;
        
            ++offset;
            consume_ws( str, offset );
            if( str[offset] == ']' )
            {
                ++offset; return std::move( Array );
            }

            while( true )
            {
                Array[index++] = parse_next( str, offset );
                consume_ws( str, offset );

                if( str[offset] == ',' )
                {
                    ++offset; continue;
                }
                else if( str[offset] == ']' )
                {
                    ++offset; break;
                }
                else
                {
                    std::cerr << "ERROR: Array: Expected ',' or ']', found '" << str[offset] << "'\n";
                    return std::move( EJSON::Make( EJSON::Class::Array ) );
                }
            }

            return std::move( Array );
        }

        EJSON parse_string( const string &str, size_t &offset )
        {
            EJSON String;
            string val;
            for( char c = str[++offset]; c != '\"' ; c = str[++offset] )
            {
                if( c == '\\' )
                {
                    switch( str[ ++offset ] )
                    {
                    case '\"': val += '\"'; break;
                    case '\\': val += '\\'; break;
                    case '/' : val += '/' ; break;
                    case 'b' : val += '\b'; break;
                    case 'f' : val += '\f'; break;
                    case 'n' : val += '\n'; break;
                    case 'r' : val += '\r'; break;
                    case 't' : val += '\t'; break;
                    case 'u' :
                    {
                        val += "\\u" ;
                        for( unsigned i = 1; i <= 4; ++i )
                        {
                            c = str[offset+i];
                            if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
                            {
                                val += c;
                            }
                            else
                            {
                                std::cerr << "ERROR: String: Expected hex character in unicode escape, found '" << c << "'\n";
                                return std::move( EJSON::Make( EJSON::Class::String ) );
                            }
                        }
                        offset += 4;
                    } break;
                    default  : val += '\\'; break;
                    }
                }
                else
                    val += c;
            }
            ++offset;
            String = val;
            return std::move( String );
        }

        EJSON parse_number( const string &str, size_t &offset )
        {
            EJSON Number;
            string val, exp_str;
            char c;
            bool isDouble = false;
            long exp = 0;
            while( true )
            {
                c = str[offset++];
                if ((c == '-') || (c >= '0' && c <= '9'))
                {
                    val += c;
                }
                else if( c == '.' )
                {
                    val += c; 
                    isDouble = true;
                }
                else
                    break;
            }
            if( c == 'E' || c == 'e' )
            {
                c = str[ offset++ ];
                if( c == '-' ) { ++offset; exp_str += '-';}
                while( true )
                {
                    c = str[ offset++ ];
                    if (c >= '0' && c <= '9')
                    {
                        exp_str += c;
                    }
                    else if( !isspace( c ) && c != ',' && c != ']' && c != '}' )
                    {
                        std::cerr << "ERROR: Number: Expected a number for exponent, found '" << c << "'\n";
                        return std::move( EJSON::Make( EJSON::Class::Null ) );
                    }
                    else
                        break;
                }
                exp = std::stol( exp_str );
            }
            else if( !isspace( c ) && c != ',' && c != ']' && c != '}' )
            {
                std::cerr << "ERROR: Number: unexpected character '" << c << "'\n";
                return std::move( EJSON::Make( EJSON::Class::Null ) );
            }
            --offset;
        
            if (isDouble)
            {
                Number = std::stod(val) * std::pow(10, exp);
            }
            else
            {
                if( !exp_str.empty() )
                    Number = std::stol( val ) * std::pow( 10, exp );
                else
                    Number = std::stol( val );
            }
            return std::move( Number );
        }

        EJSON parse_bool( const string &str, size_t &offset )
        {
            EJSON Bool;
            if (str.substr(offset, 4) == "true")
            {
                Bool = true;
            }
            else if (str.substr(offset, 5) == "false")
            {
                Bool = false;
            }
            else
            {
                std::cerr << "ERROR: Bool: Expected 'true' or 'false', found '" << str.substr( offset, 5 ) << "'\n";
                return std::move( EJSON::Make( EJSON::Class::Null ) );
            }
            offset += (Bool.ToBool() ? 4 : 5);
            return std::move( Bool );
        }

        EJSON parse_null( const string &str, size_t &offset )
        {
            EJSON Null;
            if( str.substr( offset, 4 ) != "null" )
            {
                std::cerr << "ERROR: Null: Expected 'null', found '" << str.substr( offset, 4 ) << "'\n";
                return std::move( EJSON::Make( EJSON::Class::Null ) );
            }
            offset += 4;
            return std::move( Null );
        }

        EJSON parse_next( const string &str, size_t &offset )
        {
            char value;
            consume_ws( str, offset );
            value = str[offset];
            switch( value )
            {
                case '[' : return std::move( parse_array( str, offset ) );
                case '{' : return std::move( parse_object( str, offset ) );
                case '\"': return std::move( parse_string( str, offset ) );
                case 't' :
                case 'f' : return std::move( parse_bool( str, offset ) );
                case 'n' : return std::move( parse_null( str, offset ) );
                default  : if( ( value <= '9' && value >= '0' ) || value == '-' )
                               return std::move( parse_number( str, offset ) );
            }
            std::cerr << "ERROR: Parse: Unknown starting character '" << value << "'\n";
            return EJSON();
        }
    }

    EJSON EJSON::Load( const string &str )
    {
        size_t offset = 0;
        return std::move( parse_next( str, offset ) );
    }

} // End Namespace enscape
