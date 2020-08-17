all : demo awaitable

demo : src/main.cpp
	c++ -std=c++20 -stdlib=libc++ -Wall src/main.cpp -o demo

awaitable : src/awaitable.cpp
	c++ -g -std=c++20 -fcoroutines-ts -stdlib=libc++ -Wall src/awaitable.cpp -lpthread -o awaitable 

clean :
	rm demo awaitable