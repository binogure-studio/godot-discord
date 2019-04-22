# Godot Discord (Godot Engine 2.1.x)

Discord Rich Presence for Godot Engine. Working for Windows, Linux and Max platforms.

# Usage

Copy the `godotdiscord` repository into the modules folder then compile Godot Engine.
You will need to have the sdk folder in the same directory as Godot Engine binary and exported binaries.

## SDK

Directory tree structure. Download original SDK from this [github repository](https://github.com/discordapp/discord-rpc/releases)


### godotdiscord/sdk/discord-rpc/linux32:

To generate the linux32 discord rpc build follow [this gist](https://gist.github.com/Swyter/98074d2d42cd7f7c0e5f86ef619969d3)

```
libdiscord-rpc.so
```

### godotdiscord/sdk/discord-rpc/linux64:

```
libdiscord-rpc.so
```

### godotdiscord/sdk/discord-rpc/osx32:

```
libdiscord-rpc.dylib
```

### godotdiscord/sdk/discord-rpc/win32:

```
discord-rpc.dll
discord-rpc.lib
send-presence.exe
```

### godotdiscord/sdk/discord-rpc/win64:

```
discord-rpc.dll
discord-rpc.lib
send-presence.exe
```