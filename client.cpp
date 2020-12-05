#include"client.h"

void split2(std::string str,std::vector<std::string>& strings){
    char *token;
    char *text = (char *)str.c_str();
    token = strtok_r(text, "|", &text);
    while(token) {
        strings.push_back(token);
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
    split2(master_reply,data);

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
    std::string master_reply = this->ReadFile(file_path);
    std::cout << "Response from masterserver: " << master_reply << std::endl;

    std::string prefix("ERROR");
    if(!master_reply.compare(0,prefix.size(),prefix)){
        return ;
    }

}

void client::write_file(const std::string file_path,const std::string data)
{
    std::string master_reply = this->WriteFile(file_path+"|"+data);
    std::cout << "Response from masterserver: " << master_reply << std::endl;

    std::string prefix("ERROR");
    if(!master_reply.compare(0,prefix.size(),prefix)){
        return ;
    }

    std::vector<std::string> chunkhandle_locations;
    split2(master_reply,chunkhandle_locations);

    std::cout<<"write_file " <<chunkhandle_locations.size()<<std::endl;
    std::vector<std::string> ports;
    std::string chunk_handle;
    std::string chunk_data_towrite;
    int size = chunkhandle_locations.size();
    int index;
    for(index =0;index<size;index++){
        if(index%4 == 0){
            chunk_handle = chunkhandle_locations[index];
            for(int port_num=0;port_num<ports.size();port_num++){
                this->set_chunkserver_stub_(grpc::CreateChannel("localhost:"+ports[port_num],grpc::InsecureChannelCredentials()));
                int send_data_start = (index/4)*64;
                int send_data_end = send_data_start + 63;
                if(send_data_end > data.size()) send_data_end = data.size();
                std::string send_data = chunk_handle + "|"+ data.substr(send_data_start,send_data_end);
                std::string chunk_reply = this->Write(send_data);
                std::cout << "Response from chunkserver: " << master_reply << std::endl;
            }
            ports.clear();
            continue;
        }
        ports.push_back(chunkhandle_locations[index]);
    }

    for(int port_num=0;port_num<ports.size();port_num++){
        this->set_chunkserver_stub_(grpc::CreateChannel("localhost:"+ports[port_num],grpc::InsecureChannelCredentials()));
        int send_data_start = ((index-1)/4)*64;
        int send_data_end = send_data_start + 63;
        if(send_data_end > data.size()) send_data_end = data.size();
        std::string send_data = chunk_handle + "|"+ data.substr(send_data_start,send_data_end);
        std::string chunk_reply = this->Write(send_data);
        std::cout << "Response from chunkserver: " << master_reply << std::endl;
    }

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
    // this send_message is a file path
    Request master_request;
    master_request.set_send_message(request);

    Reply master_reply;

    ClientContext context;

    Status status = this->master_stub_->ReadFile(&context, master_request, &master_reply);

    if(status.ok()){
        return master_reply.reply_message();
    }else{
        std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
        return "RPC failed for master server";
    }
}

std::string client::WriteFile(const std::string & request)
{
    // this send_message is a file path
    Request master_request;
    master_request.set_send_message(request);

    Reply master_reply;

    ClientContext context;

    Status status = this->master_stub_->WriteFile(&context, master_request, &master_reply);

    if(status.ok()){
        return master_reply.reply_message();
    }else{
        std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
        return "RPC failed for master server";
    }
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
        Request chunkserver_request;
        chunkserver_request.set_send_message(request);

        Reply chunkserver_reply;

        ClientContext context;
        Status status = this->chunkserver_stub_->Write(&context, chunkserver_request, &chunkserver_reply);
        if(status.ok()){
            return chunkserver_reply.reply_message();
        }else{
            std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
            return "RPC failed for clientserver ";
        }
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