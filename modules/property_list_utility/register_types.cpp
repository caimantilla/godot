#include "register_types.h"
#include "sub_object_property_list_helper.hpp"


void initialize_property_list_utility_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		GDREGISTER_CLASS(SubObjectPropertyListHelper);
	}
}


void uninitialize_property_list_utility_module(ModuleInitializationLevel p_level)
{
}
