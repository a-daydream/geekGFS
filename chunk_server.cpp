#include"chunk_server.h"

void split(std::string str,std::vector<std::string>& chunkhandle_ports){
    char *token;
    char *text = (char *)str.c_str();
    token = strtok_r(text, "|", &text);
    while(token) {
        chunkhandle_ports.push_back(token);
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

void chunk_server::get_chunk_space(std::string &chunk_handle,float &chunk_space,status_code& s)
{

}

void chunk_server::write(std::string &chunk_handle,std::string &data,status_code& s)
{
    std::string file_path = this->root + "/" + chunk_handle;
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

}

void chunk_server::read(std::string &chunk_handle,status_code& s)
{

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
    split(request->send_message(),chunk_handle_and_data);
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

}

Status chunk_server::GetChunkSpace(ServerContext* context,const Request* request ,Reply* reply)
{

}

Status chunk_server::Append(ServerContext* context,const Request* request ,Reply* reply)
{

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