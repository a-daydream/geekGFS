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
#include<map>
#include"cstdlib"
#include"ctime"
#include"chunk_server.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using gfs::Request;
using gfs::Reply;
using gfs::MasterServerToClient;


void choose_locs(std::vector<std::string>& chunk_server_locations)
{
    gfs_config config;
    int total = config.chunk_server_locations.size();
    srand((int)time(0));
    int num = rand() % total; 
    chunk_server_locations.push_back(config.chunk_server_locations[num]);
    chunk_server_locations.push_back(config.chunk_server_locations[(num+1)%total]);
    chunk_server_locations.push_back(config.chunk_server_locations[(num+2)%total]);
}

struct chunk
{
    std::vector<std::string> locations;
};

class file
{
private:
    std::string file_path;
    std::map<std::string,chunk&> chunks;
public:
    file(std::string &file_path): file_path(file_path){}
    std::map<std::string,chunk&>& get_chunks(){return this->chunks;}
    ~file(){}
};

class meta_data
{
private:
    std::vector<std::string> locations;
    std::map<std::string,file> files; //filepath to file
    std::map<std::string,file> chunkhandle_to_file;
    std::map<std::string,std::vector<std::string>> locations_dict;
public:
    meta_data(std::vector<std::string> locations):locations(locations){}
    void get_latest_chunk(std::string &file_path,std::string&latest_chunk_handle);
    void get_chunk_locations(const std::string chunk_handle,std::vector<std::string>& chunk_location);
    void create_new_file(std::string &file_path,std::string chunk_handle,status_code& s);
    void create_new_chunk(std::string &file_path,std::string prev_chunk_handle,std::string chunk_handle,status_code& s);
    void delete_file(std::string &file_path);
    std::map<std::string,file>& get_files(){return this->files;};
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

    void get_chunk_handle(std::string& s_uuid);
    void  check_valid_file(std::string &file_path,status_code& s);
    void  list_files(std::string &file_path,std::vector<std::string>& files);
    void create_file(std::string &file_path,std::string &chunk_handle,std::vector<std::string> &locations,status_code& s);
    status_code append_file(std::string &file_path,std::string &latest_chunk_handle,std::vector<std::string> &locations);
    status_code create_chunk(std::string &file_path,std::string &prev_chunk_handle,std::vector<std::string> &locations);
    void read_file(std::string &file_path,status_code& s);
    void write_file(std::string &file_path,std::string &data,std::string& chunks,status_code& s);
    void delete_file(std::string &file_path,status_code& s);


    Status CreateFile(ServerContext* context,const Request* request ,Reply* reply) override;
    Status DeleteFile(ServerContext* context,const Request* request ,Reply* reply) override;
    Status ListFiles(ServerContext* context,const Request* request ,Reply* reply) override;
    Status ReadFile(ServerContext* context,const Request* request ,Reply* reply) override;
    Status CreateChunk(ServerContext* context,const Request* request ,Reply* reply) override;
    Status WriteFile(ServerContext* context,const Request* request ,Reply* reply) override;
    Status AppendFile(ServerContext* context,const Request* request ,Reply* reply) override;
    ~master_server(){}
};


void RunMasterServer();


#endif