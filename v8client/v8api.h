#pragma once

#include <v8.h>

namespace v8_api
{
    class Core
    {
    private:
        v8::Isolate* isolate_;
        v8::Global<v8::Context> impl_;

    public:
        void Initialize();
        void Run(const char* source_code);
    };
}
