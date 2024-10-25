def can_build(platform, env):
  return platform=="x11" or platform=="windows" or platform=="osx"

def configure(env):
  env.Append(CPPPATH=["#modules/godotdiscord/sdk/discord-rpc/"])

  if env["platform"]== "x11":
    env.Append(LIBS=["discord-rpc"])

    if env["bits"]=="32":
      env.Append(RPATH=[env.Literal('\\$$ORIGIN/linux32')])
      env.Append(LIBPATH=["#modules/godotdiscord/sdk/discord-rpc/linux32"])
    else: # 64 bit
      env.Append(RPATH=[env.Literal('\\$$ORIGIN/linux64')])
      env.Append(LIBPATH=["#modules/godotdiscord/sdk/discord-rpc/linux64"])
  elif env["platform"] == "windows":
    # mostly VisualStudio
    if env["CC"] == "cl":
      env.Append(LINKFLAGS=["discord-rpc.lib"])

      if env["bits"]=="32":
        env.Append(LIBPATH=["#modules/godotdiscord/sdk/discord-rpc/win32"])
      else: # 64 bit
        env.Append(LIBPATH=["#modules/godotdiscord/sdk/discord-rpc/win64"])
    # mostly "gcc"
    else:
      env.Append(LIBS=["discord-rpc"])

      if env["bits"]=="32":
        env.Append(LIBPATH=["#modules/godotdiscord/sdk/discord-rpc/win32"])
      else: # 64 bit
        env.Append(LIBPATH=["#modules/godotdiscord/sdk/discord-rpc/win64"])
  elif env["platform"] == "osx":
    env.Append(LIBS=["discord-rpc"])
    env.Append(LIBPATH=['#modules/godotdiscord/sdk/discord-rpc/osx32'])
