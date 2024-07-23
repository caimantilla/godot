#include "message_command.hpp"


void MessageCommand::_bind_methods()
{
	BIND_ENUM_CONSTANT(WAIT_STYLE_NONE);
	BIND_ENUM_CONSTANT(WAIT_STYLE_STOP);
	BIND_ENUM_CONSTANT(WAIT_STYLE_PARALLEL);

	ClassDB::bind_method(D_METHOD("get_possible_names"), &MessageCommand::_get_possible_names);
	ClassDB::bind_method(D_METHOD("is_command_name_valid", "command_name"), &MessageCommand::is_command_name_valid);
	ClassDB::bind_method(D_METHOD("get_context_objects"), &MessageCommand::binding_get_context_objects);
	ClassDB::bind_method(D_METHOD("get_runtime"), &MessageCommand::get_runtime);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "runtime", PROPERTY_HINT_NONE, "MessageRuntime", PROPERTY_USAGE_NONE, "MessageRuntime"), "", "get_runtime");

	GDVIRTUAL_BIND(_get_possible_names);
	GDVIRTUAL_BIND(_execute, "state", "arguments");
}


MessageRuntime *MessageCommand::get_runtime() const
{
	return runtime;
}


bool MessageCommand::is_command_name_valid(const String &p_command_name) const
{
	PackedStringArray possible_names = _get_possible_names();
	
	for (const String &possible_name : possible_names)
	{
		// use case-insensitive comparison
		if (p_command_name.nocasecmp_to(possible_name) == 0)
		{
			return true;
		}
	}

	return false;
}


MessageCommand::WaitStyle MessageCommand::execute(MessageState *p_state, const String &p_arguments)
{
	ERR_FAIL_NULL_V_MSG(runtime, WAIT_STYLE_NONE, "Runtime undefined. Do not instantiate MessageCommand directly.");
	return _execute(p_state, p_arguments);
}


PackedStringArray MessageCommand::_get_possible_names() const
{
	PackedStringArray ret;
	if (!GDVIRTUAL_CALL(_get_possible_names, ret))
	{
		ERR_PRINT("_get_possible_names must be overriden.");
	}
	return ret;
}


MessageCommand::WaitStyle MessageCommand::_execute(MessageState *p_state, const String &p_arguments)
{
	int wait_style = WAIT_STYLE_NONE;
	if (!GDVIRTUAL_CALL(_execute, p_state, p_arguments, wait_style))
	{
		ERR_PRINT("_execute must be overriden.");
		p_state->finish();
	}
	return (WaitStyle)wait_style;
}


TypedArray<Object> MessageCommand::binding_get_context_objects() const
{
	TypedArray<Object> ret;
	ret.resize(context_object_list->size());

	for (int i = 0; i < context_object_list->size(); i++)
	{
		ret.set(i, context_object_list->operator[](i));
	}

	return ret;
}
