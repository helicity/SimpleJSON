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
        // operator[] ���� �׽�Ʈ
        // 
        // LHS �� ������ ���ܾ� �ϰ�
        // LHS�� Ÿ���� �ٸ��� �ٲ��� �ʴ´�.
        // �̶� assert�� DEBUG������ ����Ѵ�.
        // 
        // RHS�� ������ null�̳� �⺻���� ���;� �Ѵ�

        EJSON a = EJSON::Array();
        int i = a[3];   // EJSON()���� (�ڵ����� �Ȼ���)
std::cout << a << "----------" << std::endl;
        a[3] = "a";     // ������ �ڵ����� ����
std::cout << a << "----------" << std::endl;
        a[3] = 22;      // ������ overwrite
std::cout << a << "----------" << std::endl;
        a[3] = EJSON::Array("ridia", "jack");
std::cout << a << "----------" << std::endl;


        a[3][1] = "aaaa"; // a[3]�� �̹� array�̹Ƿ� [1]�� �ε��� �� �� �ִ�.

std::cout << a << "----------" << std::endl;

        a[3][1] = EJSON::Object({ "haha", 1, "hoho", 2 });

        std::cout << "--------------------------------------------\n";
    }

    if (0)
    {
        EJSON obj;

        // ������Ʈ �������� ���� (������Ʈ�� string���� �ε��� �Ѵ�)
        obj = EJSON::Object();
        obj["name"] = "hahaha";
        obj["score"] = 13;
        std::cout << obj << std::endl;
        std::cout << "--------------------------------------------\n";

        obj["name"] = obj.at("score");
        obj.at("score") = "26";
        std::cout << obj << std::endl;
        std::cout << "--------------------------------------------\n";


        ////// ��� �������� ���� (��̴� int�� �ε��� �Ѵ�)
        ////EJSON array = EJSON::Array();;
        ////array[2] = 15;     // [null, null, 15]
        ////array[1] = true;     // [null, true, 15]
        ////array[0] = 13;     // [13, true, 15]
        ////array.append(77); // �׻� array�� �Ǿ������. array�� �ƴϾ��ٸ� ���� �����ʹ� �ٳ��ư�!!!
        ////std::cout << array << std::endl;
        ////std::cout << "--------------------------------------------\n";


    }

    if (0)
    {
        std::ifstream file_in("1.json");
        //std::ifstream file_in("2.json");
        EJSON obj = EJSON::Load(file_in);


//�̷������� ��ø�ϴ� ����� ���� �ذ�

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
        // ���Ͽ��� �ҷ�����
        std::ifstream file_in("1.json");
        EJSON obj = EJSON::Load(file_in);
        std::cout << obj << std::endl;

        // ��Ʈ�� ���ۿ��� �ҷ�����
        std::string text = R"(  { "name" : "hahaha", "score" : 13, "friends" : [ "james", "sam" ] }  )";
        obj = EJSON::Load(text);

        // �ؽ�Ʈ ȭ�鿡 ���
        std::cout << obj << std::endl;
        std::cout << "--------------------------------------------\n";

        // ������Ʈ �������� ���� (������Ʈ�� string���� �ε��� �Ѵ�)
        obj = EJSON::Object();
        obj["name"] = "hahaha";                     // { "name" : "hahaha" }
        obj["score"] = 13;                           // { "name" : "hahaha", "score" : 13 }
        obj["friends"] = { "james", "best" };          // { "name" : "hahaha", "score" : 13, "friends" : { "james" : "best" } }
        obj["friends"] = EJSON::Array("ridia", "jack"); // { "name" : "hahaha", "score" : 13, "friends" : [ "ridia", "jack" ] }
        std::cout << obj << std::endl;
        std::cout << "--------------------------------------------\n";

        // ��� �������� ���� (��̴� int�� �ε��� �Ѵ�)
        EJSON array = EJSON::Array();;
        array[2] = 15;     // [null, null, 15]
        array[1] = true;     // [null, true, 15]
        array[0] = 13;     // [13, true, 15]
        array.append(77); // �׻� array�� �Ǿ������. array�� �ƴϾ��ٸ� ���� �����ʹ� �ٳ��ư�!!!
        std::cout << array << std::endl;
        std::cout << "--------------------------------------------\n";

        // �׸� ����
        EJSON array2 = EJSON::Array(1, "haha", true); // [1, "haha", true]
        array[3] = array2; // [13, 14, 15, [1, "haha", true] ]
        //array[3]    = EJSON::Array( 1, "haha", true); // ����
        std::cout << array << std::endl;
        std::cout << "--------------------------------------------\n";

        // �׸� ����
        obj.remove("friends"); // ������ �ƹ��ϵ� ���Ͼ
        std::cout << obj << std::endl;
        std::cout << "--------------------------------------------\n";

        // �׸� ����
        obj["my_info"] = { "age", 35, "handsome", true, "family", EJSON::Array("AlexandraDaddario", "ChouTzuYu", "LeeChaeYeon") };
        std::cout << obj << std::endl;
        std::cout << "--------------------------------------------\n";

        // ������Ʈ ���� ���� (���� �����ʹ� ������ �ٳ��ư�!)
        obj[0] = 111; // array�� ����
        obj = true; // bool �� ����
        obj = "Test String"; // string���� ����
        std::cout << obj << std::endl;
        std::cout << "--------------------------------------------\n";

        // ���� �ε���
        obj = EJSON::Object();
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

	return 0;
}


