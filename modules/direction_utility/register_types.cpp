#include "register_types.h"
#include "core/config/engine.h"
#include "direction_helper.hpp"
#include "side_3d_bit_field_struct.hpp"
#include "side_3d_int_struct.hpp"


void initialize_direction_utility_module(ModuleInitializationLevel p_level)
{
	// Register singletons with the server hook
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		memnew(DirectionHelper);
	}
	// Register classes with the scene hook
	else if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		GDREGISTER_ABSTRACT_CLASS(DirectionHelper); // GDREGISTER_ABSTRACT_CLASS is how bound singleton classes are registered in the engine code.
		GDREGISTER_CLASS(Side3DBitFieldStruct);
		GDREGISTER_CLASS(Side3DIntStruct);

		// Weird
		Engine::get_singleton()->add_singleton(Engine::Singleton("DirectionHelper", DirectionHelper::get_singleton(), "DirectionHelper"));
	}
}


void uninitialize_direction_utility_module(ModuleInitializationLevel p_level)
{
	// Unregister singletons with the server hook
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		Engine::get_singleton()->remove_singleton("DirectionHelper");
		memdelete(DirectionHelper::get_singleton());
	}
}
