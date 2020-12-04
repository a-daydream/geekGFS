#include"utility.h"
#include <uuid/uuid.h>
#include<unordered_map>


class meta_data
{
    private:
        std::unordered_map<std::string,std::string> files;
    public:
        meta_data(){}
        std::unordered_map<std::string,std::string>& get_files(){return this->files;};
};


int main(int argc, char** argv)
{
    
    meta_data*m = new meta_data();
    m->get_files().insert(std::pair<std::string,std::string>("a","b"));
    std::cout<<m->get_files().size()<<std::endl;

    std::unordered_map<std::string,int> s_to_int;
    s_to_int.insert(std::pair<std::string,int>("a",1));
    // std::unordered_map<std::string,int>::iterator it = s_to_int.find("a");
    // std::unordered_map<std::string,int>::iterator end = s_to_int.end();
    std::cout<<s_to_int.size()<<std::endl;
    // std::unordered_map<std::string,int>::iterator it;
    // it = s_to_int.find("a");
    // s_to_int.erase(it);
    // it = s_to_int.find("a");
    // std::cout<<(it==end)<<std::endl;
    // uuid_t uuid;
    // char str[36];

    // uuid_generate(uuid);
    // uuid_unparse(uuid, str);

    // std::string st = std::string(str);
    // std::cout<<st<<std::endl;
    return 0;
}