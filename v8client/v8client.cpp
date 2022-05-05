#include <iostream>
#include <thread>
#include "v8.h"
#include "v8api.h"

int main(int argc, char* argv[])
{
    std::cout << "Hello World!\n";

    v8_api::Core* core = new v8_api::Core();
    core->Initialize();

    const char* src = "var a = 4;";
    core->Run(src);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return 0;
}
