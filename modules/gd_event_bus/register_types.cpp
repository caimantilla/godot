#include "register_types.h"
#include "gd_event_bus.hpp"
#include "gd_event.hpp"


void initialize_gd_event_bus_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		GDREGISTER_CLASS(GDEventBus);
		GDREGISTER_CLASS(GDEvent);
	}
}


void uninitialize_gd_event_bus_module(ModuleInitializationLevel p_level)
{
}
