#include <iostream>

#include "include/v8.h"
#include "include/libplatform/libplatform.h"
#include "v8api.h"

namespace v8_api
{
    void Core::Initialize()
    {
        std::cout << "Initializing v8_api::Core\n";

        v8::V8::InitializeICU();

        std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
        v8::V8::InitializePlatform(platform.get());
        v8::V8::Initialize();

        v8::Isolate::CreateParams create_params;
        create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
        isolate_ = v8::Isolate::New(create_params);

        v8::Isolate::Scope isolate_scope(isolate_);
        v8::HandleScope scope(isolate_);
        v8::Local<v8::Context> impl = v8::Context::New(isolate_);
        impl_.Reset(isolate_, impl);

        // 以下を追加するとなぜか動く
        v8::HandleScope handle_scope(isolate_);
        {
            const char* src = "var a = 4;";
            v8::MaybeLocal<v8::String> m_source = v8::String::NewFromUtf8(isolate_, src);
            v8::Local<v8::String> source = m_source.ToLocalChecked();
            v8::MaybeLocal<v8::Script> m_script = v8::Script::Compile(impl, source);
        }
    }

    void Core::Run(const char* source_code)
    {
        v8::HandleScope handle_scope(isolate_);
        {
            v8::Local<v8::Context> context = v8::Context::New(isolate_);
            v8::Context::Scope context_scope(context);
            {
                v8::MaybeLocal<v8::String> m_source = v8::String::NewFromUtf8(isolate_, source_code);
                v8::Local<v8::String> source = m_source.ToLocalChecked();
                v8::MaybeLocal<v8::Script> m_script = v8::Script::Compile(context, source);
                v8::Local<v8::Script> script;
                if (!v8::Script::Compile(context, source).ToLocal(&script))
                {
                    return;
                }
                v8::Local<v8::Value> result = script->Run(isolate_->GetCurrentContext()).ToLocalChecked();
                v8::String::Utf8Value resultStr(isolate_, result);
                std::cout << *resultStr;
            }
        }
    }
}
