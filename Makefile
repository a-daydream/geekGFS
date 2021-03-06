LDFLAGS = -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl\
		   -luuid\
		   -lpthread

CXX = g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++11

GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

all: client master_server chunk_server

client: gfs.pb.o gfs.grpc.pb.o client.o
	$(CXX) $^ $(LDFLAGS) -o $@

chunk_server: gfs.pb.o gfs.grpc.pb.o chunk_server.o
	$(CXX) $^ $(LDFLAGS) -o $@

master_server: gfs.pb.o gfs.grpc.pb.o master_server.o
	$(CXX) $^ $(LDFLAGS) -o $@

%.grpc.pb.cc: %.proto
	protoc --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	protoc --cpp_out=. $<

clean:
	rm -rf root_chunkserver/*
	rm -f *.o *.pb.cc *.pb.h client chunk_server master_server 
