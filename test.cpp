#include"utility.h"
#include <uuid/uuid.h>
#include<unordered_map>
#include<cstring>
#include<iostream>
#include<fstream>
#include"thread"
#include<sys/stat.h>
#include <unistd.h>
#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)  

void split(std::string str,std::vector<std::string>& chunkhandle_ports){
    char *token;
    char *text = (char *)str.c_str();
    token = strtok_r(text, "|", &text);
    while(token) {
        chunkhandle_ports.push_back(token);
        token = strtok_r(text, "|" , &text);
    }
}

class meta_data
{
    private:
        std::unordered_map<std::string,std::string> files;
    public:
        meta_data(){}
        std::unordered_map<std::string,std::string>& get_files(){return this->files;};
};

// void thread_task(int n) {
//     std::this_thread::sleep_for(std::chrono::seconds(n));
//     std::cout << "hello thread "
//         << std::this_thread::get_id()
//         << " paused " << n << " seconds" << std::endl;
// }

int main(int argc, char** argv)
{

    // std::string root = "root_chunkserver";
    // if(!access(root.c_str(),F_OK)){
    //     std::cout<<"1"<<std::endl;
    //     mkdir(root.c_str(),MODE);
    // }
    std::string root = "./root_chunkserver/root1";
    
    if(access(root.c_str(),F_OK)){
        std::cout<<"2"<<std::endl;
        mkdir(root.c_str(),MODE);
    }
    // std::thread threads[5];
    // std::cout << "Spawning 5 threads...\n";
    // for (int i = 0; i < 5; i++) {
    //     threads[i] = std::thread(thread_task, i + 1);
    // }
    // std::cout << "Done spawning threads! Now wait for them to join\n";
    // for (auto& t: threads) {
    //     t.join();
    // }
    // std::cout << "All threads joined.\n";

    // return EXIT_SUCCESS;


    // try
    // {
    //     std::ofstream file;
    //     file.open("test1",std::ios::out);
    // }
    // catch(const std::exception& e)
    // {
    //     std::cerr << e.what() << '\n';
    // }
    


    // std::string s = "e7f3bab0-48d8-4fc2-b0fd-d257c4657cb2|50056|50052|50053";
    // std::vector<std::string> test;
    // split(s,test);

    // for(int index = 0;index<test.size();index++){
    //     std::cout<<test[index]<<std::endl;
    // }

    
    // meta_data*m = new meta_data();
    // m->get_files().insert(std::pair<std::string,std::string>("a","b"));
    // std::cout<<m->get_files().size()<<std::endl;

    // std::unordered_map<std::string,int> s_to_int;
    // s_to_int.insert(std::pair<std::string,int>("a",1));
    // std::unordered_map<std::string,int>::iterator it = s_to_int.find("a");
    // std::unordered_map<std::string,int>::iterator end = s_to_int.end();
    // std::cout<<s_to_int.size()<<std::endl;
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