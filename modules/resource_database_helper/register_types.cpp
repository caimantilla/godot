#include "register_types.h"
#include "resource_database_helper.hpp"
#include "core/config/engine.h"


void initialize_resource_database_helper_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		memnew(ResourceDatabaseHelper);
	}
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		GDREGISTER_ABSTRACT_CLASS(ResourceDatabaseHelper);
		Engine::get_singleton()->add_singleton(Engine::Singleton("ResourceDatabaseHelper", ResourceDatabaseHelper::get_singleton(), "ResourceDatabaseHelper"));
	}
}


void uninitialize_resource_database_helper_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		Engine::get_singleton()->remove_singleton("ResourceDatabaseHelper");
		memdelete(ResourceDatabaseHelper::get_singleton());
	}
}
