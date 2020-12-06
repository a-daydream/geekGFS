#include"chunk_server.h"

void split1(std::string str,std::vector<std::string>& strings){
    char *token;
    char *text = (char *)str.c_str();
    token = strtok_r(text, "|", &text);
    while(token) {
        strings.push_back(token);
        token = strtok_r(text, "|" , &text);
    }
}

void chunk_server::create(std::string &chunk_handle,status_code& s)
{
    std::string file_path = this->root + "/" + chunk_handle;
    try
    {
        std::fstream file;
        file.open(file_path,std::ios::out);
        file.close();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        s.value = -1;
        s.exception = "ERROR :" +std::string(e.what());
    }

    s.value = 0;
    s.exception = "SUCCESS: chunk created";
    
}

void chunk_server::get_chunk_space(std::string &chunk_handle,int &chunk_space,status_code& s)
{
    std::string file_path = this->root + "/" + chunk_handle;
    std::cout<<"get remain size of " << file_path<<std::endl;
    try
    {
        std::ifstream file;
        file.open(file_path,std::ios::out);
        std::string data;
        file>>data;
        chunk_space = data.size();
        file.close();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        s.value = -1;
        s.exception = "ERROR :" +std::string(e.what());
    }
    s.value = 0;
    s.exception = "SUCCESS: "+ this->port +" get remain size of " + file_path;

}

void chunk_server::write(std::string &chunk_handle,std::string &data,status_code& s)
{
    std::string file_path = this->root + "/" + chunk_handle;
    std::cout<<"write " << file_path<<std::endl;
    try
    {
        std::ofstream file;
        file.open(file_path,std::ios::out);
        file<<data;
        file.close();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        s.value = -1;
        s.exception = "ERROR :" +std::string(e.what());
    }

    s.value = 0;
    s.exception = "SUCCESS: "+ this->port +" writed data into " + chunk_handle;
}

void chunk_server::append(std::string &chunk_handle,std::string &data,status_code& s)
{
    std::string file_path = this->root + "/" + chunk_handle;
    std::cout<<" append " + data +" into " << file_path<<std::endl;
    try
    {
        std::ofstream file;
        file.open(file_path,std::ios::app);
        file<<data;
        file.close();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        s.value = -1;
        s.exception = "ERROR :" +std::string(e.what());
    }

    s.value = 0;
    s.exception = "SUCCESS: "+ this->port +" read data from " + chunk_handle;
}

void chunk_server::read(std::string &chunk_handle,std::string& data,status_code& s)
{
    std::string file_path = this->root + "/" + chunk_handle;
    std::cout<<"read " << file_path<<std::endl;

    try
    {
        std::ifstream file;
        file.open(file_path,std::ios::in);
        file>>data;
        file.close();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        s.value = -1;
        s.exception = "ERROR :" +std::string(e.what());
    }

    s.value = 0;
    s.exception = "SUCCESS: "+ this->port +" read data from " + chunk_handle;
}


Status chunk_server::Create(ServerContext* context,const Request* request ,Reply* reply)
{
    std::string chunk_handle = request->send_message();
    std::cout<<this->port +std::string(" CreateChunk ") + chunk_handle<<std::endl;
    status_code s;
    this->create(chunk_handle,s);
    reply->set_reply_message(s.exception);
    return Status::OK;
}

Status chunk_server::Write(ServerContext* context,const Request* request ,Reply* reply)
{
    std::vector<std::string> chunk_handle_and_data;
    split1(request->send_message(),chunk_handle_and_data);
    std::string data = chunk_handle_and_data[1];
    std::string chunk_handle = chunk_handle_and_data[0];

    std::cout<<this->port +std::string(" WriteChunk ")+ data + " to "+ chunk_handle<<std::endl;
    status_code s;
    this->write(chunk_handle,data,s);

    if(s.value != 0){
        reply->set_reply_message(s.exception);
        return Status::OK;
    }

    reply->set_reply_message(s.exception);
    return Status::OK;
}

Status chunk_server::Read(ServerContext* context,const Request* request ,Reply* reply)
{
    std::string chunk_handle = request->send_message();
    std::string data;
    status_code s;
    this->read(chunk_handle,data,s);

    if(s.value != 0){
        reply->set_reply_message(s.exception);
        return Status::OK;
    }
    
    reply->set_reply_message(data);
    return Status::OK;
}

Status chunk_server::GetChunkSpace(ServerContext* context,const Request* request ,Reply* reply)
{
    std::string chunk_handle = request->send_message();
    std::cout<<this->port +std::string(" Get ChunkRemainSize of ") + chunk_handle<<std::endl;
    status_code s;
    int chunk_rem_size;
    this->get_chunk_space(chunk_handle,chunk_rem_size,s);

    if(s.value != 0){
        reply->set_reply_message(s.exception);
        return Status::OK;
    }
    
    reply->set_reply_message(std::to_string(chunk_rem_size));
    return Status::OK;

}

Status chunk_server::Append(ServerContext* context,const Request* request ,Reply* reply)
{
    std::vector<std::string> strings;
    split1(request->send_message(),strings);
    std::string chunk_handle = strings[0];
    std::string data = strings[1];
    std::cout<<this->port +std::string(" append ")+ data +" into " + chunk_handle<<std::endl;
    status_code s;
    this->append(chunk_handle,data,s);

    if(s.value != 0){
        reply->set_reply_message(s.exception);
        return Status::OK;
    }
    
    reply->set_reply_message(s.exception);
    return Status::OK;
}


void RunChunkServer(std::string port)
{
    std::string chunk_server_address = "localhost:" + port;
    gfs_config config;
    chunk_server service(chunk_server_address,config.chunk_server_root);

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;

    builder.AddListeningPort(chunk_server_address, grpc::InsecureServerCredentials());

    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "ChunkServer listening on " << chunk_server_address << std::endl;

    server->Wait();
}

int main(void)
{
    gfs_config config;

    std::thread t1(RunChunkServer,config.chunk_server_locations[0]);
    std::thread t2(RunChunkServer,config.chunk_server_locations[1]);
    std::thread t3(RunChunkServer,config.chunk_server_locations[2]);
    std::thread t4(RunChunkServer,config.chunk_server_locations[3]);
    std::thread t5(RunChunkServer,config.chunk_server_locations[4]);


    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    return 0;
}