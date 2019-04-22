#include "godotdiscord.h"

GodotDiscord *GodotDiscord::singleton = NULL;

GodotDiscord::GodotDiscord() {
  isInitialized = false;
  singleton = this;
}

GodotDiscord::~GodotDiscord() {
  Discord_Shutdown();
  singleton = NULL;
}

GodotDiscord *GodotDiscord::get_singleton() {
  if (GodotDiscord::singleton == NULL) {
    GodotDiscord::singleton = new GodotDiscord();
  }

  return GodotDiscord::singleton;
}

void GodotDiscord::reset_singleton() {
  delete GodotDiscord::singleton;

  GodotDiscord::singleton = NULL;
}

void GodotDiscord::initialize(const Dictionary &initialize) {
  isInitialized = true;

  DiscordEventHandlers handlers;

  memset(&handlers, 0, sizeof(handlers));

  handlers.ready = &GodotDiscord::godot_ready;
  handlers.errored = &GodotDiscord::godot_errored;
  handlers.disconnected = &GodotDiscord::godot_disconnected;
  handlers.joinGame = &GodotDiscord::godot_joinGame;
  handlers.spectateGame = &GodotDiscord::godot_spectateGame;
  handlers.joinRequest = &GodotDiscord::godot_joinRequest;

  int autoRegister = 1;
  const char* steamId = NULL;

  if (initialize.has("auto_register")) {
    autoRegister = ((int)initialize["auto_register"]);
  }

  if (initialize.has("steam_id")) {
    steamId = ((String)initialize["steam_id"]).utf8().get_data();
  }

  Discord_Initialize(((String)initialize["app_id"]).utf8().get_data(), &handlers, autoRegister, steamId);
}

void GodotDiscord::clear() {
  Discord_ClearPresence();
}

void GodotDiscord::runCallbacks() {
  Discord_RunCallbacks();
}

void GodotDiscord::shutdown() {
  Discord_Shutdown();
}

void GodotDiscord::reply(String userId, int response) {
  int resp = DISCORD_REPLY_NO;
  
  switch (response) {
    case 1:
      resp = DISCORD_REPLY_YES;
      break;
    case 2:
      resp = DISCORD_REPLY_IGNORE;
      break;
  }

  Discord_Respond(userId.utf8().get_data(), resp);
}

/**
 *  https://discordapp.com/developers/docs/rich-presence/how-to
 */
void GodotDiscord::update(const Dictionary &presence) {
  //  Not initialized
  if (!isInitialized) {
    return;
  }

  DiscordRichPresence discordPresence;
  memset(&discordPresence, 0, sizeof(discordPresence));

  //  the user"s current party status
  //  idk if is necessary or optional "discord-rpc/src/serialization.cpp 108"
  if (presence.has("state")) {
    discordPresence.state = ((String)presence["state"]).utf8().get_data();
  }

  //  what the player is currently doing
  //  idk if is necessary or optional "discord-rpc/src/serialization.cpp 109"
  if (presence.has("details")) {
    discordPresence.details = ((String)presence["details"]).utf8().get_data();
  }

  //  OPTIONAL
  //  discord-rpc/src/serialization.cpp 111
  //  epoch seconds for game start - including will show time as "elapsed"
  if (presence.has("start_timestamp")) {
    discordPresence.startTimestamp = (int64_t)presence["start_timestamp"];
  }

  //  OPTIONAL
  //  discord-rpc/src/serialization.cpp 111
  //  epoch seconds for game end - including will show time as "remaining"
  if (presence.has("end_timestamp")) {
    discordPresence.endTimestamp = (int64_t)presence["end_timestamp"];
  }

  //  OPTIONAL
  //  name of the uploaded image for the large profile artwork  
  if (presence.has("large_image_key")) {
    discordPresence.largeImageKey = ((String)presence["large_image_key"]).utf8().get_data();
  }

  //  OPTIONAL
  //  tooltip for the largeImageKey
  if (presence.has("large_image_text")) {
    discordPresence.largeImageText = ((String)presence["large_image_text"]).utf8().get_data();
  }

  //  OPTIONAL
  //  name of the uploaded image for the small profile artwork
  if (presence.has("small_image_key")) {
    discordPresence.smallImageKey = ((String)presence["small_image_key"]).utf8().get_data();
  }

  //  OPTIONAL
  //  tootltip for the smallImageKey
  if (presence.has("small_image_text")) {
    discordPresence.smallImageText = ((String)presence["small_image_text"]).utf8().get_data();
  }

  // OPTIONAL
  // discord-rpc/src/serialization.cpp 136
  if (presence.has("party_id") || presence.has("party_size") || presence.has("party_max")) {

    //  id of the player's party, lobby, or group
    if (presence.has("party_id")) {
      discordPresence.partyId = ((String)presence["party_id"]).utf8().get_data();
    }

    //  current size of the player's party, lobby, or group
    //  maximum size of the player's party, lobby, or group
    if (presence.has("party_size") && presence.has("party_max")) {
      discordPresence.partySize = (int)presence["party_size"];
      discordPresence.partyMax = (int)presence["party_max"];
    }
  }

  //  OPTIONAL
  //  unique hashed string for Spectate button
  //  discord-rpc/src/serialization.cpp 147
  if (presence.has("spectate_secret")) {
    discordPresence.spectateSecret = ((String)presence["spectate_secret"]).utf8().get_data();
  }

  //  OPTIONAL
  //  unique hashed string for chat invitations and Ask to Join
  //  discord-rpc/src/serialization.cpp 147
  if (presence.has("join_secret")) {
    discordPresence.joinSecret = ((String)presence["join_secret"]).utf8().get_data();
  }

  Discord_UpdatePresence(&discordPresence);
}

void GodotDiscord::godot_ready(const DiscordUser* request) {
  Dictionary user;
  user[Variant("user_id")] = String(request->userId);
  user[Variant("username")] = String(request->username);
  user[Variant("discriminator")] = String(request->discriminator);
  user[Variant("avatar")] = String(request->avatar);

  GodotDiscord::singleton->emit_signal("discord_ready", user);
}

void GodotDiscord::godot_disconnected(int errorCode, const char* message) {
  GodotDiscord::singleton->emit_signal("discord_disconnected", errorCode, message);
}

void GodotDiscord::godot_errored(int errorCode, const char* message) {
  GodotDiscord::singleton->emit_signal("discord_errored", errorCode, message);
}

void GodotDiscord::godot_joinGame(const char* joinSecret) {
  GodotDiscord::singleton->emit_signal("discord_joinGame", joinSecret);
}

void GodotDiscord::godot_spectateGame(const char* spectateSecret) {
  GodotDiscord::singleton->emit_signal("discord_spectateGame", spectateSecret);
}

void GodotDiscord::godot_joinRequest(const DiscordUser* request) {
  Dictionary user;
  user[Variant("user_id")] = String(request->userId);
  user[Variant("username")] = String(request->username);
  user[Variant("discriminator")] = String(request->discriminator);
  user[Variant("avatar")] = String(request->avatar);
  
  GodotDiscord::singleton->emit_signal("discord_joinRequest", user);
}

void GodotDiscord::_bind_methods() {
  ObjectTypeDB::bind_method("initialize", &GodotDiscord::initialize);
  ObjectTypeDB::bind_method("clear", &GodotDiscord::clear);
  ObjectTypeDB::bind_method("update", &GodotDiscord::update);
  ObjectTypeDB::bind_method("reply", &GodotDiscord::reply);
  ObjectTypeDB::bind_method("run_callbacks", &GodotDiscord::runCallbacks);
  ObjectTypeDB::bind_method("shutdown", &GodotDiscord::shutdown);

  // Signals //////////////////////////////////
  ADD_SIGNAL(MethodInfo("discord_ready",
    PropertyInfo(Variant::DICTIONARY, "user")
  ));

  ADD_SIGNAL(MethodInfo("discord_disconnected",
    PropertyInfo(Variant::INT, "error_code"),
    PropertyInfo(Variant::STRING, "message")
  ));

  ADD_SIGNAL(MethodInfo("discord_errored",
    PropertyInfo(Variant::INT, "error_code"),
    PropertyInfo(Variant::STRING, "message")
  ));

  ADD_SIGNAL(MethodInfo("discord_joinGame",
    PropertyInfo(Variant::STRING, "join_secret")
  ));

  ADD_SIGNAL(MethodInfo("discord_spectateGame",
    PropertyInfo(Variant::STRING, "spectate_secret")
  ));

  ADD_SIGNAL(MethodInfo("discord_joinRequest",
    PropertyInfo(Variant::DICTIONARY, "user")
  ));
}
