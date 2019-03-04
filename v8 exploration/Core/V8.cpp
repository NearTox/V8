#include "v8.hpp"

#include "Console.hpp"

#include <libplatform/libplatform.h>

namespace NearTox {
using namespace std::string_view_literals;

// Initialize V8.
V8Core::V8Core(const char* args) {
  v8::V8::InitializeICUDefaultLocation(args);
  v8::V8::InitializeExternalStartupData(args);

  mPlatform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(mPlatform.get());
  v8::V8::Initialize();

  // Create a new Isolate and make it the current one.
  mCreateParams.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
}

V8Core::~V8Core() {
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();

  delete mCreateParams.array_buffer_allocator;
}

V8Core_ptr V8Core::Make(const char* args) { return std::make_unique<V8Core>(args); }

static void Version(const v8::FunctionCallbackInfo<v8::Value>& args) {
  V8StringHelper helper(v8::V8::GetVersion());
  args.GetReturnValue().Set(helper(args.GetIsolate()));
}

v8::Local<v8::Context> V8Context::CreateShellContext(v8::Isolate* isolate) {
  // Create a template for the global object.
  v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
  global->Set("version"_v8(isolate), v8::FunctionTemplate::New(isolate, Version));

  return v8::Context::New(isolate, nullptr, global);
}

}  // namespace NearTox