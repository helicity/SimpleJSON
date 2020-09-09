// test_SimpleJSON.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../SimpleJSON.hpp"
using enscape::EJSON;
using namespace std;
#include <windows.h>

#pragma execution_character_set( "utf-8" )






int _tmain(int argc, _TCHAR* argv[])
{
    SetConsoleOutputCP(65001);


    if (1)
    {
        // operator[] 참조 테스트
        // 
        // LHS 는 없으면 생겨야 하고
        // LHS가 타입이 다르면 바뀌지 않는다.
        // 이때 assert로 DEBUG에서만 경고한다.
        // 
        // RHS는 없으면 null이나 기본값이 나와야 한다

        EJSON a = EJSON::Array();
        int i = a[3];   // EJSON()리턴 (자동으로 안생김)
std::cout << a << "----------" << std::endl;
        a[3] = "a";     // 없으면 자동으로 생김
std::cout << a << "----------" << std::endl;
        a[3] = 22;      // 있으면 overwrite
std::cout << a << "----------" << std::endl;
        a[3] = EJSON::Array("ridia", "jack");
std::cout << a << "----------" << std::endl;


        a[3][1] = "aaaa"; // a[3]이 이미 array이므로 [1]로 인덱싱 할 수 있다.

std::cout << a << "----------" << std::endl;

        a[3][1] = EJSON::Object({ "haha", 1, "hoho", 2 });

        std::cout << "--------------------------------------------\n";
    }

    if (0)
    {
        EJSON obj;

        // 오브젝트 동적으로 생성 (오브젝트는 string으로 인덱싱 한다)
        obj = EJSON::Object();
        obj["name"] = "hahaha";
        obj["score"] = 13;
        std::cout << obj << std::endl;
        std::cout << "--------------------------------------------\n";

        obj["name"] = obj.at("score");
        obj.at("score") = "26";
        std::cout << obj << std::endl;
        std::cout << "--------------------------------------------\n";


        ////// 어레이 동적으로 생성 (어레이는 int로 인덱싱 한다)
        ////EJSON array = EJSON::Array();;
        ////array[2] = 15;     // [null, null, 15]
        ////array[1] = true;     // [null, true, 15]
        ////array[0] = 13;     // [13, true, 15]
        ////array.append(77); // 항상 array가 되어버린다. array가 아니었다면 기존 데이터는 다날아감!!!
        ////std::cout << array << std::endl;
        ////std::cout << "--------------------------------------------\n";


    }

    if (0)
    {
        std::ifstream file_in("1.json");
        //std::ifstream file_in("2.json");
        EJSON obj = EJSON::Load(file_in);


//이런식으로 중첩하는 경우의 관계 해결

        obj[1]["aa"][3] = "aaaa";

        obj["measurement"]["size"]["preprocessing"] = "afjlaskfjlasdfjlsakdjflsakdjflsakjfl";

        EJSON::SetOutputFormat(0);

        std::ofstream file_out("1_out.json");
        //std::ofstream file_out("2_out.json");
        file_out << obj;
    }

    if (0)
    {
        EJSON obj =
            {
                "Key", 1,
                "Key3", true,
                "Key4", nullptr,
                "Key2", {
                    "Key4", "VALUE",
                    "Arr", EJSON::Array(1, "Str", false)
                }
            }
        ;
        std::cout << obj << std::endl;
    }

    if (0)
    {
        // 파일에서 불러오기
        std::ifstream file_in("1.json");
        EJSON obj = EJSON::Load(file_in);
        std::cout << obj << std::endl;

        // 스트링 버퍼에서 불러오기
        std::string text = R"(  { "name" : "hahaha", "score" : 13, "friends" : [ "james", "sam" ] }  )";
        obj = EJSON::Load(text);

        // 텍스트 화면에 출력
        std::cout << obj << std::endl;
        std::cout << "--------------------------------------------\n";

        // 오브젝트 동적으로 생성 (오브젝트는 string으로 인덱싱 한다)
        obj = EJSON::Object();
        obj["name"] = "hahaha";                     // { "name" : "hahaha" }
        obj["score"] = 13;                           // { "name" : "hahaha", "score" : 13 }
        obj["friends"] = { "james", "best" };          // { "name" : "hahaha", "score" : 13, "friends" : { "james" : "best" } }
        obj["friends"] = EJSON::Array("ridia", "jack"); // { "name" : "hahaha", "score" : 13, "friends" : [ "ridia", "jack" ] }
        std::cout << obj << std::endl;
        std::cout << "--------------------------------------------\n";

        // 어레이 동적으로 생성 (어레이는 int로 인덱싱 한다)
        EJSON array = EJSON::Array();;
        array[2] = 15;     // [null, null, 15]
        array[1] = true;     // [null, true, 15]
        array[0] = 13;     // [13, true, 15]
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
        obj[0] = 111; // array로 변경
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
        if (obj.GetType() == EJSON::Class::Object) cout << "Object!!" << endl;
        if (array.GetType() == EJSON::Class::Array) cout << "Array!!" << endl;
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

	return 0;
}


