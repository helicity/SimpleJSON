# SimpleJSON

nbsdx 의 SimpleJSON 을 가져와서 기본 포매팅을 추가하고 태그명 순서 유지하게 하도록 기능 수정했다.
원래 json.hpp 파일 하나로만 되어있어서, 여러군데서 사용할려고 하면 심벌 중복으로 링크가 안된다. 그래서 cpp파일 분리했다.

예제 파일이나 빌드 스크립트 등등은 싱크가 맞지 않는다.

## License
원래 라이선스를 유지한다.
(original license = Do what the fuck you want public license)



## Upcoming Features

## Example

```cpp
#include "json.hpp"

    // 파일에서 불러오기
    std::ifstream file_in("../json_sample/module_h0_s0.json");
    EJSON obj = EJSON::Load(file_in);
    std::cout << obj << std::endl;

    // 스트링 버퍼에서 불러오기
    std::string text = R"(  { "name" : "hahaha", "score" : 13, "friends" : [ "james", "sam" ] }  )";
    obj = EJSON::Load(text);

    // 텍스트 화면에 출력
    EJSON::SetOutputFormat(2); // output with shortest format
    //EJSON::SetOutputFormat(1); // output with condensed format
    //EJSON::SetOutputFormat(0); // output with normal format
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // 파일로 저장
    std::ofstream file_out("output.json");
    file_out << obj;


    // 오브젝트 동적으로 생성 (오브젝트는 string으로 인덱싱 한다)
    obj = enscape::Object();
    obj["name"] = "hahaha";                     // { "name" : "hahaha" }
    obj["score"] = 13;                           // { "name" : "hahaha", "score" : 13 }
    obj["friends"] = { "james", "best" };          // { "name" : "hahaha", "score" : 13, "friends" : { "james" : "best" } }
    obj["friends"] = enscape::Array("ridia", "jack"); // { "name" : "hahaha", "score" : 13, "friends" : [ "ridia", "jack" ] }
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // 어레이 동적으로 생성 (어레이는 int로 인덱싱 한다)
    EJSON array = enscape::Array();;
    array[2] = 15;     // [null, null, 15]
    array[1] = true;     // [null, true, 15]
    array[0] = 13;     // [13, true, 15]
    array.append(77); // 항상 array가 되어버린다. array가 아니었다면 기존 데이터는 다날아감!!!
    std::cout << array << std::endl;
    std::cout << "--------------------------------------------\n";

    // 항목 삽입
    EJSON array2 = enscape::Array(1, "haha", true); // [1, "haha", true]
    array[3] = array2; // [13, 14, 15, [1, "haha", true] ]
    //array[3]    = enscape::Array( 1, "haha", true); // 동일
    std::cout << array << std::endl;
    std::cout << "--------------------------------------------\n";

    // 항목 삭제
    obj.remove("friends"); // 없으면 아무일도 안일어남
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // 항목 삽입
    obj["my_info"] = { "age", 35, "handsome", true, "family", enscape::Array("AlexandraDaddario", "ChouTzuYu", "LeeChaeYeon") };
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // 오브젝트 종류 변경 (기존 데이터는 경고없이 다날아감!)
    obj[0] = 111; // array로 변경
    obj = true; // bool 로 변경
    obj = "Test String"; // string으로 변경
    std::cout << obj << std::endl;
    std::cout << "--------------------------------------------\n";

    // 다중 인덱싱
    obj = enscape::Object();
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
```
Sadly, we don't have access to the : character in C++, so we can't use that to seperate key-value pairs, but by using commas, we can achieve a very similar effect. The other point you might notice, is that we have to explictly create arrays. This is a limitation of C++'s operator overloading rules, so we can't use the [] operator to define the array :( I'm looking into ways to make this smoother.
