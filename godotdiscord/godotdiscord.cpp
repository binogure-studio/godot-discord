#include "godotdiscord.h"

GodotDiscord *GodotDiscord::singleton = NULL;

GodotDiscord::GodotDiscord() {
  isInitialized = false;
  singleton = this;
  memset(&discordPresence, 0, sizeof(discordPresence));
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

void GodotDiscord::initialize(const String &appId, int auto_register, const String &steamId) {
  isInitialized = true;

  DiscordEventHandlers handlers;

  memset(&handlers, 0, sizeof(handlers));

  handlers.ready = &GodotDiscord::godot_ready;
  handlers.errored = &GodotDiscord::godot_errored;
  handlers.disconnected = &GodotDiscord::godot_disconnected;
  handlers.joinGame = &GodotDiscord::godot_joinGame;
  handlers.spectateGame = &GodotDiscord::godot_spectateGame;
  handlers.joinRequest = &GodotDiscord::godot_joinRequest;

  Discord_Initialize(appId.utf8().get_data(), &handlers, auto_register, steamId.utf8().get_data());
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

void GodotDiscord::setState(const String &state, const String &details) {
  discordPresence.state = strdup(state.utf8().get_data());
  discordPresence.details = strdup(details.utf8().get_data());
}

void GodotDiscord::setTimestamp(int64_t start_timestamp, int64_t end_timestamp) {
  discordPresence.startTimestamp = start_timestamp;
  discordPresence.endTimestamp = end_timestamp;
}

void GodotDiscord::setLargeImage(const String &large_image_text, const String &large_image_key) {
  discordPresence.largeImageText = strdup(large_image_text.utf8().get_data());
  discordPresence.largeImageKey = strdup(large_image_key.utf8().get_data());
}

void GodotDiscord::setSmallImage(const String &small_image_text, const String &small_image_key) {
  discordPresence.smallImageText = strdup(small_image_text.utf8().get_data());
  discordPresence.smallImageKey = strdup(small_image_key.utf8().get_data());
}

void GodotDiscord::setPartySize(const String &party_id, int size_value, int max_value) {
  discordPresence.partyId = strdup(party_id.utf8().get_data());
  discordPresence.partySize = size_value;
  discordPresence.partyMax = max_value;
}

void GodotDiscord::setSpectateSecret(const String &spectate_secret) {
  discordPresence.spectateSecret = strdup(spectate_secret.utf8().get_data());
}

void GodotDiscord::setJoinSecret(const String &join_secret) {
  discordPresence.joinSecret = strdup(join_secret.utf8().get_data());
}

void GodotDiscord::update() {
  //  Not initialized
  if (!isInitialized) {
    return;
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

  ObjectTypeDB::bind_method("set_state", &GodotDiscord::setState);
  ObjectTypeDB::bind_method("set_timestamp", &GodotDiscord::setTimestamp);
  ObjectTypeDB::bind_method("set_large_image", &GodotDiscord::setLargeImage);
  ObjectTypeDB::bind_method("set_small_image", &GodotDiscord::setSmallImage);
  ObjectTypeDB::bind_method("set_party_size", &GodotDiscord::setPartySize);
  ObjectTypeDB::bind_method("set_spectate_secret", &GodotDiscord::setSpectateSecret);
  ObjectTypeDB::bind_method("set_join_secret", &GodotDiscord::setJoinSecret);

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
