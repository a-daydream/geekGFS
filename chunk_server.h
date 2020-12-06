#ifndef CHUNK_SERVER_H
#define CHUNK_SERVER_H
#include"utility.h"
#include <unistd.h>
#include<sys/stat.h>
#include<iostream>
#include<fstream>
#include"thread"
#include<memory>
#include<string>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include"gfs.grpc.pb.h"

#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)  
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using gfs::Request;
using gfs::Reply;
using gfs::ChunkServerToClient;





class chunk_server  final :public ChunkServerToClient::Service
{
private:
    std::string port;
    std::string root;
public:
    chunk_server(std::string &port,std::string &root)
    {
        this->port = port;
        this->root = "./" + root + "/" + port;
        if(access(root.c_str(),F_OK)){
            mkdir(root.c_str(),MODE);
        }
        if(access(this->root.c_str(),F_OK)){
            mkdir(this->root.c_str(),MODE);
        }
    }

    void create(std::string &chunk_handle,status_code& s);
    void get_chunk_space(std::string &chunk_handle,int &chunk_space,status_code& s);
    void write(std::string &chunk_handle,std::string &data,status_code& s);
    void append(std::string &chunk_handle,std::string &data,status_code& s);
    void read(std::string &chunk_handle,std::string& data,status_code& s);


    Status Create(ServerContext* context,const Request* request ,Reply* reply) override;
    Status Write(ServerContext* context,const Request* request ,Reply* reply) override;
    Status Read(ServerContext* context,const Request* request ,Reply* reply) override;
    Status GetChunkSpace(ServerContext* context,const Request* request ,Reply* reply) override;
    Status Append(ServerContext* context,const Request* request ,Reply* reply) override;

    ~chunk_server(){}
  
};


void RunChunkServer(std::string port);


#endif



