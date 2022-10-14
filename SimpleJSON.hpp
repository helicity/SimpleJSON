// from https://github.com/nbsdx/SimpleJSON
//
// modified by jeffrey
//

/*
#include "SimpleJSON.hpp"
#include <iostream>
#include <cstddef>
#include <ios>

using enscape::EJSON;
using namespace std;

void test()
{
    // 파일에서 불러오기
    std::ifstream file_in("../json_sample/module_h0_s0.json");
    EJSON obj = EJSON::Load(file_in);
    std::cout << obj << std::endl;

    // 스트링 버퍼에서 불러오기
    std::string text = R"(  { "name" : "hahaha", "score" : 13, "friends" : [ "james", "sam" ] }  )";
    obj = EJSON::Load(text);

    // 텍스트 화면에 출력
    EJSON::SetOutputFormat(2);   // output with shortest format
    //EJSON::SetOutputFormat(1); // output with condensed format
    //EJSON::SetOutputFormat(0); // output with normal format
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // 파일로 저장
    std::ofstream file_out("output.json");
    file_out << obj;


    // 오브젝트 동적으로 생성 (오브젝트는 string으로 인덱싱 한다)
    obj = EJSON::Object();
    obj["name"] = "hahaha";                         // { "name" : "hahaha" }
    obj["score"] = 13;                              // { "name" : "hahaha", "score" : 13 }
    obj["friends"] = { "james", "best" };           // { "name" : "hahaha", "score" : 13, "friends" : { "james" : "best" } }
    obj["friends"] = EJSON::Array("ridia", "jack"); // { "name" : "hahaha", "score" : 13, "friends" : [ "ridia", "jack" ] }
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // 어레이 동적으로 생성 (어레이는 int로 인덱싱 한다)
    EJSON array = EJSON::Array();;
    array[2] = 15;    // [null, null, 15]
    array[1] = true;  // [null, true, 15]
    array[0] = 13;    // [13, true, 15]
    array.append(77); // 항상 array가 되어버린다. array가 아니었다면 기존 데이터는 다날아감!!!
    std::cout << array << std::endl;
    std::cout << "--------------------------------------------\n";

    // 항목 삽입
    EJSON array2 = EJSON::Array(1, "haha", true); // [1, "haha", true]
    array[3] = array2; // [13, 14, 15, [1, "haha", true] ]
    //array[3]    = EJSON::Array( 1, "haha", true); // 동일
    std::cout << array << std::endl;
    std::cout << "--------------------------------------------\n";

    // 항목 삭제
    obj.remove("friends"); // 없으면 아무일도 안일어남
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // 항목 삽입
    obj["my_info"] = { "age", 35, "handsome", true, "family", EJSON::Array("AlexandraDaddario", "ChouTzuYu", "LeeChaeYeon") };
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // 오브젝트 종류 변경 (기존 데이터는 경고없이 다날아감!)
    obj[0] = 111; // 에러. 암묵적인 변경은 안됨
    obj = true; // bool 로 변경
    obj = "Test String"; // string으로 변경
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // 다중 인덱싱
    obj = EJSON::Object();
    obj["AAA"] = "aaa";           // {"AAA":"aaa"}
    obj["bbb"]["aa"] = 11;          // {"AAA":"aaa","bbb":{"aa":11}}
    obj["bbb"]["bb"] = 22;          // {"AAA":"aaa","bbb":{"aa":11,"bb":22}}
    obj["bbb"]["cc"]["dd"] = 777;   // {"AAA":"aaa","bbb":{"aa":11,"bb":22,"cc":{"dd":777}}}
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // 종류 얻기
    if (obj.GetType()   == EJSON::Class::Object) cout << "Object!!" << endl;
    if (array.GetType() == EJSON::Class::Array ) cout << "Array!!"  << endl;
    if (obj.IsObject()) cout << "Object!!" << endl;
    if (arrayIsArray()) cout << "Array!!"  << endl;
    std::cout << "--------------------------------------------\n";

    // 오브젝트 순회하기 (오브젝트 아닌 경우는 아무것도 안나온다)
    for (auto &o : obj.ObjectRange())  std::cout << "Object[ " << o.first << " ] = " << o.second << "\n";
    std::cout << "--------------------------------------------\n";

    // 어레이 순회하기 (어레이 아닌 경우는 아무것도 안나온다)
    for (auto &a : array.ArrayRange())        std::cout << "Value: " << a << "\n";
    std::cout << "--------------------------------------------\n";

    // 포함 검사
    cout << boolalpha << obj.has("AAA") << endl;
    cout << boolalpha << array.has(13) << endl;
}

*/


#pragma once

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
#include <cassert>

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

    class EJSON
    {
        struct BackingData // 200827 jeffrey : union to struct, by adding Key
        {
            BackingData( double d ) : Float ( d ){}
            BackingData( long   l ) : Int   ( l ){}
            BackingData( bool   b ) : Bool  ( b ){}
            BackingData( string s ) : String( new string( s ) ){}
            BackingData()           : Int   ( 0 ){}

            deque<EJSON>*       List   = nullptr;
            map<string, EJSON>* Map    = nullptr;
            deque<string>*      Key    = nullptr;
            string*             String = nullptr;
            double              Float  = .0f;
            long                Int    = 0;
            bool                Bool   = false;
        } Internal;

    public:
        enum class Class
        {
            Null,
            Object,
            Array,
            String,
            Floating,
            Integral,
            Boolean
        };

        template <typename Container>
        class JSONWrapper
        {
            Container *object;

            public:
                JSONWrapper( Container *val ) : object( val     ) {}
                JSONWrapper( std::nullptr_t ) : object( nullptr ) {}

                typename Container::iterator begin()             { return object ? object->begin() : typename Container::iterator(); }
                typename Container::iterator end()               { return object ? object->end()   : typename Container::iterator(); }
                typename Container::const_iterator begin() const { return object ? object->begin() : typename Container::iterator(); }
                typename Container::const_iterator end() const   { return object ? object->end()   : typename Container::iterator(); }
        };

        template <typename Container>
        class JSONConstWrapper
        {
            const Container *object;

            public:
                JSONConstWrapper( const Container *val ) : object( val ) {}
                JSONConstWrapper( std::nullptr_t )  : object( nullptr ) {}

                typename Container::const_iterator begin() const { return object ? object->begin() : typename Container::const_iterator(); }
                typename Container::const_iterator end()   const { return object ? object->end()   : typename Container::const_iterator(); }
        };

        EJSON();
        EJSON(std::initializer_list<EJSON> list);
        EJSON(EJSON&& other);
        EJSON& operator=(EJSON&& other);
        EJSON(const EJSON &other);
        EJSON& operator=(const EJSON &other);
        EJSON(Class type);
        ~EJSON();

        EJSON(int val)          { SetType(Class::Integral); Internal.Int    = val; }
        EJSON(std::string val)  { SetType(Class::String);  *Internal.String = val; }
        EJSON(const char* val)  { SetType(Class::String);  *Internal.String = val; }
        EJSON(bool val)         { SetType(Class::Boolean);  Internal.Bool   = val; }
        EJSON(double val)       { SetType(Class::Floating); Internal.Float  = val; }
        EJSON(float val)        { SetType(Class::Floating); Internal.Float  = val; }

        template <typename T>
        EJSON( T b, typename enable_if<is_same<T,bool>::value>::type* = 0 ) : Internal( b ), Type( Class::Boolean ){}

        template <typename T>
        EJSON( T i, typename enable_if<is_integral<T>::value && !is_same<T,bool>::value>::type* = 0 ) : Internal( (long)i ), Type( Class::Integral ){}

        template <typename T>
        EJSON( T f, typename enable_if<is_floating_point<T>::value>::type* = 0 ) : Internal( (double)f ), Type( Class::Floating ){}

        template <typename T>
        EJSON( T s, typename enable_if<is_convertible<T,string>::value>::type* = 0 ) : Internal( string( s ) ), Type( Class::String ){}

        EJSON(std::nullptr_t);

        static EJSON Make(Class type);
        static EJSON Load(std::istream &);
        static EJSON Load(const string &);

        static EJSON Object();
        static EJSON Array();

        const std::deque<std::string>* GetNames() const { return Internal.Key; }

        template <typename... T>
        static EJSON Array(T... args)
        {
            EJSON arr = EJSON::Make(EJSON::Class::Array);
            arr.append(args...);
            return std::move(arr);
        }

        static EJSON Object(std::initializer_list<EJSON> list)
        {
            EJSON obj = EJSON::Make(EJSON::Class::Object);
            for (auto i = list.begin(), e = list.end(); i != e; ++i, ++i)
                obj.operator[](i->ToString()) = *std::next(i);
            return std::move(obj);
        }

        template <typename T>
        void append( T arg )
        {
            SetType( Class::Array ); Internal.List->emplace_back( arg );
        }

        template <typename T, typename... U>
        void append( T arg, U... args )
        {
            append( arg ); append( args... );
        }

        template <typename T>
        typename enable_if<is_same<T,bool>::value, EJSON&>::type operator=( T b )
        {
            SetType( Class::Boolean ); Internal.Bool = b; return *this;
        }

        template <typename T>
        typename enable_if<is_integral<T>::value && !is_same<T,bool>::value, EJSON&>::type operator=( T i )
        {
            SetType( Class::Integral ); Internal.Int = i; return *this;
        }

        template <typename T>
        typename enable_if<is_floating_point<T>::value, EJSON&>::type operator=( T f )
        {
            SetType( Class::Floating ); Internal.Float = f; return *this;
        }

        template <typename T>
        typename enable_if<is_convertible<T,string>::value, EJSON&>::type operator=( T s )
        {
            SetType( Class::String ); *Internal.String = string( s ); return *this;
        }

        operator int        () const { if (Type==Class::Integral) return  Internal.Int;             return 0; }
        operator std::string() const { if (Type==Class::String)   return *Internal.String;          return ""; }
        operator const char*() const { if (Type==Class::String)   return  Internal.String->c_str(); return nullptr; }
        operator bool       () const { if (Type==Class::Boolean)  return  Internal.Bool;            return false; }
        operator double     () const { if (Type==Class::Floating) return  Internal.Float;           return .0; }
        operator float      () const { if (Type==Class::Floating) return  (float)Internal.Float;    return .0f; }

    public:
        EJSON& operator[](const string& key);
        const EJSON& operator[](const string& key) const;

        EJSON& operator[](const char* key);
        const EJSON& operator[](const char* key) const;

        EJSON& operator[](int index);
        const EJSON& operator[](const int index) const;

        EJSON &at(const string &key);
        const EJSON &at(const string &key) const;

        EJSON &at(unsigned index);
        const EJSON &at(unsigned index) const;

        bool remove(const string& key); // remove subitem from object
        bool remove(const int index);   // remove subitem from array

        template <typename T>
        bool has(const T& key)
        {
            if (Type == Class::Object)
                return hasKey(key);
            if (Type == Class::Array)
            {
                for (auto &a : ArrayRange())
                    if (a.Type == enscape::EJSON::Class::String && a.ToString() == key)
                        return true;
            }
            return false;
        }

        template <>
        bool has<double>(const double& key)
        {
            if (Type == Class::Array)
            {
                for (auto &a : ArrayRange())
                    if (a.Type == enscape::EJSON::Class::Floating && a.ToFloat() == key)
                        return true;
            }
            return false;
        }

        template <>
        bool has<int>(const int& key)
        {
            if (Type == Class::Array)
            {
                for (auto &a : ArrayRange())
                    if (a.Type == enscape::EJSON::Class::Integral && a.ToInt() == key)
                        return true;
            }
            return false;
        }

        template <>
        bool has<bool>(const bool& key)
        {
            if (Type == Class::Array)
            {
                for (auto &a : ArrayRange())
                    if (a.Type == enscape::EJSON::Class::Boolean && a.ToBool() == key)
                        return true;
            }
            return false;
        }

        bool hasKey(const string &key) const;
        bool hasIndex(const int index) const;

        int size() const;

        Class JSONType() const { return Type; }

        /// Functions for getting primitives from the EJSON object.
        bool IsNull() const { return Type == Class::Null; }

        string ToString() const { bool b; return std::move( ToString( b ) ); }
        string ToString(bool &ok) const;
        double ToFloat() const { bool b; return ToFloat( b ); }
        double ToFloat(bool &ok) const;
        long ToInt() const { bool b; return ToInt( b ); }
        long ToInt(bool &ok) const;
        bool ToBool() const { bool b; return ToBool( b ); }
        bool ToBool(bool &ok) const;

        JSONWrapper<map<string, EJSON>> ObjectRange();
        JSONWrapper<deque<EJSON>> ArrayRange();
        JSONConstWrapper<map<string, EJSON>> ObjectRange() const;
        JSONConstWrapper<deque<EJSON>> ArrayRange() const;

        string dump(int depth = 1) const;

        friend std::ostream& operator<<( std::ostream&, const EJSON & );

    private:
        void SetType(Class type);

    public:
        Class GetType() { return Type; }
        bool IsObject() const  { return this->Type == EJSON::Class::Object; }
        bool IsArray() const   { return this->Type == EJSON::Class::Array; }
        bool IsString() const  { return this->Type == EJSON::Class::String; }
        bool IsFloat() const   { return this->Type == EJSON::Class::Floating; }
        bool IsInteger() const { return this->Type == EJSON::Class::Integral; }
        bool IsBool() const    { return this->Type == EJSON::Class::Boolean; }

        // level = 0:normal, 1:condensed, 2:shortest
        static void SetOutputFormat(int level = 0);


    private:
        /* beware: only call if YOU know that Internal is allocated. No checks performed here. 
            This function should be called in a constructed EJSON just before you are going to 
        overwrite Internal... 
        */
        void ClearInternal();

    private:

        Class Type = Class::Null;

        // dump formatting
        static std::string TAB;
        static std::string SPACE;
        static std::string NEWLINE;
        static bool spacing_option_1;
        static bool spacing_option_2;
        static bool spacing_option_3;
    };


    std::ostream& operator<<(std::ostream &os, const EJSON &enscape);

} // End Namespace enscape
