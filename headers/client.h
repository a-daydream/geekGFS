#ifndef CLIENT_H
#define CLIENT_H


#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>

#include"gfs.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using gfs::Request;
using gfs::Reply;
using gfs::MasterServerToClient;
using gfs::ChunkServerToClient;

class client
{
private:
    std::unique_ptr<MasterServerToClient::Stub> master_stub_;
    std::unique_ptr<ChunkServerToClient::Stub> chunkserver_stub_;
public:
    client(std::shared_ptr<Channel> master_channel)
        : master_stub_(MasterServerToClient::NewStub(master_channel)){}
    


    std::string ListFiles(const std::string & request);
    std::string CreateFile(const std::string & request);
    std::string DeleteFile(const std::string & request);
    std::string ReadFile(const std::string & request);
    std::string WriteFile(const std::string & request);
    std::string UndeleteFile(const std::string & request);
    std::string AppendFile(const std::string & request);
    
    std::string Create(const std::string & request);
    std::string GetChunkSpace(const std::string & request);
    std::string Write(const std::string & request);
    std::string Append(const std::string & request);
    std::string Read(const std::string & request);

    ~client(){}
};

void RunClient();

#endif