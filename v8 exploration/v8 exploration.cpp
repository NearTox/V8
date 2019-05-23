#include "resource.h"

#include <SDKDDKVer.h>
#include <windows.h>

#include "Core/v8.hpp"

#include "Core/Console.hpp"

#include "include/libplatform/libplatform.h"
#include "include/v8-inspector.h"

#include <cassert>

using namespace std::string_view_literals;
using namespace std::string_literals;

void ReportException(v8::Isolate* isolate, v8::TryCatch* try_catch) {
  v8::HandleScope handle_scope(isolate);
  v8::String::Utf8Value exception(isolate, try_catch->Exception());
  const char* exception_string = NearTox::ToCString(exception);
  v8::Local<v8::Message> message = try_catch->Message();
  if (message.IsEmpty()) {
    // V8 didn't provide any extra information about this error; just
    // print the exception.
    fprintf(stderr, "%s\n", exception_string);
  } else {
    // Print (filename):(line number): (message).
    v8::String::Utf8Value filename(isolate, message->GetScriptOrigin().ResourceName());
    v8::Local<v8::Context> context(isolate->GetCurrentContext());
    const char* filename_string = NearTox::ToCString(filename);
    int linenum = message->GetLineNumber(context).FromJust();
    fprintf(stderr, "%s:%i: %s\n", filename_string, linenum, exception_string);
    // Print line of source code.
    v8::String::Utf8Value sourceline(isolate, message->GetSourceLine(context).ToLocalChecked());
    const char* sourceline_string = NearTox::ToCString(sourceline);
    fprintf(stderr, "%s\n", sourceline_string);
    // Print wavy underline (GetUnderline is deprecated).
    int start = message->GetStartColumn(context).FromJust();
    for (int i = 0; i < start; i++) { fprintf(stderr, " "); }
    int end = message->GetEndColumn(context).FromJust();
    for (int i = start; i < end; i++) { fprintf(stderr, "^"); }
    fprintf(stderr, "\n");
    v8::Local<v8::Value> stack_trace_string;
    if (try_catch->StackTrace(context).ToLocal(&stack_trace_string) &&
        stack_trace_string->IsString() &&
        v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0) {
      v8::String::Utf8Value stack_trace(isolate, stack_trace_string);
      const char* stack_trace_string = NearTox::ToCString(stack_trace);
      fprintf(stderr, "%s\n", stack_trace_string);
    }
  }
}

int main(int argc, char* argv[]) {
  using namespace NearTox;

  // Initialize V8.
  V8Core_ptr v8core = V8Core::Make(argv[0]);

  // new isolate
  V8Isolate_ptr _isolate = V8Isolate::Make(*v8core);
  if (_isolate != nullptr) {
    auto isolate = _isolate->Get();
    V8Context context(isolate);
    if (context.Get().IsEmpty()) {
      fprintf(stderr, "Error creating context\n");
      return 1;
    }

    // Enter the context for compiling and running the hello world script.
    v8::Context::Scope context_scope(context.Get());
    V8Console console(context.Get(), isolate);

    {
      // Compile the source code with a string containing the JavaScript source code
      // and get the result.
      v8::Local<v8::Value> result =
          /*"'Hello' + ', World!'"*/ "Object.keys(this)"_v8_js(context.Get(), kV8ScriptRun)
              .ToLocalChecked();

      // Convert the result to an UTF8 string and print it.
      v8::String::Utf8Value utf8(isolate, result);
      printf("%s\n", *utf8);
    }

    {
      v8::TryCatch try_catch(isolate);
      v8::ScriptOrigin origin(" (shell) "_v8(isolate));
      v8::Local<v8::Value> result;
      auto script = "Verison()"_v8_js(context.Get(), kV8ScriptRun, &origin);

      if (!script.ToLocal(&result)) {
        assert(try_catch.HasCaught());
        ReportException(isolate, &try_catch);
      } else {
        assert(!try_catch.HasCaught());
        // Convert the result to an UTF8 string and print it.
        v8::String::Utf8Value utf8(isolate, result);
        printf("%s\n", *utf8);
      }
    }
    {
      // Compile the source code with a string containing the JavaScript source code.
      // Use the JavaScript API to generate a WebAssembly module.
      //
      // |bytes| contains the binary format for the following module:
      //
      //     (func (export "add") (param i32 i32) (result i32)
      //       get_local 0
      //       get_local 1
      //       i32.add)
      //
      // and get the result.
      v8::Local<v8::Value> result =
          "let bytes = new Uint8Array(["
          "  0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x07, 0x01,"
          "  0x60, 0x02, 0x7f, 0x7f, 0x01, 0x7f, 0x03, 0x02, 0x01, 0x00, 0x07,"
          "  0x07, 0x01, 0x03, 0x61, 0x64, 0x64, 0x00, 0x00, 0x0a, 0x09, 0x01,"
          "  0x07, 0x00, 0x20, 0x00, 0x20, 0x01, 0x6a, 0x0b"
          "]);"
          "let module = new WebAssembly.Module(bytes);"
          "let instance = new WebAssembly.Instance(module);"
          "instance.exports.add(3, 4);"_v8_js(context.Get(), kV8ScriptRun)
              .ToLocalChecked();

      // Convert the result to a uint32 and print it.
      uint32_t number = result->Uint32Value(context.Get()).ToChecked();
      printf("3 + 4 = %u\n", number);
    }
  }

  return 0;
}
