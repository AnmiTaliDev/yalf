project(
  'yalfc',
  'c',
  version : '1.0.0',
  license : 'GPL-3.0-or-later',
  default_options : [
    'c_std=c99',
    'b_pie=true',
    'warning_level=2',
    'optimization=2',
  ],
)

inc = include_directories('include')

sources = files(
  'src/buf.c',
  'src/strpool.c',
  'src/parser.c',
  'src/compiler.c',
  'src/main.c',
)

executable(
  'yalfc',
  sources,
  include_directories : inc,
  c_args : ['-D_GNU_SOURCE'],
  install : true,
)
