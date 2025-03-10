project('c-containers', 'c', version : '0.0.1')

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