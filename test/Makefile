.PHONY:mqexchangetest
mqexchangetest:mqexchangetest.cpp
	g++ $^ -o $@ -std=c++17 -lgtest -lprotobuf -lsqlite3

mqfiletest:mqfiletest.cpp
	g++ $^ -o $@ -std=c++17
.PHONY:clean
clean:
	rm mqfiletest dir  mqexchangetest  data -rf