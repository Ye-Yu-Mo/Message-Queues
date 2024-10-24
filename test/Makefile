.PHONY:mqvhostmanagertest
CFLAG= -I../third/muduo/include/
LFALG= -L../third/muduo/lib -lgtest -lsqlite3 -lprotobuf  -pthread -lmuduo_net -lmuduo_base -lz
mqvhostmanagertest:mqvhostmanagertest.cpp ../common/msg.pb.cc ../common/protocol.pb.cc ../third/muduo/include/muduo/protobuf/codec.cc
	g++ $(CFLAG) $^ -o $@ -std=c++17 $(LFALG)

.PHONY:clean
clean:
	rm mqvhostmanagertest ./data -rf