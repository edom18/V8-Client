#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include "v8.h"
#include "v8api.h"

void ReadFile(const char* filepath, std::string& result)
{
    std::ifstream ifs(filepath);
    if (!ifs.is_open())
    {
        std::cerr << "Could not open a file [" << filepath << "]" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::stringstream buffer;

    buffer << ifs.rdbuf();

    result = buffer.str();
}

int main(int argc, char* argv[])
{
    std::cout << "Hello World!\n";

    v8_api::Core* core = new v8_api::Core();
    core->Initialize();

    const char* filepath = "test.js";
    std::string source;
    ReadFile(filepath, source);

    const char* src = source.c_str();
    std::unique_ptr<v8_api::Context> context(new v8_api::Context(core->GetIsolat()));
    core->Run(src, context->GetContext());

    v8::HandleScope scope(core->GetIsolat());
    {
        v8::Local<v8::Context> local_context = context->GetContext().Get(core->GetIsolat());
        v8::Local<v8::Object> global = local_context->Global();
        v8::MaybeLocal<v8::Value> m_value = global->Get(local_context, v8::String::NewFromUtf8(core->GetIsolat(), "update").ToLocalChecked());
        v8::Local<v8::Value> value;
        if (!m_value.ToLocal(&value))
        {
            return 0;
        }

        if (!value->IsFunction())
        {
            return 0;
        }

        v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(value);
        v8::Local<v8::Value> js_result = func->ToObject(local_context).ToLocalChecked()->CallAsFunction(local_context, global, 0, nullptr).ToLocalChecked();
        v8::String::Utf8Value utf8result(core->GetIsolat(), js_result);

        std::cout << *utf8result << std::endl;

        v8_api::PositionInfo& info = context->GetInfo();

        std::cout << "updated info.z=" << info.z() << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return 0;
}
