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

// Initialize Steamworks
void GodotDiscord::initialize(Dictionary initialize) {
  isInitialized = true;

  DiscordEventHandlers handlers;

  memset(&handlers, 0, sizeof(handlers));

  handlers.ready = &GodotDiscord::handleDiscordReady;
  handlers.errored = &GodotDiscord::handleDiscordError;
  handlers.disconnected = &GodotDiscord::handleDiscordDisconnected;
  handlers.joinGame = &GodotDiscord::handleDiscordJoinGame;
  handlers.spectateGame = &GodotDiscord::handleDiscordSpectateGame;
  handlers.joinRequest = &GodotDiscord::handleDiscordJoinRequest;

  CharString buff = ((String)initialize["app_id"]).ascii();

  int autoRegister = 1;
  const char* steamId = NULL;
  const char* appId = buff.get_data();

  if (initialize.has("auto_register")) {
    autoRegister = ((int)initialize["auto_register"]);
  }

  if (initialize.has("steam_id")) {
    buff = ((String)initialize["steam_id"]).ascii();
    steamId = buff.get_data();
  }

  Discord_Initialize(appId, &handlers, autoRegister, steamId);
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
  CharString buff = userId.ascii();
  
  switch (response) {
    case 1:
      resp = DISCORD_REPLY_YES;
      break;
    case 2:
      resp = DISCORD_REPLY_IGNORE;
      break;
  }

  Discord_Respond(buff.get_data(), resp);
}

/**
 *  https://discordapp.com/developers/docs/rich-presence/how-to
 */
void GodotDiscord::update(Dictionary presence) {
  //  Not initialized
  if (!isInitialized) {
    return;
  }

  CharString buff;
  DiscordRichPresence discordPresence;
  memset(&discordPresence, 0, sizeof(discordPresence));

  //  the user's current party status
  //  idk if is necessary or optional "discord-rpc/src/serialization.cpp 108"
  if (presence.has("state")) {
    buff = ((String)presence["state"]).utf8();
    discordPresence.state = buff.get_data();
  }

  //  what the player is currently doing
  //  idk if is necessary or optional "discord-rpc/src/serialization.cpp 109"
  if (presence.has("details")) {
    buff = ((String)presence["details"]).utf8();
    discordPresence.details = buff.get_data();
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
    buff = ((String)presence["large_image_key"]).utf8();
    discordPresence.largeImageKey = buff.get_data();
  }

  //  OPTIONAL
  //  tooltip for the largeImageKey
  if (presence.has("large_image_text")) {
    buff = ((String)presence["large_image_text"]).utf8();
    discordPresence.largeImageText = buff.get_data();
  }

  //  OPTIONAL
  //  name of the uploaded image for the small profile artwork
  if (presence.has("small_image_key")) {
    buff = ((String)presence["small_image_key"]).utf8();
    discordPresence.smallImageKey = buff.get_data();
  }

  //  OPTIONAL
  //  tootltip for the smallImageKey
  if (presence.has("small_image_text")) {
    buff = ((String)presence["small_image_text"]).utf8();
    discordPresence.smallImageText = buff.get_data();
  }

  // OPTIONAL
  // discord-rpc/src/serialization.cpp 136
  if (presence.has("party_id") || presence.has("party_size") || presence.has("party_max")) {

    //  id of the player's party, lobby, or group
    if (presence.has("party_id")) {
      buff = ((String)presence["party_id"]).utf8();
      discordPresence.partyId = buff.get_data();
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
    buff = ((String)presence["spectate_secret"]).utf8();
    discordPresence.spectateSecret = buff.get_data();
  }

  //  OPTIONAL
  //  unique hashed string for chat invitations and Ask to Join
  //  discord-rpc/src/serialization.cpp 147
  if (presence.has("join_secret")) {
    buff = ((String)presence["join_secret"]).utf8();  
    discordPresence.joinSecret = buff.get_data();
  }

  Discord_UpdatePresence(&discordPresence);
}

void GodotDiscord::_ready(const DiscordUser* request) {
  Dictionary user;
  user[Variant("user_id")] = String(request->userId);
  user[Variant("username")] = String(request->username);
  user[Variant("discriminator")] = String(request->discriminator);
  user[Variant("avatar")] = String(request->avatar);

  emit_signal("ready", user);
}

void GodotDiscord::_disconnected(int errorCode, const char* message) {
  emit_signal("disconnected", errorCode, message);
}

void GodotDiscord::_errored(int errorCode, const char* message) {
  emit_signal("errored", errorCode, message);
}

void GodotDiscord::_joinGame(const char* joinSecret) {
  emit_signal("joinGame", joinSecret);
}

void GodotDiscord::_spectateGame(const char* spectateSecret) {
  emit_signal("spectateGame", spectateSecret);
}

void GodotDiscord::_joinRequest(const DiscordUser* request) {
  Dictionary user;
  user[Variant("user_id")] = String(request->userId);
  user[Variant("username")] = String(request->username);
  user[Variant("discriminator")] = String(request->discriminator);
  user[Variant("avatar")] = String(request->avatar);
  
  emit_signal("joinRequest", user);
}

void GodotDiscord::_bind_methods() {
  ObjectTypeDB::bind_method("initialize", &GodotDiscord::init);
  ObjectTypeDB::bind_method("clear", &GodotDiscord::clear);
  ObjectTypeDB::bind_method("update", &GodotDiscord::update);
  ObjectTypeDB::bind_method("reply", &GodotDiscord::reply);
  ObjectTypeDB::bind_method("run_callbacks", &GodotDiscord::runCallbacks);
  ObjectTypeDB::bind_method("shutdown", &GodotDiscord::shutdown);

  // Signals //////////////////////////////////
  ADD_SIGNAL(MethodInfo("ready",
    PropertyInfo(Variant::DICTIONARY, 'user')
  ));

  ADD_SIGNAL(MethodInfo("disconnected",
    PropertyInfo(Variant::INT, 'error_code'),
    PropertyInfo(Variant::STRING, 'message')
  ));

  ADD_SIGNAL(MethodInfo("errored",
    PropertyInfo(Variant::INT, 'error_code'),
    PropertyInfo(Variant::STRING, 'message')
  ));

  ADD_SIGNAL(MethodInfo("joinGame",
    PropertyInfo(Variant::STRING, 'joinSecret')
  ));

  ADD_SIGNAL(MethodInfo("spectateGame",
    PropertyInfo(Variant::STRING, 'spectateSecret')
  ));

  ADD_SIGNAL(MethodInfo("joinRequest",
    PropertyInfo(Variant::DICTIONARY, 'user')
  ));
}
