#include"master_server.h"

void split_master(std::string str,std::vector<std::string>& strings){
    char *token;
    char *text = (char *)str.c_str();
    token = strtok_r(text, "|", &text);
    while(token) {
        strings.push_back(token);
        token = strtok_r(text, "|" , &text);
    }
}






void meta_data::get_latest_chunk(std::string &file_path,std::string&latest_chunk_handle)
{
    if(this->files.find(file_path)->second.get_chunks().size()>0){
        latest_chunk_handle = this->files.find(file_path)->second.get_chunks().rbegin()->first;
    }
}

void meta_data::get_chunk_locations(const std::string& chunk_handle,std::vector<std::string>& chunk_location)
{
    std::map<std::string,file>::iterator file_it = this->chunkhandle_to_file.find(chunk_handle);
    file f = file_it->second;
    std::map<std::string,chunk&> chunks = f.get_chunks(); 
    std::map<std::string,chunk&>::iterator chunk_it = chunks.find(chunk_handle);
    chunk_location = chunk_it->second.locations;
}

void meta_data::create_new_file(std::string &file_path,std::string& chunk_handle,status_code& s)
{
    
    std::map<std::string,file>::iterator file_it = this->files.find(file_path);
    std::map<std::string,file>::iterator file_end = this->files.end();
    if(file_it != file_end){
        s.value = -1;
        s.exception = std::string("ERROR: File exists already:") + file_path;
        return ;
    }
    this->files.insert(std::pair<std::string,file>(file_path,file(file_path)));
    chunk_handle = "1" + chunk_handle;
    std::string prev_chunk_handle = "-1";
    this->create_new_chunk(file_path,prev_chunk_handle,chunk_handle,s);
}

//to_do
void meta_data::create_new_chunk(std::string &file_path,std::string& prev_chunk_handle,std::string& chunk_handle,status_code& s)
{
    // std::cout<<"create_new_chunk"<<std::endl;
    std::map<std::string,file>::iterator file_it = this->files.find(file_path);
    std::map<std::string,file>::iterator file_end = this->files.end();

    if(file_it == file_end){
        s.value = -2;
        s.exception = std::string("ERROR: New chunk file doesn't exist:") + file_path;
        return ;
    }
    
    std::string latest_chunk = "-2";
    if(prev_chunk_handle != std::to_string(-1)){
        this->get_latest_chunk(file_path,latest_chunk);
    }
    
    if((prev_chunk_handle != std::to_string(-1)) && (latest_chunk != prev_chunk_handle)){
        s.value = -3;
        s.exception = std::string("ERROR: New chunk already created: ")+file_path+std::string(":")+chunk_handle;
        return ;
    }

    chunk* new_chunk =  new chunk();
    file_it->second.get_chunks().insert(std::pair<std::string,chunk&>(chunk_handle,*new_chunk));
    std::map<std::string,chunk&>::iterator  chunk_it =  file_it->second.get_chunks().find(chunk_handle);
    
    
    std::vector<std::string> locations;
    choose_locs(locations);

    for(int index =0;index<locations.size();index++){
        chunk_it->second.locations.push_back(locations[index]);
    }
    
    s.value = 0;
    s.exception = std::string("New Chunk Created");
}

void meta_data::delete_file(std::string &file_path)
{
    std::map<std::string,file>::iterator file_it = this->files.find(file_path);
    this->files.erase(file_it);
}




// void master_server::storage_metadata()
// {
//     std::string file_path = "./metadata";
    
//     std::string data;
//     std::map<std::string,file>::iterator file_begin =  this->metaData.get_files().begin();
//     std::map<std::string,file>::iterator file_end =  this->metaData.get_files().end();
//     std::string file_name;
//     std::string chunk_handle;
//     std::string chunk_locations;
//     while(file_begin!=file_end){
//         file_name = file_begin->first;
        
//         std::map<std::string, chunk &>::iterator chunk_it_begin =  file_begin->second.get_chunks().begin();
//         std::map<std::string, chunk &>::iterator chunk_it_end =  file_begin->second.get_chunks().end();
//         chunk_handle = "";
//         while(chunk_it_begin!=chunk_it_end){
//             chunk_handle = "|chunkhandle|" + chunk_it_begin->first;
//             chunk_locations = "";
//             for(int loc_index =0;loc_index<chunk_it_begin->second.locations.size();loc_index++){
//                 chunk_locations =chunk_locations+ "|"+chunk_it_begin->second.locations[loc_index];
//             }
//             chunk_handle = chunk_handle +"|locations" + chunk_locations;
//             chunk_it_begin++;
//         }
//         data = data+"|filename"+"|"+file_name + chunk_handle;
//         file_begin++;
//     }
//     try
//     {
//         std::ofstream meta_file;
//         meta_file.open(file_path,std::ios::out);
//         meta_file<<data;
//         meta_file.close();
//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << e.what() << '\n';
//     }
// }

// void master_server::read_metadata()
// {
//     std::string file_path = "./metadata";
//     std::string data;
//     try
//     {
//         std::ifstream meta_file;
//         meta_file.open(file_path,std::ios::in);
//         meta_file>>data;
//         meta_file.close();
//     }
//     catch(const std::exception& e)
//     {
//         std::cerr << e.what() << '\n';
//     }

//     std::vector<std::string> strings;
//     split_master(data,strings);
//     std::string content;
//     int index = 0;
//     std::string file_name;
//     std::string chunk_handle;
//     while (index<strings.size())
//     {
        
//     }   

// }


void master_server::get_chunk_handle(std::string & s_uuid)
{
    uuid_t  uuid;
    uuid_generate(uuid);
    char str[256];
    uuid_unparse(uuid, str);
    s_uuid = std::string(str);
}

void master_server::check_valid_file(std::string &file_path,status_code& s)
{
    std::map<std::string,file>::iterator file_it = this->metaData.get_files().find(file_path);
    if(file_it == this->metaData.get_files().end()){
        s.value = -1;
        s.exception = std::string("ERROR: file ") + file_path + std::string("doesn't exist");
    }else{
        s.value = 0;
        s.exception = std::string("SUCCESS: file ") + file_path + std::string("exist");
    }
}

void master_server::list_files(std::string &file_path,std::vector<std::string>& files)
{
    std::map<std::string,file>::iterator file_it;
    std::map<std::string,file>::iterator file_itEnd;
    file_it = this->metaData.get_files().begin();
    file_itEnd = this->metaData.get_files().end();
    while (file_it != file_itEnd){
        if(!file_it->first.compare(0,file_path.size(),file_path)){
            files.push_back(file_it->first);
        }
        file_it++;
    } 
}

void master_server::create_file(std::string &file_path,std::string &chunk_handle,std::vector<std::string> &locations,status_code& s)
{
    this->get_chunk_handle(chunk_handle);
    this->metaData.create_new_file(file_path,chunk_handle,s);

    if(s.value != 0){
        return ;
    }

    std::map<std::string,file>::iterator file_it = this->metaData.get_files().find(file_path);
    file f = file_it->second;
    // std::cout<<f.get_chunks().find(chunk_handle)->first<<std::endl;
    
    locations = f.get_chunks().find(chunk_handle)->second.locations;
}

void master_server::append_file(std::string &file_path,std::string &latest_chunk_handle,std::vector<std::string> &locations,status_code& s)
{
    locations = this->metaData.get_files().find(file_path)->second.get_chunks().find(latest_chunk_handle)->second.locations;
    s.value = 0;
    s.exception = std::string("SUCCESS: file ") + file_path + std::string(" last chunk get");
}

void master_server::create_chunk(std::string &file_path,std::string &prev_chunk_handle,std::vector<std::string> &locations,status_code& s)
{
    int chunk_index = prev_chunk_handle[0];
    std::string new_chunk_handle;
    this->get_chunk_handle(new_chunk_handle);
    new_chunk_handle = std::to_string(chunk_index) + new_chunk_handle;
    chunk* new_c = new chunk();
    choose_locs(locations);
    new_c->locations = locations;
    this->metaData.get_files().find(file_path)->second.get_chunks().insert(std::pair<std::string,chunk&>(new_chunk_handle,*new_c));
    prev_chunk_handle = new_chunk_handle;
    s.value = 0;
    s.exception = std::string("SUCCESS: file ") + file_path + std::string(" create new chunk ");
}

void master_server::read_file(std::string &file_path,std::string& data,status_code& s)
{
    this->check_valid_file(file_path,s);
    if(s.value !=0){
        return ;
    }
    std::map<std::string,chunk&> chunks= this->metaData.get_files().find(file_path)->second.get_chunks();
    std::map<std::string,chunk&>::iterator chunks_it = chunks.begin();
    std::map<std::string,chunk&>::iterator chunks_end = chunks.end();
    
    std::vector<std::string> all_chunk_handles;
    while (chunks_it != chunks_end){
        data = data + "|" + chunks_it->first + "|" + chunks_it->second.locations[0];
        chunks_it++;
    } 
    s.value = 0;
    s.exception = std::string("SUCCESS: file ") + file_path + std::string(" is readed");

}

void master_server::delete_file(std::string &file_path,status_code& s)
{
    this->check_valid_file(file_path,s);
    if(s.value != 0){
        return ;
    }
    
    try
    {
        this->metaData.delete_file(file_path);
    }
    catch(const std::exception& e)
    {
        s.value = -1;
        s.exception = std::string("ERROR: ")+e.what() + std::string("\n");
        return ;
    }

    s.value = 0;
    s.exception = std::string("SUCCESS: file ") + file_path + std::string(" is deleted");
}

void master_server::write_file(std::string &file_path,std::string& data,std::string& chunks,status_code& s)
{
    this->check_valid_file(file_path,s);
    if(s.value != 0){
        return ;
    }

    gfs_config config;
    int chunk_num  = data.size() / config.chunk_size + 1;
    int pre_size = this->metaData.get_files().find(file_path)->second.get_chunks().size();
    std::map<std::string, chunk &>::iterator chunk_begin = this->metaData.get_files().find(file_path)->second.get_chunks().begin();
    std::map<std::string, chunk &>::iterator chunk_end = this->metaData.get_files().find(file_path)->second.get_chunks().end();
    
    if(chunk_num <= pre_size){
        int temp_chunk_num = 0;
        while(temp_chunk_num<chunk_num){
            chunks = chunks + "|" + chunk_begin->first;
            for(int j=0;j<chunk_begin->second.locations.size();j++){
                chunks = chunks +"|"+ chunk_begin->second.locations[j];
            }
            chunk_begin++;
            temp_chunk_num++;
        }

        while(chunk_begin!=chunk_end){
            std::string to_delete = chunk_begin->first;
            chunk_begin++;
            this->metaData.get_files().find(file_path)->second.get_chunks().erase(to_delete);
        }

    }else{
        int add_num = chunk_num - pre_size+1;
        while(chunk_begin!=chunk_end){
            chunks = chunks + "|" + chunk_begin->first;
            for(int j=0;j<chunk_begin->second.locations.size();j++){
                chunks = chunks +"|"+ chunk_begin->second.locations[j];
            }
            chunk_begin++;
        }
        std::string chunk_handle;
        int add_index = 1;
        while(add_index<add_num){
            this->get_chunk_handle(chunk_handle);
            chunk_handle = std::to_string(add_index+pre_size) + chunk_handle;
            chunks = chunks +"|"+ chunk_handle;

            chunk* c = new chunk();
            std::vector<std::string> locations;
            choose_locs(locations);
            c->locations = locations;
            for(int j=0;j<locations.size();j++){
                chunks = chunks +"|"+ locations[j];
            }
            this->metaData.get_files().find(file_path)->second.get_chunks().insert(std::pair<std::string,chunk&>(chunk_handle,*c));
            add_index++;
        }
    }

    s.value=0;
    s.exception = std::string("SUCCESS: file ") + file_path + std::string(" is writed");
}



Status master_server::CreateFile(ServerContext* context,const Request* request ,Reply* reply) 
{
    std::string file_path = request->send_message();
    std::cout<<std::string("Command create ") + file_path<<std::endl;
    std::string  chunk_handle;
    std::vector<std::string> locations;
    status_code s;
    this->create_file(file_path,chunk_handle,locations,s);
    std::cout<<s.exception<<std::endl;
    if(s.value !=0){
        reply->set_reply_message(s.exception);
        return Status::OK;
    }

    std::string m_reply = chunk_handle;
    for(int index =0;index<locations.size();index++){
        m_reply = m_reply +std::string("|")+locations[index];
    }

    reply->set_reply_message(m_reply);
    return Status::OK;
}

Status master_server::DeleteFile(ServerContext* context,const Request* request ,Reply* reply) 
{
    std::string file_path = request->send_message();
    std::cout<<std::string("Command delete ") + file_path<<std::endl;
    status_code s;
    this->delete_file(file_path,s);
    reply->set_reply_message(s.exception);
    return Status::OK;
}

Status master_server::ListFiles(ServerContext* context,const Request* request ,Reply* reply) 
{
    std::string file_path = request->send_message();

    std::cout<<std::string("Command list ") + file_path<<std::endl;
    std::vector<std::string> files;
    this->list_files(file_path,files);
    

    std::string m_reply;
    if(files.size()==0){
        m_reply = file_path + " is not exist";
        reply->set_reply_message(m_reply);
        return Status::OK;
    }
    for(int index =0;index<files.size();index++){
        m_reply = m_reply +std::string("|")+files[index];
    }
    
    reply->set_reply_message(m_reply);
    return Status::OK;
}

Status master_server::ReadFile(ServerContext* context,const Request* request ,Reply* reply) 
{
    std::string file_path = request->send_message();
    std::cout<<std::string("Command read ") + file_path<<std::endl;
    std::string data;
    status_code s;
    this->read_file(file_path,data,s);

    if(s.value !=0){
        reply->set_reply_message(s.exception);
        return Status::OK;
    }

    reply->set_reply_message(data);
    return Status::OK;

}

Status master_server::CreateChunk(ServerContext* context,const Request* request ,Reply* reply) 
{
    std::vector<std::string> strings;
    split_master(request->send_message(),strings);
    std::string file_path = strings[0];
    std::string chunk_handle = strings[1];
    
    std::cout<<std::string("Command create chunk of ") + file_path<<std::endl;
    status_code s;
    std::vector<std::string> locations;
    this->create_chunk(file_path,chunk_handle,locations,s);
    if(s.value !=0){
        reply->set_reply_message(s.exception);
        return Status::OK;
    }

    std::string reply_message = chunk_handle;
    for(int index=0;index<locations.size();index++){
        reply_message = reply_message+"|"+locations[index];
    }
    reply->set_reply_message(reply_message);
    return Status::OK;
}

Status master_server::WriteFile(ServerContext* context,const Request* request ,Reply* reply) 
{
    std::vector<std::string> strings;
    split_master( request->send_message(),strings);
    std::string file_path = strings[0];
    std::string data = strings[1];
    std::cout<<std::string("Command write ")  +data +" to "+ file_path<<std::endl;
    status_code s;
    std::string chunks;
    this->write_file(file_path,data,chunks,s);

    if(s.value !=0){
        reply->set_reply_message(s.exception);
        return Status::OK;
    }
    
    reply->set_reply_message(chunks);
    return Status::OK;
}

Status master_server::AppendFile(ServerContext* context,const Request* request ,Reply* reply) 
{
    std::vector<std::string> strings;
    split_master( request->send_message(),strings);
    std::string file_path = strings[0];
    std::string data = strings[1];
    std::cout<<std::string("Command append ")  + data +" to "+ file_path<<std::endl;
    status_code s;
    std::string last_chunk;
    std::vector<std::string> locations;
    this->metaData.get_latest_chunk(file_path,last_chunk);
    
    this->append_file(file_path,last_chunk,locations,s);


    if(s.value !=0){
        reply->set_reply_message(s.exception);
        return Status::OK;
    }
    

    std::string reply_message = last_chunk;
    for(int index=0;index<locations.size();index++){
        reply_message = reply_message + "|" + locations[index];
    }
    reply->set_reply_message(reply_message);
    return Status::OK;
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
    std::cout << "Master Server listening on " << server_address << std::endl;

    server->Wait();
}


int main(void)
{
    RunMasterServer();
    return 0;
}