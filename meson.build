project('input_device_daemon', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])


incdir = include_directories('include')


if build_machine.system() == 'windows'
  os_src = ['src/windows/channel.c', 'src/windows/virtual_keyboard.c', 'src/windows/keycode.c']
elif build_machine.system() == 'linux'
  os_src = ['src/linux/channel.c', 'src/linux/virtual_keyboard.c', 'src/linux/keycode.c']
endif
src_files = ['src/message.c']

client_src = ['src/client.c', 'src/commands.c', 'src/keycodeFromStr.c', 'src/parse_option.c']

tool_src = ['src/linux/keycode.c']

# executable('demo',
#            ['src/main.c'] + src_files + os_src,
#            include_directories : incdir,
#            install : true)

executable('vkeyd',
           ['src/daemon.c'] + src_files + os_src,
           include_directories : incdir,
           install : true)

executable('vkey',
           client_src + src_files + os_src,
           include_directories : incdir,
           install : true)

executable('vkeytool',
           ['src/vkeytool/vkeytool.c'] + tool_src,
           include_directories: incdir,
           install : true)

# executable('linux_test',
#   ['src/linux_test.c', 'src/linux/virtual_keyboard.c'],
#   include_directories : incdir,
#   install : false
# )