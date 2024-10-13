.PHONY:all
all:publish_client consume_client
CFLAG= -I../third/muduo/include/
LFALG= -L../third/muduo/lib -lgtest -lsqlite3 -lprotobuf  -pthread -lmuduo_net -lmuduo_base -lz
publish_client:publish_client.cc ../common/msg.pb.cc ../common/protocol.pb.cc ../third/muduo/include/muduo/protobuf/codec.cc
	g++ -g $(CFLAG) $^ -o $@ -std=c++17 $(LFALG)

consume_client:consume_client.cc ../common/msg.pb.cc ../common/protocol.pb.cc ../third/muduo/include/muduo/protobuf/codec.cc
	g++ -g $(CFLAG) $^ -o $@ -std=c++17 $(LFALG)

.PHONY:clean
clean:
	rm publish_client consume_client -rf