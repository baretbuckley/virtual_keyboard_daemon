project('input_device_daemon', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])


incdir = include_directories('include')



# files shared between both client and daemon (not tool)
shared_src = ['src/message.c']

client_src = ['src/client_options.c', 'src/keycode_from_str.c', 'src/parse_option.c']

daemon_src = ['src/daemon_options.c', 'src/parse_option.c']

tool_src = []


if build_machine.system() == 'windows'
  shared_src += ['src/windows/channel.c', 'src/windows/keycode.c']
  daemon_src += ['src/windows/virtual_keyboard.c']
  

elif build_machine.system() == 'linux'
  shared_src += ['src/linux/channel.c', 'src/linux/keycode.c']
  daemon_src += ['src/linux/virtual_keyboard.c']
  tool_src += ['src/linux/keycode.c']
endif
# src_files = ['src/message.c']

# client_src = ['src/client.c', 'src/commands.c', 'src/keycodeFromStr.c', 'src/parse_option.c']

# tool_src = ['src/linux/keycode.c']

# executable('demo',
#            ['src/main.c'] + src_files + os_src,
#            include_directories : incdir,
#            install : true)

executable('vkeyd',
           ['src/daemon.c'] + shared_src + daemon_src,
           include_directories : incdir,
           install : true)

executable('vkey',
           ['src/client.c'] + client_src + shared_src,
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