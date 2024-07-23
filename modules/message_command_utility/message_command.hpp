#ifndef MESSAGE_COMMAND_HPP
#define MESSAGE_COMMAND_HPP


#include "core/object/object.h"
#include "core/object/gdvirtual.gen.inc"
#include "message_state.hpp"
#include "message_runtime.hpp"


class MessageCommand : public Object
{
	friend class MessageRuntime;

	GDCLASS(MessageCommand, Object);

public:
	enum WaitStyle
	{
		WAIT_STYLE_NONE = 0,
		WAIT_STYLE_STOP = 1,
		WAIT_STYLE_PARALLEL = 2,
	};

protected:
	MessageRuntime *runtime = nullptr;
	LocalVector<Object *> *context_object_list; // ALWAYS ADD THIS PLEASE.

	TypedArray<Object> binding_get_context_objects() const;
	static void _bind_methods();
	virtual PackedStringArray _get_possible_names() const;
	virtual WaitStyle _execute(MessageState *p_state, const String &p_arguments);

	GDVIRTUAL0RC(PackedStringArray, _get_possible_names);
	GDVIRTUAL2R(int, _execute, MessageState *, const String &);

public:
	bool is_command_name_valid(const String &p_command_name) const;
	WaitStyle execute(MessageState *p_state, const String &p_arguments);
	MessageRuntime *get_runtime() const;
};


VARIANT_ENUM_CAST(MessageCommand::WaitStyle);


#endif // MESSAGE_COMMAND_TYPE_HPP
