#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thread>

#include "include/v8.h"
#include "include/libplatform/libplatform.h"

#include "testclass.h"
#include "bindtest.h"

v8::Global<v8::FunctionTemplate> func_;
v8::Global<v8::FunctionTemplate> js_func_;
v8::Global<v8::Context> context_;
v8::Global<v8::Function> process_;

void ReadFile(const char* filePath, std::string& contents)
{
    std::ifstream f(filePath);
    std::stringstream buffer;

    buffer << f.rdbuf();

    contents = buffer.str();
}

static void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() < 1) return;

    v8::Isolate* isolate = args.GetIsolate();
    v8::HandleScope scope(isolate);
    v8::Local<v8::Value> arg = args[0];

    v8::String::Utf8Value value(isolate, arg);
    std::string str(*value);

    std::cout << str;
}

int main(int argc, char* argv[])
{
    std::cout << "Hello World!\n";

    const char* filePath = "test.js";
    std::string fileContents;
    ReadFile(filePath, fileContents);

    v8::V8::InitializeICUDefaultLocation(argv[0]);
    v8::V8::InitializeExternalStartupData(argv[0]);

    std::cout << "----------------\n";

    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();

    v8::V8::InitializePlatform(platform.get());

    v8::V8::Initialize();

    //// Create a new Isolate and make it the current one.
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();

    v8::Isolate* isolate = v8::Isolate::New(create_params);
    {
        v8::Isolate::Scope isolate_scope(isolate);

        // Create a stack-allocated handle scope.
        v8::HandleScope handle_scope(isolate);

        // Create a global object.
        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);

        // Assign "log" function to the global object.
        global->Set(isolate, "log", v8::FunctionTemplate::New(isolate, LogCallback));
        global->Set(v8::String::NewFromUtf8(isolate, "version").ToLocalChecked(), v8::Number::New(isolate, 1.1), v8::ReadOnly);

        // Bind a C++ class to V8 side.
        v8::Local<v8::FunctionTemplate> constructor = v8::FunctionTemplate::New(isolate, JSBindTest::construct);
        constructor->InstanceTemplate()->SetInternalFieldCount(1);
        constructor->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "age").ToLocalChecked(), JSBindTest::get, JSBindTest::set);

        global->Set(v8::String::NewFromUtf8(isolate, "JSBindTest").ToLocalChecked(), constructor, v8::ReadOnly);

        v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global);

        // Enter the context for compiling and running the hello world script.
        v8::Context::Scope context_scope(context);

        {
            //v8::Handle<v8::Object> a = greeter->Wrap(isolate);
            //v8::Local<v8::Object> obj = v8::Local<v8::Object>::New(isolate, a);
            //v8::Local<v8::Object> a = greeter->Wrap(isolate);
            //global->Set(v8::String::NewFromUtf8(isolate, "Greeter").ToLocalChecked(), obj, v8::ReadOnly);

            // Create a string containing the JavaScript source code.
            v8::MaybeLocal<v8::String> m_source = v8::String::NewFromUtf8(isolate, fileContents.c_str());
            v8::Local<v8::String> source = m_source.ToLocalChecked();

            // Compile the source code.
            v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();

            // Run the script to get the result.
            v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

            // Convert the result to an UTF8 string and print it.
            v8::String::Utf8Value utf8(isolate, result);
            printf("%s\n", *utf8);
        }

        //delete greeter;
    }

    std::cout << "Initialized v8!!!!!!!!!!\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    return 0;
}
