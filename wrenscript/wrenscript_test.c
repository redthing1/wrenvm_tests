#include "wren.h"
#include <stdio.h>

char* read_file_text(const char* path) {
  FILE* file = fopen(path, "r");
  if (file == NULL) {
    fprintf(stderr, "Could not open file %s\n", path);
    return NULL;
  }
  fseek(file, 0, SEEK_END);
  size_t file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char* text = (char*)malloc(file_size + 1);
  fread(text, 1, file_size, file);
  text[file_size] = '\0';
  fclose(file);
  return text;
}

static void wren_write_fn(WrenVM* vm, const char* text) {
  // wren: write data
  printf("%s", text);
}

static void wren_error_fn(WrenVM* vm, WrenErrorType type, const char* module, int line, const char* message) {
  // wren: error data
  printf("wren error: %s:%d: %s\n", module, line, message);
}

int main(void) {
  const char* source_module = "hello";
  const char* source_file = "./hello.wren";

  printf("loading file %s\n", source_file);
  char* source_text = read_file_text(source_file);

  printf("wren: initialize configuration\n");
  WrenConfiguration config;
  wrenInitConfiguration(&config);
  config.writeFn = wren_write_fn;
  config.errorFn = wren_error_fn;
  printf("wren: create vm\n");
  WrenVM* vm = wrenNewVM(&config);
  printf("wren: interpret module: [%s]:\n%s\n---\n", source_module, source_text);
  WrenInterpretResult result = wrenInterpret(vm, source_module, source_text);
  if (result == WREN_RESULT_SUCCESS) {
    printf("wren: success\n");
  } else {
    printf("wren: error\n");
  }

  printf("wren: free vm\n");
  wrenFreeVM(vm);

  return 0;
}