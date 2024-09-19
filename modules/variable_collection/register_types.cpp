#include "register_types.h"
#include "variable_collection.hpp"


void initialize_variable_collection_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		GDREGISTER_CLASS(VariableCollection);
	}
}


void uninitialize_variable_collection_module(ModuleInitializationLevel p_level)
{
}
