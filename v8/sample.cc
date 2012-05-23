#include <v8.h>
#include <sys/mman.h>
#include <string.h>

using namespace v8;

Handle<Value> Cfunc(const Arguments& args) {
  HandleScope scope;

  if(args.Length() != 1) {
    ThrowException(Exception::TypeError(String::New("Wrong number of arguments")));
    return scope.Close(Undefined());
  }
  if (!args[0]->IsArray()) {
    ThrowException(Exception::TypeError(String::New("Wrong arguments")));
    return scope.Close(Undefined());
  }
  
  Local<Array> arr = Local<Array>::Cast(args[0]);
  int l = arr->Length();
  unsigned char codes[l];
  
  for ( int i=0 ; i<l ;i++) {
    codes[i] = arr->Get(i)->Uint32Value();
    printf("%02x,",codes[i]);
  }
  
  printf("\n");
  void *res = mmap(0,l,PROT_READ | PROT_WRITE | PROT_EXEC,
                   MAP_PRIVATE | MAP_ANON, -1, 0);
  memcpy(res,codes,l);
  auto test = reinterpret_cast<int(*)()>(res);
  munmap(res,0);
  Local<Number> num = Number::New(test());
  
  return scope.Close(num);  
}


int main(int argc, char* argv[]) {
  HandleScope handle_scope;
  Handle<ObjectTemplate> global = ObjectTemplate::New();
  global->Set(String::New("Cfunc"), FunctionTemplate::New(Cfunc));

  Persistent<Context> context = Context::New(NULL,global);  
  Context::Scope context_scope(context);
  
  Handle<String> source = String::New("Cfunc(new Array(0xb8,0x05,0x00,0x00,0x00,0xc3));");
  Handle<Script> script = Script::Compile(source);
  Handle<Value> result = script->Run();
  
  context.Dispose();
  String::AsciiValue ascii(result);
  printf("%s\n",*ascii);
  return 0;
}

