#ifndef _V8Core_Console_HPP
#define _V8Core_Console_HPP

#include "Utils.hpp"

namespace NearTox {

// must be singleton
class V8Console {
 public:
  V8Console(v8::Local<v8::Context>& context, v8::Isolate* isolate);

 private:
  v8::Local<v8::Object> pThis;

  static void log(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void error(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}  // namespace NearTox
#endif  // _V8Core_Console_HPP