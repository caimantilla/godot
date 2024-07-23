#include "register_types.h"


#include "message_runtime.hpp"
#include "message_state.hpp"
#include "message_command.hpp"


void initialize_message_command_utility_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		GDREGISTER_CLASS(MessageState);
		GDREGISTER_CLASS(MessageCommand);
		GDREGISTER_CLASS(MessageRuntime);
	}
}


void uninitialize_message_command_utility_module(ModuleInitializationLevel p_level)
{
}
