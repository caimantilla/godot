#include "register_types.h"
#include "sheet_read_utility.hpp"
#include "core/config/engine.h"


void initialize_sheet_read_utility_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		GDREGISTER_CLASS(SheetReadUtility);
		Engine::get_singleton()->add_singleton(Engine::Singleton("SheetReadUtility", memnew(SheetReadUtility), "SheetReadUtility"));
	}
}


void uninitialize_sheet_read_utility_module(ModuleInitializationLevel p_level)
{
}
