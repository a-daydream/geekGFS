#include"../headers/master_server.h"

std::string meta_data::get_latest_chunk(std::string &file_path)
{

}
std::vector<std::string> meta_data::get_chunk_locations(uuid_t chunk_handle)
{

}
status_code meta_data::create_new_file(std::string &file_path,uuid_t chunk_handle)
{

}
status_code meta_data::create_new_chunk(std::string &file_path,uuid_t prev_chunk_handle,uuid_t chunk_handle)
{

}
void meta_data::delete_file(std::string &file_path)
{

}




std::string master_server::get_chunk_handle()
{

}
status_code master_server::check_valid_file(std::string &file_path)
{

}
std::vector<std::string> master_server::list_files(std::string &file_path)
{

}
status_code master_server::create_file(std::string &file_path,std::string &chunk_handle,std::vector<std::string> &locations)
{

}
status_code master_server::append_file(std::string &file_path,std::string &latest_chunk_handle,std::vector<std::string> &locations)
{

}
status_code master_server::create_chunk(std::string &file_path,std::string &prev_chunk_handle,std::vector<std::string> &locations)
{

}
status_code master_server::read_file(std::string &file_path)
{

}
status_code master_server::delete_file(std::string &file_path)
{

}




Status master_server::CreateFile(ServerContext* context,const Request* request ,Reply* reply) 
{

}
Status master_server::DeleteFile(ServerContext* context,const Request* request ,Reply* reply) 
{

}
Status master_server::ListFiles(ServerContext* context,const Request* request ,Reply* reply) 
{

}
Status master_server::ReadFile(ServerContext* context,const Request* request ,Reply* reply) 
{

}
Status master_server::CreateChunk(ServerContext* context,const Request* request ,Reply* reply) 
{

}
Status master_server::WriteFile(ServerContext* context,const Request* request ,Reply* reply) 
{

}
Status master_server::UndeleteFile(ServerContext* context,const Request* request ,Reply* reply) 
{

}
Status master_server::AppendFile(ServerContext* context,const Request* request ,Reply* reply) 
{

}



void RunMasterServer()
{
    std::string server_address("0.0.0.0:50051");
    gfs_config config;
    master_server service(server_address,config.chunk_server_locations);

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;

    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;

    server->Wait();
}


int main(void)
{
    RunMasterServer();
    return 0;
}