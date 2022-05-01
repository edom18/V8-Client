#pragma once

#include <v8.h>

class JSObject
{
public:
    JSObject() {}
    ~JSObject() {}

    virtual void BindFunctions(v8::Handle<v8::Object> klass) = 0;

    v8::Handle<v8::Object> Wrap(v8::Isolate* isolate)
    {
        v8::EscapableHandleScope scope(isolate);

        // Set up our JS Prototype
        v8::Persistent<v8::ObjectTemplate> class_template;
        v8::Handle<v8::ObjectTemplate> raw_template = v8::ObjectTemplate::New(isolate);
        raw_template->SetInternalFieldCount(1);

        // Create a JS Instance of the Prototype
        v8::Local<v8::Context> context = isolate->GetCurrentContext();
        v8::MaybeLocal<v8::Object> result = raw_template->NewInstance(context);
        v8::Local<v8::Object> localResult = result.ToLocalChecked();

        // Embed our current object into the JS Instance as an internal field.
        v8::Handle<v8::External> class_ptr = v8::External::New(isolate, this);
        localResult->SetInternalField(0, class_ptr);

        BindFunctions(localResult);

        // Return the JS wrapper for our C++ object
        return scope.Escape(localResult);
    }

    static JSObject* Unwrap(v8::Handle<v8::Object> jsObject)
    {
        // Return the internal field representing this JS wrapper's C++ object
        v8::Handle<v8::External> pointer = v8::Handle<v8::External>::Cast(jsObject->GetInternalField(0));
        return static_cast<JSObject*>(pointer->Value());
    }

protected:
    static void BindFunction(v8::Handle<v8::Object> klass, const char* key, v8::FunctionCallback functionPointer)
    {
        v8::Local<v8::FunctionTemplate> funcTemp = v8::FunctionTemplate::New(klass->GetIsolate(), functionPointer);
        funcTemp->ReadOnlyPrototype();
        v8::Local<v8::Context> context = klass->GetIsolate()->GetCurrentContext();
        klass->Set(context, v8::String::NewFromUtf8(klass->GetIsolate(), key).ToLocalChecked(), funcTemp->GetFunction(klass->GetIsolate()->GetCurrentContext()).ToLocalChecked());
    }
};

class Greeter : public JSObject
{
public:
    Greeter() {}
    ~Greeter() {}

    void greet(std::string name)
    {
        std::cout << "Hello, " << name;
    }

    static void jsGreet(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::HandleScope scope(args.GetIsolate());

        Greeter* greeter = static_cast<Greeter*>(Greeter::Unwrap(args.This()));

        v8::Local<v8::Value> arg0 = args[0];
        v8::Local<v8::Value> argVal = arg0->ToString(args.GetIsolate()->GetCurrentContext()).ToLocalChecked();
        v8::String::Utf8Value nameValue(args.GetIsolate(), argVal);
        greeter->greet(*nameValue);
    }

    virtual void BindFunctions(v8::Handle<v8::Object> klass)
    {
        BindFunction(klass, "greet", Greeter::jsGreet);
    }
};


class JSEdo
{
public:
    int age = 5;
    static void ConstructorHandler(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        std::cout << "In constructor\n";

        v8::Local<v8::ObjectTemplate> tmp = v8::ObjectTemplate::New(args.GetIsolate());

        JSEdo* instance = new JSEdo();
        v8::Local<v8::Object> localObj = tmp->NewInstance(args.GetIsolate()->GetCurrentContext()).ToLocalChecked();
        //localObj->SetInternalField(0);
        v8::Persistent<v8::Object> obj(args.GetIsolate(), localObj);
        //obj.SetWeak<JSEdo>(null, )
        v8::ReturnValue<v8::Value> retVal = args.GetReturnValue();
        retVal.Set(obj.Get(args.GetIsolate()));
    }
};
