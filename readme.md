
generic windows and linux keyboard emulator


## Usage

### Spawning the daemon / service

Before running vkey, the daemon/service needs to be started. This is done simply by running the daemons process as:

`vkeyd`

If multiple daemons are needed to be run simultaniously, they must have seperate channel names to distingish them when the client is connecting. This is mentioned in more detail in the 'Specifying the Channel Name' section below.

### vkey usage

The vkey command allows multiple subcommands to be chained together

`vkey [subcommands]*`

Currently implimented subcommands:
- `type` - Type a string
- `press` - Press and release a specified key
- `hold` - Press and hold a specified key
- `release` - Release a held key
- `close-server` - Request the connected daemon to close
- `repeat` - Repeat the subsequent subcommand n times
- `delay` - Delay execution by n ms

Further subcommand information can be found by running `vkey --help`

### Specifying the Channel Name

When running `vkey` or `vkeyd`, the channel is the point of connection for communicating between the processes. The channel must have an associated name. The default when not specified is 'vkeyd'. For linux systems inplace of the name, a complete path can be used.

When creating multiple instances of the daemon, each must of a unique channel name

`vkeyd|vkey`
- `-c, --channel-name <name>`   specify the channel name
- `--linux-channel-path <path>` (linux only) specify the path for the channel location


### Examples

Close a window (alt+F4):

    vkey press -a F4

Type 'hello world' 10 times with a repeat delay of 1 second:

    vkey repeat -d 1000 10 type "hello world"

Pressing the keycode by number:

    vkey press -k 12

Press the left mouse button 5 times with a delay of 0.25 seconds:

    vkey repeat -d 250 5 press LeftMouse

Close the connected daemon:

    vkey close-server

### Additional tools with vkeytool

The goal of `vkeytool` is to provide short programs to assist the user in understanding and working with `vkey`.

`vkeytool list-keys`
- The list-keys subcommand will print out a list of all implimented keys currently offered by vkey.

`vkeytool key-reader`
- Display all pressed keys on the terminal. Usefull for figuring out the correct keyname for a needed key.