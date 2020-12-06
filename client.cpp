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

    std::vector<std::string> chunk_handle_and_locations;
    split2(master_reply,chunk_handle_and_locations);

    std::string chunk_handle;
    std::string location;

    std::string data;
    int chunk_num = chunk_handle_and_locations.size();
    for(int index =0;index<chunk_num;index+=2){
        chunk_handle = chunk_handle_and_locations[index];
        location =  chunk_handle_and_locations[index+1];
        this->set_chunkserver_stub_(grpc::CreateChannel("localhost:"+location,grpc::InsecureChannelCredentials()));
        std::string chunk_reply = this->Read(chunk_handle);
        std::cout << "Response from chunkserver: " << chunk_reply << std::endl;
        data = data + chunk_reply;
    }
    std::cout<<file_path+":"<<data<<std::endl;
    
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

    std::vector<std::string> ports;
    std::string chunk_handle;
    std::string chunk_data_towrite;
    int size = chunkhandle_locations.size();
    int index = 0;
    while(index<size){
        if(index%4 == 0){
            int send_data_start = (index/4)*64;
            int send_data_size = 64;
            chunk_handle = chunkhandle_locations[index];
            ports.push_back(chunkhandle_locations[++index]);
            ports.push_back(chunkhandle_locations[++index]);
            ports.push_back(chunkhandle_locations[++index]);
            for(int port_num=0;port_num<ports.size();port_num++){
                this->set_chunkserver_stub_(grpc::CreateChannel("localhost:"+ports[port_num],grpc::InsecureChannelCredentials()));
                std::string send_data = chunk_handle + "|"+ data.substr(send_data_start,send_data_size);
                std::string chunk_reply = this->Write(send_data);
                std::cout << "Response from chunkserver: " << chunk_reply << std::endl;
            }
            ports.clear();
        }
        ++index;
    }

}

void client::append_file(const std::string file_path,const std::string data)
{
    std::string master_reply = this->AppendFile(file_path+"|"+data);
    std::cout << "Response from masterserver: " << master_reply << std::endl;

    std::string prefix("ERROR");
    if(!master_reply.compare(0,prefix.size(),prefix)){
        return ;
    }
    
    int data_size = data.size();
    std::vector<std::string> last_chunk_and_locations;
    split2(master_reply,last_chunk_and_locations);
    
    std::string chunk_handle = last_chunk_and_locations[0];
    
    this->set_chunkserver_stub_(grpc::CreateChannel("localhost:"+last_chunk_and_locations[1],grpc::InsecureChannelCredentials()));
    std::string rem_ssize = this->GetChunkSpace(chunk_handle);
    std::cout<<rem_ssize << std::endl;
    int rem_size = 64 - std::stoi(rem_ssize);
    if(data_size <=rem_size){
        for(int index =1;index<last_chunk_and_locations.size();index++){
            this->set_chunkserver_stub_(grpc::CreateChannel("localhost:"+last_chunk_and_locations[index],grpc::InsecureChannelCredentials()));
            std::string chunk_reply =  this->Append(chunk_handle +"|" + data);
            std::cout << "Response from chunkserver: " << master_reply << std::endl;
        } 
    }else{
            std::string data1 = data.substr(0,rem_size);
            for(int index =1;index<last_chunk_and_locations.size();index++){
            this->set_chunkserver_stub_(grpc::CreateChannel("localhost:"+last_chunk_and_locations[index],grpc::InsecureChannelCredentials()));
            std::string chunk_reply =  this->Append(chunk_handle +"|" + data1);
            std::cout << "Response from chunkserver: " << master_reply << std::endl;
            } 
            std::string send_message = file_path + "|" + chunk_handle;
            std::string add_chunk_handle_and_locations = this->CreateChunk(send_message);
            std::vector<std::string> strings;
            split2(add_chunk_handle_and_locations,strings);
            std::string data2 = data.substr(rem_size);
            for(int j=1;j<strings.size();j++){
                this->set_chunkserver_stub_(grpc::CreateChannel("localhost:"+strings[j],grpc::InsecureChannelCredentials()));
                std::string chunk_reply =  this->Append(strings[0] + "|" + data2);
                std::cout << "Response from chunkserver: " << master_reply << std::endl;
            }
    }
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

std::string client::CreateChunk(const std::string & request)
{
    Request master_request;
    master_request.set_send_message(request);

    Reply master_reply;

    ClientContext context;

    Status status = this->master_stub_->CreateChunk(&context, master_request, &master_reply);

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
    // this send_message is a file path and data
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
    // this send_message is a file path and data
    Request master_request;
    master_request.set_send_message(request);

    Reply master_reply;

    ClientContext context;

    Status status = this->master_stub_->AppendFile(&context, master_request, &master_reply);

    if(status.ok()){
        return master_reply.reply_message();
    }else{
        std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
        return "RPC failed for master server";
    }
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
    Request chunkserver_request;
    chunkserver_request.set_send_message(request);

    Reply chunkserver_reply;

    ClientContext context;
    Status status = this->chunkserver_stub_->GetChunkSpace(&context, chunkserver_request, &chunkserver_reply);
    if(status.ok()){
        return chunkserver_reply.reply_message();
    }else{
        std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
        return "RPC failed for clientserver ";
    }
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
    Request chunkserver_request;
    chunkserver_request.set_send_message(request);

    Reply chunkserver_reply;

    ClientContext context;
    Status status = this->chunkserver_stub_->Append(&context, chunkserver_request, &chunkserver_reply);
    if(status.ok()){
        return chunkserver_reply.reply_message();
    }else{
        std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
        return "RPC failed for clientserver ";
    }
}

std::string client::Read(const std::string & request)
{
    Request chunkserver_request;
    chunkserver_request.set_send_message(request);

    Reply chunkserver_reply;

    ClientContext context;
    Status status = this->chunkserver_stub_->Read(&context, chunkserver_request, &chunkserver_reply);
    if(status.ok()){
        return chunkserver_reply.reply_message();
    }else{
        std::cout << status.error_code() << ": " << status.error_message()<< std::endl;
        return "RPC failed for clientserver ";
    }
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