#pragma once

#include <v8.h>
#include <string>

namespace v8_api
{
    class Core
    {
    private:
        static bool is_initialized_;
        std::unique_ptr<v8::Platform> platform_;
        v8::Isolate* isolate_;
        v8::Global<v8::Context> impl_;
        std::vector<v8::Global<v8::Context>> contexts_;

    public:
        v8::Isolate* GetIsolat();
        void Initialize();
        void Run(const char* source_code, v8::Global<v8::Context>& context);
    };

    class PositionInfo
    {
    private:
        float x_;
        float y_;
        float z_;

    public:
        PositionInfo();
        PositionInfo(float x, float y, float z);
        float x();
        float y();
        float z();

        static void Get(v8::Local<v8::String> property_name, const v8::PropertyCallbackInfo<v8::Value>& args)
        {
            v8::Local<v8::Object> self = args.Holder();
            v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
            void* ptr = wrap->Value();
            PositionInfo* info = static_cast<PositionInfo*>(ptr);

            int length = property_name->Utf8Length(args.GetIsolate()) + 1;
            char* buf = new char[length];
            property_name->WriteUtf8(args.GetIsolate(), buf, length);

            if (strcmp(buf, "x") == 0)
            {
                args.GetReturnValue().Set(info->x_);
            }
            else if (strcmp(buf, "y") == 0)
            {
                args.GetReturnValue().Set(info->y_);
            }
            else if (strcmp(buf, "z") == 0)
            {
                args.GetReturnValue().Set(info->z_);
            }
        }

        static void Set(v8::Local<v8::String> property_name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
        {
            v8::Local<v8::Object> self = args.Holder();
            v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
            void* ptr = wrap->Value();

            PositionInfo* info = static_cast<PositionInfo*>(ptr);

            int length = property_name->Utf8Length(args.GetIsolate()) + 1;
            char* buf = new char[length];
            property_name->WriteUtf8(args.GetIsolate(), buf, length);

            if (strcmp(buf, "x") == 0)
            {
                info->x_ = (float)value->NumberValue(args.GetIsolate()->GetCurrentContext()).ToChecked();
            }
            else if (strcmp(buf, "y") == 0)
            {
                info->y_ = (float)value->NumberValue(args.GetIsolate()->GetCurrentContext()).ToChecked();
            }
            else if (strcmp(buf, "z") == 0)
            {
                info->z_ = (float)value->NumberValue(args.GetIsolate()->GetCurrentContext()).ToChecked();
            }
        }
    };

    class Context
    {
    private:
        v8::Global<v8::Context> context_;
        PositionInfo* info_;

    public:
        Context(v8::Isolate* isolate);

        PositionInfo& GetInfo();

        v8::Global<v8::Context>& GetContext();
    };
}
