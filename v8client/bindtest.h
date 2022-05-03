#pragma once

#include <string>
#include <v8.h>

class JSBindTest
{
public:

    static void construct(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        // Create your c++ object that will follow the JavaScript object around 
        // make sure not to make it on the stack or it won't be around later when you need it.
        JSBindTest* instance = new JSBindTest();

        args.This()->SetInternalField(0, v8::External::New(args.GetIsolate(), instance));
        args.GetReturnValue().Set(args.This());
    }

    static void get(v8::Local<v8::String> property_name, const v8::PropertyCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> self = args.Holder();

        // This is where we take the actual c++ object that was embedded
        //   into the JavaScript object and get it back to a useable c++ object.
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        int value = static_cast<JSBindTest*>(ptr)->age_;
        args.GetReturnValue().Set(value);
    }

    static void set(v8::Local<v8::String> property_name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
    {
        v8::Local<v8::Object> self = args.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        static_cast<JSBindTest*>(ptr)->age_ = value->Int32Value(args.GetIsolate()->GetCurrentContext()).ToChecked();
    }

private:

    const std::string class_name_;
    int age_ = 20;
    v8::Local<v8::FunctionTemplate> class_;
    v8::Local<v8::ObjectTemplate> instance_tmpl_;
    v8::Local<v8::ObjectTemplate> prototype_tmpl_;

    static void destruct(v8::Persistent<v8::Value> handle, void* parameter)
    {
        JSBindTest* instance = static_cast<JSBindTest*>(parameter);
        delete instance;
    }
};
