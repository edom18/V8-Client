#include <iostream>

#include "include/v8.h"
#include "include/libplatform/libplatform.h"
#include "v8api.h"

static std::unique_ptr<v8::Platform> s_platform;
static bool s_is_initialized = false;

namespace v8_api
{
    bool Core::is_initialized_ = false;

    v8::Isolate* Core::GetIsolat()
    {
        return isolate_;
    }

    void Core::Initialize()
    {
        std::cout << "Initializing v8_api::Core\n";

        if (!s_is_initialized)
        {
            v8::V8::InitializeICU();

            platform_ = v8::platform::NewDefaultPlatform();
            v8::V8::InitializePlatform(platform_.get());
            v8::V8::Initialize();

            s_is_initialized = true;
        }

        v8::Isolate::CreateParams create_params;
        create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
        isolate_ = v8::Isolate::New(create_params);

        v8::Isolate::Scope isolate_scope(isolate_);
        v8::HandleScope scope(isolate_);
        v8::Local<v8::Context> impl = v8::Context::New(isolate_);
        impl_.Reset(isolate_, impl);
    }

    void Core::Run(const char* source_code, v8::Global<v8::Context>& gcontext)
    {
        v8::HandleScope handle_scope(isolate_);
        {
            //v8::Local<v8::Context> context = v8::Context::New(isolate_);
            v8::Local<v8::Context> context = gcontext.Get(isolate_);
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

                v8::MaybeLocal<v8::Value> m_result = script->Run(isolate_->GetCurrentContext());
                v8::Local<v8::Value> result;
                if (!m_result.ToLocal(&result))
                {
                    return;
                }

                v8::String::Utf8Value resultStr(isolate_, result);
                std::cout << *resultStr << std::endl;
            }
        }
    }

    Context::Context(v8::Isolate* isolate)
    {
        v8::HandleScope score(isolate);
        {
            // Create a global object for this context.
            v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);

            // Create a context.
            v8::Local<v8::Context> context = v8::Context::New(isolate, NULL, global);
            context_.Reset(isolate, context);

            // Create an API object.
            v8::Local<v8::ObjectTemplate> api_t = v8::ObjectTemplate::New(isolate);

            // Create a PositionInfo object that is accessed like "api.position.x".
            v8::Local<v8::ObjectTemplate> pos_t = v8::ObjectTemplate::New(isolate);
            pos_t->SetInternalFieldCount(1);

            // Set all properties up.
            v8::Local<v8::String> prop_x = v8::String::NewFromUtf8(isolate, "x").ToLocalChecked();
            pos_t->SetAccessor(prop_x, PositionInfo::Get, PositionInfo::Set);

            v8::Local<v8::String> prop_y = v8::String::NewFromUtf8(isolate, "y").ToLocalChecked();
            pos_t->SetAccessor(prop_y, PositionInfo::Get, PositionInfo::Set);

            v8::Local<v8::String> prop_z = v8::String::NewFromUtf8(isolate, "z").ToLocalChecked();
            pos_t->SetAccessor(prop_z, PositionInfo::Get, PositionInfo::Set);

            // Store a C++ instance.
            info_ = new PositionInfo(1.0f, 2.0f, 3.0f);

            v8::Local<v8::Object> pos_o = pos_t->NewInstance(context).ToLocalChecked();
            pos_o->SetInternalField(0, v8::External::New(isolate, info_));

            // Set the PositionInfo up to the API object.
            v8::Local<v8::Object> api_o = api_t->NewInstance(context).ToLocalChecked();
            api_o->Set(context, v8::String::NewFromUtf8(isolate, "position").ToLocalChecked(), pos_o);

            // Set the API object up to the global object.
            context->Global()->Set(context, v8::String::NewFromUtf8(isolate, "api").ToLocalChecked(), api_o);
        }
    }

    PositionInfo& Context::GetInfo()
    {
        return *info_;
    }

    v8::Global<v8::Context>& Context::GetContext()
    {
        return context_;
    }

    PositionInfo::PositionInfo() : x_(0), y_(0), z_(0) {}
    PositionInfo::PositionInfo(float x, float y, float z) : x_(x), y_(y), z_(z) {}
    float PositionInfo::x() { return x_; }
    float PositionInfo::y() { return y_; }
    float PositionInfo::z() { return z_; }
}
