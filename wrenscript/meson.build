project('wrenscript', 'c',
  version : '0.1',
  default_options : ['warning_level=1']
)

cc = meson.get_compiler('c')
host_os = host_machine.system()

wren_basedir = '../external/wren'

wren_sources = [
  wren_basedir + '/src/vm/wren_compiler.c',
  wren_basedir + '/src/vm/wren_core.c',
  wren_basedir + '/src/vm/wren_debug.c',
  wren_basedir + '/src/vm/wren_primitive.c',
  wren_basedir + '/src/vm/wren_utils.c',
  wren_basedir + '/src/vm/wren_value.c',
  wren_basedir + '/src/vm/wren_vm.c',
  wren_basedir + '/src/optional/wren_opt_meta.c',
  wren_basedir + '/src/optional/wren_opt_random.c',
]
wren_include = include_directories(
  wren_basedir + '/src/include',
  wren_basedir + '/src/vm',
  wren_basedir + '/src/optional'
)

wrenscript_sources = wren_sources
wrenscript_include = wren_include

wrenscript_lflags = [
  '-static',
]

# system libraries
wrenscript_deps = []

if host_os == 'linux'
  libm_dep = cc.find_library('m', required : true)
  wrenscript_deps += [libm_dep]
endif

wrenscript_stlib = static_library('wrenscript',
  wrenscript_sources,
  include_directories : wrenscript_include,
  gnu_symbol_visibility : 'default',
  dependencies : wrenscript_deps,
  link_args : wrenscript_lflags,
)

test_exe = executable('wrenscript_test',
  'wrenscript_test.c',
  include_directories : wrenscript_include,
  link_with : wrenscript_stlib
)
test('wrenscript', test_exe)

# # make this library usable as a meson subproject.
wrenscript_dep = declare_dependency(
  include_directories: wrenscript_include,
  link_with : wrenscript_stlib,
)
