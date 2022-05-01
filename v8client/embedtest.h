/*
 * v8_wrap_class.cpp
 *
 *  Created on: 14/01/2013
 *      Author: nicholas
 *     License: public domain
 */

/* CMakeLists.txt
project(v8_wrap_class)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")
add_executable(v8_wrap_class v8_wrap_class.cpp)
target_link_libraries(v8_wrap_class v8)
*/

#include <v8.h>
#include <cstdio>
#include <string>
#include <stdexcept>
#include <memory>

using namespace v8;

/*
var Simple = function(v)
{
	this.value = v;
}
Simple.prototype.func = function()
{
	alert(this.value);
}

var obj = new Simple(4);
obj.func();
*/
struct Simple
{
	double value;
	
	Simple(double v)
	 : value(v)
	{
		fprintf(stderr, "Simple::ctor\n");
	}
	
	void func()
	{
		fprintf(stderr, "Simple::func(%f)\n", value);
	}
	
	~Simple()
	{
		fprintf(stderr, "Simple::dtor\n");
	}
};

namespace js
{

/*
 * Retrieve the c++ object pointer from the js object
 */
template <typename T>
T* unwrap(const Arguments& args)
{
	auto self = args.Holder();
	auto wrap = Local<External>::Cast(self->GetInternalField(0));
	return static_cast<T*>(wrap->Value());
}

/*
 * Construct a new c++ object and wrap it in a js object
 */
template <typename T, typename... Args>
Persistent<Object> make_object(Handle<Object> object, Args&&... args)
{
	auto x = new T(std::forward<Args>(args)...);
	auto obj = Persistent<Object>::New(object);
	obj->SetInternalField(0, External::New(x));

	obj.MakeWeak(x, [](Persistent<Value> obj, void* data)
	{
		auto x = static_cast<T*>(data);
		delete x;

		obj.Dispose();
		obj.Clear();
	});

	return obj;
}

}

void bind_Simple(Local<Object> global)
{
	// Name the class in js
	auto name = String::NewSymbol("Simple");
	
	auto tpl = FunctionTemplate::New([&](const Arguments& args) -> Handle<Value>
	{
		if (!args.IsConstructCall())
			return ThrowException(String::New("Cannot call constructor as function"));

		HandleScope scope;

		// Read and pass constructor arguments
		js::make_object<Simple>(args.This(), args[0]->NumberValue());

		return args.This();
	});
	
	tpl->SetClassName(name);
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	
	auto prototype = tpl->PrototypeTemplate();
	
	// Add object properties to the prototype
	// Methods, Properties, etc.
	prototype->Set(String::New("func"), FunctionTemplate::New([](const Arguments& args) -> Handle<Value>
	{
		auto s = js::unwrap<Simple>(args);
		s->func();
		return {};
	})->GetFunction());
	
	auto constructor = Persistent<Function>::New(tpl->GetFunction());
	global->Set(name, constructor);
}

int main()
{
	std::string js_source = R"(
		for(var i = 0; i < 1000; ++i)
		{
			var s = new Simple(4);
			s.value = 5;
			s.func();
		}
	)";
	
	/*
	 * This code is mostly uninteresting.
	 * Just run the vm with the script provided.
	 */
	{
		HandleScope handle_scope;
		Handle<ObjectTemplate> global_template = ObjectTemplate::New();

		Persistent<Context> context = Context::New(0, global_template);
		Context::Scope context_scope(context);

		auto global = context->Global();

		// Wrap the class and bind to the global scope.
		bind_Simple(global);
		
		{
			HandleScope handle_scope;

			TryCatch trycatch;

			Local<String> source = String::New(js_source.c_str(), js_source.size());

			Local<Script> script = Script::Compile(source);
			if (script.IsEmpty())
			{
				Handle<Value> exception = trycatch.Exception();
				String::AsciiValue exception_str(exception);
				throw std::runtime_error(*exception_str);
			}

			Local<Value> result = script->Run();
			if (result.IsEmpty())
			{
				Local<Value> exception = trycatch.Exception();
				String::AsciiValue exception_str(exception);
				throw std::runtime_error(*exception_str);
			}
		}

		context.Dispose();
		context.Clear();
	}

	// Run the GC until there is nothing to reclaim.
	while (!V8::IdleNotification())
		;
	return 0;
}

