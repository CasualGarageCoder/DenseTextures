#include "register_types.h"

#include "core/class_db.h"
#include "densetextures.h"

void register_densetextures_types() {
	ClassDB::register_class<HiResTexture>();
	ClassDB::register_class<DenseGradientTexture>();
}

void unregister_densetextures_types() {
}
