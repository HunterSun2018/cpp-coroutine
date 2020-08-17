#include <iostream>
#include <experimental/coroutine>

using namespace std;

int main(int argc, const char *argv[])
{
    return 0;
}

void foo(){
  std::cout << "Hello" << std::endl;
  co_await std::experimental::suspend_always();
  std::cout << "World" << std::endl;
}

std::future<std::expected<std::string>> load_data( std::string resource )
{
  auto handle = co_await open_resouce(resource);
  while( auto line = co_await read_line(handle)) {
    if (std::optional<std::string> r = parse_data_from_line( line ))
       co_return *r;
  }
  co_return std::unexpected( resource_lacks_data(resource) );
}