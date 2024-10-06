.PHONY:mqmessagetest
mqmessagetest:mqmessagetest.cpp ../common/msg.pb.cc
	g++ $^ -o $@ -std=c++17 -lgtest -lsqlite3 -lprotobuf

mqbindingtest:mqbindingtest.cpp
	g++ $^ -o $@ -std=c++17 -lgtest -lprotobuf -lsqlite3

mqqueuetest:mqqueuetest.cpp
	g++ $^ -o $@ -std=c++17 -lgtest -lprotobuf -lsqlite3

mqexchangetest:mqexchangetest.cpp
	g++ $^ -o $@ -std=c++17 -lgtest -lprotobuf -lsqlite3

mqfiletest:mqfiletest.cpp
	g++ $^ -o $@ -std=c++17
.PHONY:clean
clean:
	rm mqfiletest dir  mqexchangetest mqmessagetest data -rf