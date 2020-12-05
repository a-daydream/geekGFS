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

void client::list_file(const std::string file_path)
{
    std::string master_reply = this->ListFiles(file_path);
    std::cout << "Response from masterserver: " << master_reply << std::endl;
}

void client::create_file(const std::string file_path)
{
    std::string master_reply = this->CreateFile(file_path);
    std::cout << "Response from masterserver: " << master_reply << std::endl;

    std::string prefix("ERROR");
    if(!master_reply.compare(0,prefix.size(),prefix)){
        return ;
    }

    std::vector<std::string> data;
    split(master_reply,data);

    std::string chunk_handle = data[0];
    std::string chunkserver_port;
    std::string chunkserver_reply;
    for(int index=1;index<data.size();index++){
        chunkserver_port = std::string("localhost:")+data[index];
        this->set_chunkserver_stub_(grpc::CreateChannel(chunkserver_port,grpc::InsecureChannelCredentials()));
        chunkserver_reply = this->Create(chunk_handle);
        std::cout << "Response from chunkserver: " << chunkserver_reply << std::endl;
    }
}

void client::read_file(const std::string file_path)
{

}

void client::write_file(const std::string file_path,const std::string data)
{

}

void client::append_file(const std::string file_path,const std::string data)
{

}

void client::delete_file(const std::string file_path)
{
    std::string master_reply = this->DeleteFile(file_path);
    std::cout << "Response from masterserver: " << master_reply << std::endl;
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
        return "RPC failed for master server";
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
        return "RPC failed for master server";
    }
}

std::string client::DeleteFile(const std::string & request)
{
    // this send_message is a file path
    Request master_request;
    master_request.set_send_message(request);

    Reply master_reply;

    ClientContext context;

    Status status = this->master_stub_->DeleteFile(&context, master_request, &master_reply);

    if(status.ok()){
        return master_reply.reply_message();
    }else{
        std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
        return "RPC failed for master server";
    }
}

std::string client::ReadFile(const std::string & request)
{

}

std::string client::WriteFile(const std::string & request)
{

}

std::string client::AppendFile(const std::string & request)
{

}


 //RPC for chunkserver   
std::string client::Create(const std::string & request)
{
        Request chunkserver_request;
        chunkserver_request.set_send_message(request);

        Reply chunkserver_reply;

        ClientContext context;
        Status status = this->chunkserver_stub_->Create(&context, chunkserver_request, &chunkserver_reply);
        if(status.ok()){
            return chunkserver_reply.reply_message();
        }else{
            std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
            return "RPC failed for clientserver ";
        }
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

void RunClient(std::string command,std::string file_path,std::string args)
{
    std::string master_target_str;
    master_target_str = "localhost:50051";
    
    client clienter(grpc::CreateChannel(master_target_str,grpc::InsecureChannelCredentials()));

    if(command == "create")
    {
        clienter.create_file(file_path);
    }
    else if(command == "list")
    {
        clienter.list_file(file_path);
    }
    else if(command == "append")
    {
        if(args.size()==0){
            std::cout<<"No input data given to append"<<std::endl;
        }else{
            clienter.append_file(file_path,args);
        }
    }
    else if(command == "read"){
        clienter.read_file(file_path);
    }
    else if(command == "write"){
        if(args.size()==0){
            std::cout<<"No input data given to write"<<std::endl;
        }else{
            clienter.write_file(file_path,args);
        }
    }
    else if(command == "delete")
    {
        clienter.delete_file(file_path);
    }
    else{
        std::cout<<"Invalid Command"<<std::endl;
    }
   
}





int main(int argc, char const *argv[])
{
    if(argc<3){
        std::cout<<"Usage:" + std::string(argv[0])+ " <command> <file_path> <args>"<<std::endl;
    }

    std::string args;
    for(int index=3 ;index<argc;index++){
        args = args + "|"+ std::string(argv[index]);
    }
    
    RunClient(std::string(argv[1]),std::string(argv[2]),args);
    return 0;
}