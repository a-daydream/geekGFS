#include"client.h"


//RPC for master server
std::string client::ListFiles(const std::string & request)
{
    // this send_message is a file path
    Request master_request;
    master_request.set_send_message(request);

    Reply master_reply;

    ClientContext context;

    Status status = this->master_stub_->ListFiles(&context, master_request, &master_reply);

    if(status.ok()){
        return master_reply.reply_message();
    }else{
        std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
        return "RPC failed";
    }
}

std::string client::CreateFile(const std::string & request)
{

}

std::string client::DeleteFile(const std::string & request)
{

}

std::string client::ReadFile(const std::string & request)
{

}
std::string client::WriteFile(const std::string & request)
{

}
std::string client::UndeleteFile(const std::string & request)
{

}
std::string client::AppendFile(const std::string & request)
{

}


 //RPC for chunkserver   
std::string client::Create(const std::string & request)
{

}
std::string client::GetChunkSpace(const std::string & request)
{

}
std::string client::Write(const std::string & request)
{

}
std::string client::Append(const std::string & request)
{

}
std::string client::Read(const std::string & request)
{

}


// start client

void RunClient()
{
    std::string master_target_str;
    master_target_str = "localhost:50051";
    
    client clienter(grpc::CreateChannel(master_target_str,grpc::InsecureChannelCredentials()));

    std::string user("master");
    std::string reply = clienter.ListFiles(user);
    std::cout << "Greeter received: " << reply << std::endl;
}


int main(void)
{
    RunClient();
    return 0;
}