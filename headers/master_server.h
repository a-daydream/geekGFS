#ifndef MASTER_SERVER_H
#define MASTER_SERVER_H

#include<iostream>
#include<memory>
#include<string>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include"utility.h"
#include"gfs.grpc.pb.h"
#include <uuid/uuid.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using gfs::Request;
using gfs::Reply;
using gfs::MasterServerToClient;

struct chunk
{
    std::vector<std::string> locations;
};

class file
{
private:
    std::string file_path;
    std::unordered_map<uuid_t,chunk>* chunks;
    bool file_delete ;
public:
    file(std::string &file_path): file_path(file_path),file_delete(false){}
    ~file(){}
};

class meta_data
{
private:
    std::vector<std::string> locations;
    std::unordered_map<std::string,file>* files;
    std::unordered_map<std::string,std::vector<uuid_t>>* locations_dict;
public:
    meta_data(std::vector<std::string> locations):locations(locations){}
    std::string get_latest_chunk(std::string &file_path);
    std::vector<std::string> get_chunk_locations(uuid_t chunk_handle);
    status_code create_new_file(std::string &file_path,uuid_t chunk_handle);
    status_code create_new_chunk(std::string &file_path,uuid_t prev_chunk_handle,uuid_t chunk_handle);
    void delete_file(std::string &file_path);
    ~meta_data(){}
};




class master_server final: public MasterServerToClient::Service
{
private:
    std::string port;
    std::vector<std::string> file_list;
    meta_data metaData;
public:
    master_server(std::string &port,std::vector<std::string> locations):port(port),metaData(locations){}

    std::string get_chunk_handle();
    status_code check_valid_file(std::string &file_path);
    std::vector<std::string> list_files(std::string &file_path);
    status_code create_file(std::string &file_path,std::string &chunk_handle,std::vector<std::string> &locations);
    status_code append_file(std::string &file_path,std::string &latest_chunk_handle,std::vector<std::string> &locations);
    status_code create_chunk(std::string &file_path,std::string &prev_chunk_handle,std::vector<std::string> &locations);
    status_code read_file(std::string &file_path);
    status_code delete_file(std::string &file_path);


    Status CreateFile(ServerContext* context,const Request* request ,Reply* reply) override;
    Status DeleteFile(ServerContext* context,const Request* request ,Reply* reply) override;
    Status ListFiles(ServerContext* context,const Request* request ,Reply* reply) override;
    Status ReadFile(ServerContext* context,const Request* request ,Reply* reply) override;
    Status CreateChunk(ServerContext* context,const Request* request ,Reply* reply) override;
    Status WriteFile(ServerContext* context,const Request* request ,Reply* reply) override;
    Status UndeleteFile(ServerContext* context,const Request* request ,Reply* reply) override;
    Status AppendFile(ServerContext* context,const Request* request ,Reply* reply) override;
    ~master_server(){}
};


void RunMasterServer();


#endif