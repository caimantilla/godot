#include "message_state.hpp"


void MessageState::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("finish"), &MessageState::finish);
	ClassDB::bind_method(D_METHOD("is_finished"), &MessageState::is_finished);
	ClassDB::bind_method(D_METHOD("set_variable", "name", "value"), &MessageState::set_variable);
	ClassDB::bind_method(D_METHOD("get_variable", "name"), &MessageState::get_variable);
	ClassDB::bind_method(D_METHOD("has_variable", "name"), &MessageState::has_variable);

	ADD_SIGNAL(MethodInfo("finished"));
}


void MessageState::finish()
{
	ERR_FAIL_COND_MSG(fnsh, "Already finished.");
	fnsh = true;
	emit_signal(SNAME("finished"));
}


bool MessageState::is_finished() const
{
	return fnsh;
}


void MessageState::set_variable(const StringName &p_name, const Variant &p_value)
{
	set_meta(p_name, p_value);
}


Variant MessageState::get_variable(const StringName &p_name) const
{
	return get_meta(p_name);
}


bool MessageState::has_variable(const StringName &p_name) const
{
	return has_meta(p_name);
}
