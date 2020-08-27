// from https://github.com/nbsdx/SimpleJSON
//
// modified by jeffrey
//

/*
#include "enscape.hpp"
#include <iostream>
#include <cstddef>
#include <ios>

using enscape::EJSON;
using namespace std;

void test()
{
    // ���Ͽ��� �ҷ�����
    std::ifstream file_in("../json_sample/module_h0_s0.json");
    EJSON obj = EJSON::Load(file_in);
    std::cout << obj << std::endl;

    // ��Ʈ�� ���ۿ��� �ҷ�����
    std::string text = R"(  { "name" : "hahaha", "score" : 13, "friends" : [ "james", "sam" ] }  )";
    obj = EJSON::Load(text);

    // �ؽ�Ʈ ȭ�鿡 ���
    EJSON::SetOutputFormat(2); // output with shortest format
    //EJSON::SetOutputFormat(1); // output with condensed format
    //EJSON::SetOutputFormat(0); // output with normal format
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // ���Ϸ� ����
    std::ofstream file_out("output.json");
    file_out << obj;


    // ������Ʈ �������� ���� (������Ʈ�� string���� �ε��� �Ѵ�)
    obj = enscape::Object();
    obj["name"] = "hahaha";                     // { "name" : "hahaha" }
    obj["score"] = 13;                           // { "name" : "hahaha", "score" : 13 }
    obj["friends"] = { "james", "best" };          // { "name" : "hahaha", "score" : 13, "friends" : { "james" : "best" } }
    obj["friends"] = enscape::Array("ridia", "jack"); // { "name" : "hahaha", "score" : 13, "friends" : [ "ridia", "jack" ] }
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // ��� �������� ���� (��̴� int�� �ε��� �Ѵ�)
    EJSON array = enscape::Array();;
    array[2] = 15;     // [null, null, 15]
    array[1] = true;     // [null, true, 15]
    array[0] = 13;     // [13, true, 15]
    array.append(77); // �׻� array�� �Ǿ������. array�� �ƴϾ��ٸ� ���� �����ʹ� �ٳ��ư�!!!
    std::cout << array << std::endl;
    std::cout << "--------------------------------------------\n";

    // �׸� ����
    EJSON array2 = enscape::Array(1, "haha", true); // [1, "haha", true]
    array[3] = array2; // [13, 14, 15, [1, "haha", true] ]
    //array[3]    = enscape::Array( 1, "haha", true); // ����
    std::cout << array << std::endl;
    std::cout << "--------------------------------------------\n";

    // �׸� ����
    obj.remove("friends"); // ������ �ƹ��ϵ� ���Ͼ
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // �׸� ����
    obj["my_info"] = { "age", 35, "handsome", true, "family", enscape::Array("AlexandraDaddario", "ChouTzuYu", "LeeChaeYeon") };
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // ������Ʈ ���� ���� (���� �����ʹ� ������ �ٳ��ư�!)
    obj[0] = 111; // array�� ����
    obj = true; // bool �� ����
    obj = "Test String"; // string���� ����
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // ���� �ε���
    obj = enscape::Object();
    obj["AAA"] = "aaa";           // {"AAA":"aaa"}
    obj["bbb"]["aa"] = 11;          // {"AAA":"aaa","bbb":{"aa":11}}
    obj["bbb"]["bb"] = 22;          // {"AAA":"aaa","bbb":{"aa":11,"bb":22}}
    obj["bbb"]["cc"]["dd"] = 777;   // {"AAA":"aaa","bbb":{"aa":11,"bb":22,"cc":{"dd":777}}}
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // ���� ���
    if (obj.GetType() == EJSON::Class::Object) cout << "Object!!" << endl;
    if (array.GetType() == EJSON::Class::Array) cout << "Array!!" << endl;
    std::cout << "--------------------------------------------\n";

    // ������Ʈ ��ȸ�ϱ� (������Ʈ �ƴ� ���� �ƹ��͵� �ȳ��´�)
    for (auto &o : obj.ObjectRange())  std::cout << "Object[ " << o.first << " ] = " << o.second << "\n";
    std::cout << "--------------------------------------------\n";

    // ��� ��ȸ�ϱ� (��� �ƴ� ���� �ƹ��͵� �ȳ��´�)
    for (auto &a : array.ArrayRange())        std::cout << "Value: " << a << "\n";
    std::cout << "--------------------------------------------\n";

    // ���� �˻�
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
        struct BackingData // 200827 jeffrey : union to struct, adding Key
        {
            BackingData( double d ) : Float ( d ){}
            BackingData( long   l ) : Int   ( l ){}
            BackingData( bool   b ) : Bool  ( b ){}
            BackingData( string s ) : String( new string( s ) ){}
            BackingData()           : Int   ( 0 ){}

            deque<EJSON>*        List   = nullptr;
            map<string, EJSON>*  Map    = nullptr;
            deque<string>*      Key    = nullptr;
            string*             String = nullptr;
            double              Float;
            long                Int;
            bool                Bool;
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
                JSONWrapper( Container *val ) : object( val ) {}
                JSONWrapper( std::nullptr_t )  : object( nullptr ) {}

                typename Container::iterator begin() { return object ? object->begin() : typename Container::iterator(); }
                typename Container::iterator end() { return object ? object->end() : typename Container::iterator(); }
                typename Container::const_iterator begin() const { return object ? object->begin() : typename Container::iterator(); }
                typename Container::const_iterator end() const { return object ? object->end() : typename Container::iterator(); }
        };

        template <typename Container>
        class JSONConstWrapper
        {
            const Container *object;

            public:
                JSONConstWrapper( const Container *val ) : object( val ) {}
                JSONConstWrapper( std::nullptr_t )  : object( nullptr ) {}

                typename Container::const_iterator begin() const { return object ? object->begin() : typename Container::const_iterator(); }
                typename Container::const_iterator end() const { return object ? object->end() : typename Container::const_iterator(); }
        };

        EJSON();
        EJSON(std::initializer_list<EJSON> list);
        EJSON(EJSON&& other);
        EJSON& operator=(EJSON&& other);
        EJSON(const EJSON &other);
        EJSON& operator=(const EJSON &other);
        ~EJSON();

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

        EJSON& operator[](const string &key);

        EJSON& operator[](unsigned index);

        EJSON &at(const string &key);

        const EJSON &at(const string &key) const;

        EJSON &at(unsigned index);

        const EJSON &at(unsigned index) const;

        void remove(const string& key);

        int length() const;

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

    EJSON Array();

    template <typename... T>
    EJSON Array( T... args )
    {
        EJSON arr = EJSON::Make( EJSON::Class::Array );
        arr.append( args... );
        return std::move( arr );
    }

    EJSON Object();

    std::ostream& operator<<(std::ostream &os, const EJSON &enscape);

} // End Namespace enscape
