#ifndef GODOTDISCORD_H
#define GODOTDISCORD_H

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <include/discord_rpc.h>
#include <include/discord_register.h>

#include "core/io/ip.h"
#include "core/io/ip_address.h"
#include "dictionary.h"
#include "object.h"
#include "reference.h"
#include "scene/resources/texture.h"

class GodotDiscord : public Object
{
public:

  static GodotDiscord *get_singleton();
  static void reset_singleton();
  GodotDiscord();
  ~GodotDiscord();

  void initialize(Dictionary initialize);
  void clear(); 
  void update(Dictionary presence);
  void reply(String userId, int reply);
  void runCallbacks();
  void shutdown();

  // Discord callbacks
  void _ready(const DiscordUser* discordUser);
  void _disconnected(int code, const char* message);
  void _error(int code, const char* message);
  void _join(const char* secret);
  void _joinRequest(const DiscordUser* request);
  void _spectateGame(const char* secret);

protected:
  bool isInitialized;

  static void _bind_methods();
  static GodotDiscord *singleton;

private:
  OBJ_TYPE(GodotDiscord, Object);
  OBJ_CATEGORY("GodotDiscord");
};
#endif // GODOTDISCORD_H
