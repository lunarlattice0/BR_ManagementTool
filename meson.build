project(
  'BR_Linux_ManagementTool',
  'cpp',
  version : '0.1',
  meson_version : '>= 1.3.0',
  default_options : ['warning_level=3', 'cpp_std=c++17'],
)

dependencies = [
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
  link_args:['-static-libstdc++', '-static-libgcc','-pthread','-static']
)

payload = library(
    'brlmt_payload',
    'src/payload.cpp',
    dependencies: dependencies,
    include_directories: ['src/'],
    link_args:['-static']
)
