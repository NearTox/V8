#ifndef _V8Core_V8_HPP
#define _V8Core_V8_HPP

#include "Utils.hpp"

namespace NearTox {
class V8Core;
using V8Core_ptr = std::unique_ptr<V8Core>;

class V8Isolate;
using V8Isolate_ptr = std::unique_ptr<V8Isolate>;

// must be singleton (no copy, no move allowed)
class V8Core {
 public:
  // Initialize V8.
  V8Core(const char* args);
  V8Core(V8Core&) = delete;
  V8Core(V8Core&&) = delete;

  // tear down V8 on destroy
  ~V8Core();
  static V8Core_ptr Make(const char* args);

  V8Core& operator=(V8Core&) = delete;
  V8Core& operator=(V8Core&&) = delete;

  v8::Platform* gPlatform() { return mPlatform.get(); }

 private:
  std::unique_ptr<v8::Platform> mPlatform;
  v8::Isolate::CreateParams mCreateParams;

  friend class V8Isolate;
};

class V8Isolate {
 public:
  V8Isolate(V8Core& v8Core) : pIsolate(v8::Isolate::New(v8Core.mCreateParams)) {}
  ~V8Isolate() {
    if (pIsolate) { pIsolate->Dispose(); }
  }
  static V8Isolate_ptr Make(V8Core& v8Core) {
    auto out = std::make_unique<V8Isolate>(v8Core);
    if (out.get() == nullptr || out->Get() == nullptr) { out.reset(); }
    return out;
  }

  [[nodiscard]] inline v8::Isolate* Get() noexcept { return pIsolate; }

 private:
  v8::Isolate* pIsolate = nullptr;
};

class V8Context {
 public:
  V8Context(v8::Isolate* isolate) :
      isolate_scope(isolate),
      handle_scope(isolate),
      context(CreateShellContext(isolate)) {}

  [[nodiscard]] inline v8::Local<v8::Context>& Get() noexcept { return context; }
  [[nodiscard]] inline v8::Local<v8::Context>& operator->() noexcept { return context; }

 private:
  v8::Isolate::Scope isolate_scope;
  // Create a stack-allocated handle scope.
  v8::HandleScope handle_scope;
  v8::Local<v8::Context> context;

  v8::Local<v8::Context> CreateShellContext(v8::Isolate* isolate);
};

}  // namespace NearTox
#endif  // _V8Core_V8_HPP