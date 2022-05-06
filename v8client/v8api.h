#pragma once

#include <v8.h>

namespace v8_api
{
    class Core
    {
    private:
        std::unique_ptr<v8::Platform> platform_;
        v8::Isolate* isolate_;
        v8::Global<v8::Context> impl_;

    public:
        void Initialize();
        void Run(const char* source_code);
    };
}
