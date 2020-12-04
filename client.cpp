#include"client.h"



void split(std::string str,std::vector<std::string>& chunkhandle_ports){
    char *token;
    char *text = (char *)str.c_str();
    token = strtok_r(text, "|", &text);
    while(token) {
        chunkhandle_ports.push_back(token);
        token = strtok_r(text, "|" , &text);
    }
}

void client::set_chunkserver_stub_(std::shared_ptr<Channel> chunkserver_channel)
{
    this->chunkserver_stub_ =   ChunkServerToClient::NewStub(chunkserver_channel);
}



void client::create_file(const std::string file_path)
{
    std::string reply = this->CreateFile(file_path);
    std::cout << "Response from masterserver: " << reply << std::endl;

    std::string prefix("ERROR");
    if(!reply.compare(0,prefix.size(),prefix)){
        return ;
    }

    std::vector<std::string> data;
    split(reply,data);

    std::string chunk_handle = data[0];
    std::string chunkserver_port;
    for(int index=1;index<data.size();index++){
        chunkserver_port = std::string("localhost:")+data[index];
        this->set_chunkserver_stub_(grpc::CreateChannel(chunkserver_port,grpc::InsecureChannelCredentials()));

        Request chunkserver_request;
        chunkserver_request.set_send_message(chunk_handle);

        Reply chunkserver_reply;

        ClientContext context;
        Status status = this->chunkserver_stub_->Create(&context, chunkserver_request, &chunkserver_reply);
        if(status.ok()){
            std::cout<<chunkserver_reply.reply_message()<<std::endl;
        }else{
            std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
            std::cout<<"RPC failed for clientserver "<<chunkserver_port<<std::endl;
        }
    }

}

void client::read_file(const std::string file_path)
{

}

void client::write_file(const std::string file_path)
{

}

void client::append_file(const std::string file_path)
{

}

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
    // this send_message is a file path
    Request master_request;
    master_request.set_send_message(request);

    Reply master_reply;

    ClientContext context;

    Status status = this->master_stub_->CreateFile(&context, master_request, &master_reply);

    if(status.ok()){
        return master_reply.reply_message();
    }else{
        std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
        return "RPC failed";
    }
    
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

    std::string file_name("file1");
    clienter.create_file(file_name);
    
}


int main(void)
{
    RunClient();
    return 0;
}