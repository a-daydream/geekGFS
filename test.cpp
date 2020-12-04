#include"utility.h"
#include <uuid/uuid.h>
#include<unordered_map>

int main(int argc, char** argv)
{

    std::unordered_map<std::string,int> s_to_int;
    s_to_int.insert(std::pair<std::string,int>("a",1));
    s_to_int.insert(std::pair<std::string,int>("a",2));
    // std::unordered_map<std::string,int>::iterator it;
    // it = s_to_int.find("a");
    // s_to_int.erase(it);
    // it = s_to_int.find("a");
    std::cout<<s_to_int.size()<<std::endl;
    // uuid_t uuid;
    // char str[36];

    // uuid_generate(uuid);
    // uuid_unparse(uuid, str);

    // std::string st = std::string(str);
    // std::cout<<st<<std::endl;
    return 0;
}