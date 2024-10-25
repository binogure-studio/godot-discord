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

  void initialize(const String &appId, int auto_register, const String &steamId);
  void clear(); 
  void update();
  void reply(String userId, int reply);
  void runCallbacks();
  void shutdown();

  void setState(const String &state, const String &details);
  void setStartTimestamp(int64_t start_timestamp);
  void setEndTimestamp(int64_t end_timestamp);
  void setLargeImage(const String &large_image_text, const String &large_image_key);
  void setSmallImage(const String &small_image_text, const String &small_image_key);
  void setPartySize(const String &party_id, int size_value, int max_value);
  void setSpectateSecret(const String &spectate_secret);
  void setJoinSecret(const String &join_secret);

  // Discord callbacks
  static void godot_ready(const DiscordUser* discordUser);
  static void godot_disconnected(int code, const char* message);
  static void godot_errored(int code, const char* message);
  static void godot_joinGame(const char* secret);
  static void godot_joinRequest(const DiscordUser* request);
  static void godot_spectateGame(const char* secret);

protected:
  bool isInitialized;
  DiscordRichPresence discordPresence;

  static void _bind_methods();
  static GodotDiscord *singleton;

private:
  OBJ_TYPE(GodotDiscord, Object);
  OBJ_CATEGORY("GodotDiscord");
};
#endif // GODOTDISCORD_H
