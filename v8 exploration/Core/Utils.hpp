#ifndef _V8Core_Utils_HPP
#define _V8Core_Utils_HPP

#include <v8.h>
#include <string_view>

namespace NearTox {

static const char* ToCString(const v8::String::Utf8Value& value) {
  return *value != nullptr ? *value : "<string conversion failed>";
}

[[nodiscard]] inline v8::MaybeLocal<v8::String> NewV8String(
    v8::Isolate* isolate, const std::string_view& data) {
  return v8::String::NewFromUtf8(isolate, data.data(), v8::NewStringType::kNormal, data.size());
}

struct V8StringHelper {
  std::string_view data;

  constexpr V8StringHelper(const char* str, size_t len) noexcept : data(str, len) {}
  V8StringHelper(const char* str) noexcept : data(str, std::strlen(str)) {}

  [[nodiscard]] inline v8::Local<v8::String> operator()(v8::Isolate* isolate) {
    return NewV8String(isolate, data).ToLocalChecked();
  }
};

struct V8ScriptRun_t {
  // indicates a autorun script
  constexpr explicit V8ScriptRun_t() noexcept = default;
};

static constexpr V8ScriptRun_t kV8ScriptRun{};

struct V8ScriptHelper {
  std::string_view code;

  constexpr V8ScriptHelper(const char* str, size_t len) noexcept : code(str, len) {}
  V8ScriptHelper(const char* str) noexcept : code(str, std::strlen(str)) {}

  [[nodiscard]] inline v8::MaybeLocal<v8::Script> operator()(
      v8::Local<v8::Context> context, v8::ScriptOrigin* origin = nullptr) {
    auto source = NewV8String(context->GetIsolate(), code).ToLocalChecked();
    // Compile the source code.
    return v8::Script::Compile(context, source, origin);
  }

  [[nodiscard]] inline v8::MaybeLocal<v8::Value> operator()(
      v8::Local<v8::Context> context, V8ScriptRun_t, v8::ScriptOrigin* origin = nullptr) {
    auto source = NewV8String(context->GetIsolate(), code).ToLocalChecked();
    // Compile the source code.
    return v8::Script::Compile(context, source, origin).ToLocalChecked()->Run(context);
  }
};

inline namespace v8_literals {

[[nodiscard]] constexpr V8StringHelper operator"" _v8(const char* _Str, size_t _Len) noexcept {
  return V8StringHelper{_Str, _Len};
}

[[nodiscard]] constexpr V8ScriptHelper operator"" _v8_js(const char* _Str, size_t _Len) noexcept {
  return V8ScriptHelper{_Str, _Len};
}

}  // namespace v8_literals
}  // namespace NearTox
#endif  // _V8Core_Utils_HPP