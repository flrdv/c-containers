project('c-containers', 'c', version : '0.0.1')

if get_option('buildtype').startswith('debug')
    message('Building with sanitizers')
    add_project_arguments('-fsanitize=address,undefined', language: 'c')
endif

# Mount subdirectories
tests = []

subdir('tests')

# Dependencies
unity_dep = subproject('unity').get_variable('unity_dep')

# Tests
foreach test : tests
    name = test[0]
    file = test[1]
    exe = executable(file, ['tests' / file], dependencies : unity_dep)
    test(name, exe)
endforeach