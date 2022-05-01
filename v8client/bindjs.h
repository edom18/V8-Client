#pragma once

#include <string>
#include <v8.h>

class JSExportTest
{
public:
    JSExportTest()
    {
        class_ = v8::FunctionTemplate::New(NULL, JSExportTest::construct);
        class_->SetClassName(v8::String::NewFromUtf8(NULL, "JSExportTest").ToLocalChecked());
        prototype_tmpl_ = class_->PrototypeTemplate();
        instance_tmpl_ = class_->InstanceTemplate();
        instance_tmpl_->SetInternalFieldCount(1);
        instance_tmpl_->SetAccessor(v8::String::NewFromUtf8(NULL, "age").ToLocalChecked(), JSExportTest::get, JSExportTest::set);
    }

    v8::Local<v8::FunctionTemplate> get_class() const
    {
        return class_;
    }

private:
    const std::string class_name_;
    int age_ = 20;
    v8::Local<v8::FunctionTemplate> class_;
    v8::Local<v8::ObjectTemplate> instance_tmpl_;
    v8::Local<v8::ObjectTemplate> prototype_tmpl_;

    static void get(v8::Local<v8::String> property_name, const v8::PropertyCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Value> a = args.Holder()->GetInternalField(0);
        v8::External* b = v8::External::Cast(*a);
        JSExportTest* instance = static_cast<JSExportTest*>(b->Value());
        args.GetReturnValue().Set(instance->age_);
    }

    static void set(v8::Local<v8::String> property_name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
    {
        v8::Local<v8::Value> a = args.Holder()->GetInternalField(0);
        v8::External* b = v8::External::Cast(*a);
        JSExportTest* instance = static_cast<JSExportTest*>(b->Value());
        instance->age_ = 100; // value.Int32Value(args.GetIsolate()->GetCurrentContext());
    }

    static void construct(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        JSExportTest* instance = new JSExportTest();
        v8::Local<v8::Object> this_obj = args.This();
        this_obj->SetInternalField(0, v8::External::New(args.GetIsolate(), instance));
        v8::Persistent<v8::Object> holder(args.GetIsolate(), this_obj);
        //holder.SetWeak(instance, JSExportTest::destruct);
        args.GetReturnValue().Set(this_obj);
    }

    static void destruct(v8::Persistent<v8::Value> handle, void* parameter)
    {
        JSExportTest* instance = static_cast<JSExportTest*>(parameter);
        delete instance;
    }
};
