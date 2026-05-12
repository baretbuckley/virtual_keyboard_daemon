project('input_device_daemon', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])


incdir = include_directories('include')

src_files = ['src/channel.c', 'src/message.c', 'src/keycode.c']

client_src = ['src/client.c', 'src/commands.c', 'src/keycodeFromStr.c']


executable('demo',
           ['src/main.c'] + src_files,
           include_directories : incdir,
           install : true)

executable('daemon',
           ['src/daemon.c'] + src_files,
           include_directories : incdir,
           install : true)

executable('client',
           client_src + src_files,
           include_directories : incdir,
           install : true)

# eDNA genetric material present in the enviroment
# Metabarcoding:
#   use of sequence variation in conserved genees to identify the species