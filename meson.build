project(
  'BR_Linux_ManagementTool',
  'cpp',
  version : '0.1',
  meson_version : '>= 1.3.0',
  default_options : ['warning_level=3', 'cpp_std=c++17',],
)

asio_dep = declare_dependency(
    include_directories: 'src/vendor/asio/asio/include'
)

crow_dep = declare_dependency(
    include_directories: 'src/vendor/Crow/include'
)

dependencies = [
asio_dep,
crow_dep,
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
