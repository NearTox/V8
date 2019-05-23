#include "Console.hpp"

namespace NearTox {

using namespace std::string_view_literals;

V8Console::V8Console(v8::Local<v8::Context>& context, v8::Isolate* isolate) {
  {
    v8::EscapableHandleScope handle_scope(isolate);

    auto raw_t = v8::ObjectTemplate::New(isolate);

    raw_t->SetInternalFieldCount(1);
    raw_t->Set("log"_v8(isolate), v8::FunctionTemplate::New(isolate, log));
    raw_t->Set("error"_v8(isolate), v8::FunctionTemplate::New(isolate, error));

    auto class_t = v8::Local<v8::ObjectTemplate>::New(isolate, raw_t);
    // create instance
    v8::Local<v8::Object> result =
        class_t->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
    // create wrapper for this
    v8::Local<v8::External> ptr = v8::External::New(isolate, this);
    result->SetInternalField(0, ptr);
    pThis = handle_scope.Escape(result);
  }
  if (!pThis.IsEmpty()) { context->Global()->Set(context, "console"_v8(isolate), pThis); }
}

inline void V8Console::log(const v8::FunctionCallbackInfo<v8::Value>& args) {
  bool first = true;
  for (int i = 0; i < args.Length(); i++) {
    v8::HandleScope handle_scope(args.GetIsolate());
    if (first) {
      first = false;
    } else {
      printf(" ");
    }
    v8::String::Utf8Value str(args.GetIsolate(), args[i]);
    printf("%s", ToCString(str));
  }
  printf("\n");
  fflush(stdout);
}

void V8Console::error(const v8::FunctionCallbackInfo<v8::Value>& args) {
  bool first = true;
  for (int i = 0; i < args.Length(); i++) {
    v8::HandleScope handle_scope(args.GetIsolate());
    if (first) {
      first = false;
    } else {
      printf(" ");
    }
    v8::String::Utf8Value str(args.GetIsolate(), args[i]);
    fprintf(stderr, "%s", ToCString(str));
  }
  fprintf(stderr, "\n");
}

}  // namespace NearTox