project('sulfide', 'cpp',
	version : '0.1',
	default_options : [
		'cpp_std=c++20',
		'cpp_rtti=false',
		'cpp_eh=none'
	])

flex = find_program('flex')
bison = find_program('bison')

flex_gen = generator(flex,
	output : '@BASENAME@.yy.cpp',
	arguments : ['-o', '@OUTPUT@', '@INPUT@'])

bison_gen = generator(bison,
	output : ['@BASENAME@.cpp', '@BASENAME@.hpp'],
	arguments : ['-Wall', '-Wcounterexamples', '--color', '@INPUT@', '-o', '@OUTPUT0@'])

conflict_dep = dependency('conflict')

executable('sulfide', 'src/frontend/main.cpp', 'src/frontend/debug.cpp',
	flex_gen.process('src/frontend/scanner.ll'),
	bison_gen.process('src/frontend/parser.yy'),
	include_directories : 'src/',
	dependencies : [ conflict_dep ])
