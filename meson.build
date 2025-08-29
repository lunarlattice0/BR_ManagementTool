project(
  'BR_Linux_ManagementTool',
  'cpp',
  version : '0.1',
  meson_version : '>= 1.3.0',
  default_options : ['warning_level=3', 'cpp_std=c++23',],
)

cmake = import('cmake')

opt_var = cmake.subproject_options()
opt_var.add_cmake_defines({'CMAKE_TRY_COMPILE_TARGET_TYPE':'STATIC_LIBRARY'})
minhook_subproject = cmake.subproject('minhook', options:opt_var)
minhook_dependency = minhook_subproject.dependency('minhook')

asio_dep = declare_dependency(
    include_directories: 'src/vendor/asio/asio/include'
)

crow_dep = declare_dependency(
    include_directories: 'src/vendor/Crow/include'
)

dependencies = [
asio_dep,
crow_dep,
minhook_dependency,
]

brlmt_include_directories = [
  'src/',
]

brlmt_src_files = [
  'src/main.cpp',
  'src/brlmt.cpp',
]

exe = executable(
  'brlmt',
  [brlmt_src_files],
  install : true,
  dependencies : dependencies,
  include_directories: brlmt_include_directories,
  link_args:['-static-libstdc++', '-static-libgcc','-pthread','-static'],
)

payload = library(
    'brlmt_payload',
    'src/payload.cpp',
    dependencies: dependencies,
    include_directories: brlmt_include_directories,
    link_args:['-static-libstdc++','-static-libgcc','-lpthread','-static','-lws2_32','-lwsock32','-lgdiplus'],
    # Please be aware, static link is required.

)
