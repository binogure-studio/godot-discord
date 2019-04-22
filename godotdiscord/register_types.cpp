#include "globals.h"
#include "object_type_db.h"
#include "register_types.h"

#include "godotdiscord.h"

void register_godotdiscord_types() {
  Globals::get_singleton()->add_singleton(Globals::Singleton("GodotDiscord", GodotDiscord::get_singleton()));

  ObjectTypeDB::register_virtual_type<GodotDiscord>();
}

void unregister_godotdiscord_types() {
  GodotDiscord::reset_singleton();
}
