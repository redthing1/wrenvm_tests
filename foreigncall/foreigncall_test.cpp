#include <stdio.h>
#include <string.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <wren.hpp>

#define WREN_NEEDED_SLOTS 12

const char* BUILTIN_MODULE_NAME = "builtin";
const char* BUILTIN_MODULE_SOURCE = R"***(
class Test {
  foreign static cool_func_impl(name, platforms, hashes)

  static cool_func(meta) {
    var meta_name = meta.name
    var meta_platforms = meta.platforms
    var meta_hashes = meta.hashes

    return cool_func_impl(meta_name, meta_platforms, meta_hashes)
  }
}
)***";

struct CoolMeta {
  std::string name;
  std::vector<std::string> platforms;
  std::unordered_map<std::string, std::string> hashes;
};

static void print_cool_meta(const CoolMeta& meta) {
  printf("CoolMeta {\n");
  printf("  name: %s\n", meta.name.c_str());
  printf("  platforms: [");
  for (size_t i = 0; i < meta.platforms.size(); i++) {
    printf("%s", meta.platforms[i].c_str());
    if (i < meta.platforms.size() - 1) {
      printf(", ");
    }
  }
  printf("]\n");
  printf("  hashes: {\n");
  for (const auto& [key, value] : meta.hashes) {
    printf("    %s: %s\n", key.c_str(), value.c_str());
  }
  printf("  }\n");
  printf("}\n");
}

static void assert_msg(bool condition, const char* message) {
  if (!condition) {
    fprintf(stderr, "assertion failed: %s\n", message);
    exit(1);
  }
}

static void fcall_cool_func_impl(WrenVM* vm) {
  // read the arguments to make a CoolMeta struct
  // if we can parse successfully, return true

  // get name
  size_t arg_name_slot = 1;
  assert_msg(wrenGetSlotType(vm, arg_name_slot) == WREN_TYPE_STRING, "name must be a string");
  const char* name = wrenGetSlotString(vm, arg_name_slot);

  // get platforms
  size_t arg_platforms_slot = 2;
  assert_msg(wrenGetSlotType(vm, arg_platforms_slot) == WREN_TYPE_LIST, "platforms must be a list");
  WrenHandle* platforms_list = wrenGetSlotHandle(vm, arg_platforms_slot);
  std::vector<std::string> platforms;

  // get hashes
  size_t arg_hashes_slot = 3;
  assert_msg(wrenGetSlotType(vm, arg_hashes_slot) == WREN_TYPE_MAP, "hashes must be a map");
  WrenHandle* hashes_map = wrenGetSlotHandle(vm, arg_hashes_slot);
  std::unordered_map<std::string, std::string> hashes;

  // ensure that we have plenty of slots
  wrenEnsureSlots(vm, WREN_NEEDED_SLOTS);
  size_t slot_temp = 10;

  // size_t platforms_count = wrenGetListCount(vm, arg_platforms_slot);
  // for (size_t i = 0; i < platforms_count; i++) {
  //   size_t platform_list_el_slot = slot_temp;
  //   wrenGetListElement(vm, arg_platforms_slot, i, platform_list_el_slot);
  //   assert_msg(wrenGetSlotType(vm, platform_list_el_slot) == WREN_TYPE_STRING, "platform must be a string");
  //   const char* platform = wrenGetSlotString(vm, platform_list_el_slot);
  //   platforms.push_back(platform);
  // }

  // return true
  wrenSetSlotBool(vm, 0, true);
}

char* read_file_text(const char* path) {
  FILE* file = fopen(path, "r");
  if (file == NULL) {
    fprintf(stderr, "Could not open file %s\n", path);
    return NULL;
  }
  fseek(file, 0, SEEK_END);
  size_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char* text = (char*) malloc(file_size + 1);
  fread(text, 1, file_size, file);
  text[file_size] = '\0';
  fclose(file);
  return text;
}

static void wren_write_fn(WrenVM* vm, const char* text) { printf("%s", text); }

static void wren_error_fn(WrenVM* vm, WrenErrorType type, const char* module, int line, const char* message) {
  switch (type) {
  case WREN_ERROR_COMPILE:
    printf("wren error: compile error: %s:%d: %s\n", module, line, message);
    break;
  case WREN_ERROR_RUNTIME:
    printf("wren error: runtime error: %s:%d: %s\n", module, line, message);
    break;
  case WREN_ERROR_STACK_TRACE:
    printf("wren error: stack trace: %s:%d: %s\n", module, line, message);
    break;
  }
}

static WrenLoadModuleResult wren_load_module_fn(WrenVM* vm, const char* name) {
  WrenLoadModuleResult result;
  if (strcmp(name, BUILTIN_MODULE_NAME) == 0) {
    result.source = BUILTIN_MODULE_SOURCE;
    result.onComplete = NULL;
    return result;
  }

  printf("error: unknown module %s\n", name);
  return {NULL};
}

static WrenForeignMethodFn wren_bind_foreign_method_fn(
    WrenVM* vm, const char* module, const char* className, bool isStatic, const char* signature
) {
  if (strcmp(module, BUILTIN_MODULE_NAME) == 0) {
    if (strcmp(className, "Test") == 0) {
      if (strcmp(signature, "cool_func_impl(_,_,_)") == 0) {
        return fcall_cool_func_impl;
      }
    }
  }

  return NULL;
}

int main(void) {
  const char* source_module = "test";
  const char* source_file = "./test.wren";

  printf("loading file %s\n", source_file);
  char* source_text = read_file_text(source_file);

  printf("wren: initialize configuration\n");
  WrenConfiguration config;
  wrenInitConfiguration(&config);

  // bind functions
  config.writeFn = wren_write_fn;
  config.errorFn = wren_error_fn;
  config.loadModuleFn = wren_load_module_fn;
  config.bindForeignMethodFn = wren_bind_foreign_method_fn;

  printf("wren: create vm\n");
  WrenVM* vm = wrenNewVM(&config);

  printf("wren: interpret module: [%s]:\n%s\n---\n", source_module, source_text);
  WrenInterpretResult result = wrenInterpret(vm, source_module, source_text);
  if (result == WREN_RESULT_SUCCESS) {
    printf("wren: success\n");
  } else {
    printf("wren: error\n");
  }

  // now, we want to call Script.run()

  wrenEnsureSlots(vm, WREN_NEEDED_SLOTS);

  // get handle to Script class
  wrenGetVariable(vm, source_module, "Script", 0);
  WrenHandle* script_class = wrenGetSlotHandle(vm, 0);
  // make call handle to run method
  WrenHandle* run_method = wrenMakeCallHandle(vm, "run()");

  // invoke Script.run()
  wrenSetSlotHandle(vm, 0, script_class);
  printf("wren: call Script.run()\n");
  wrenCall(vm, run_method);

  // get the boolean return value
  if (wrenGetSlotType(vm, 0) != WREN_TYPE_BOOL) {
    printf("wren: Script.run() must return a boolean, but it returned %d\n", wrenGetSlotType(vm, 0));
    assert_msg(false, "Script.run() returned invalid type");
  }
  bool run_result = wrenGetSlotBool(vm, 0);
  printf("wren: Script.run() returned %s\n", run_result ? "true" : "false");

  // release handles
  wrenReleaseHandle(vm, script_class);
  wrenReleaseHandle(vm, run_method);

  printf("wren: free vm\n");
  wrenFreeVM(vm);

  return 0;
}