#include "gd_event_bus.hpp"

#ifdef MODULE_GDSCRIPT_ENABLED
#include "modules/gdscript/gdscript.h"
#endif // MODULE_GDSCRIPT_ENABLED


void GDEventBus::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("invoke", "event"), &GDEventBus::invoke);
	ClassDB::bind_method(D_METHOD("subscribe", "to", "handler"), &GDEventBus::subscribe);
	ClassDB::bind_method(D_METHOD("unsubscribe", "from", "handler"), &GDEventBus::unsubscribe);
}


const bool GDEventBus::Connection::inc_callback(const Callable &p_ref)
{
	ERR_FAIL_COND_V_MSG(!p_ref.is_valid(), false, RTR("Attempted to add an invalid callback."));
	if (callback_set.has(p_ref))
	{
		callback_set[p_ref] += 1;
	}
	else
	{
		callback_set[p_ref] = 1;
	}
	return true;
}


const bool GDEventBus::Connection::dec_callback(const Callable &p_ref)
{
	ERR_FAIL_COND_V_MSG(!callback_set.has(p_ref), false, RTR("Attempted to decrement an unregistered callback."));

	uint32_t new_ref_count = callback_set[p_ref] - 1;

	if (new_ref_count == 0)
	{
		return erase_callback(p_ref);
	}
	else
	{
		callback_set[p_ref] = new_ref_count;
		return true;
	}
}


const bool GDEventBus::Connection::erase_callback(const Callable &p_ref)
{
	ERR_FAIL_COND_V_MSG(!callback_set.has(p_ref), false, RTR("Attempted to erase a callback, but it wasn't found."));
	return callback_set.erase(p_ref);
}


void GDEventBus::Connection::erase_invalid_callbacks()
{
	for (const KeyValue<Callable, uint32_t> &E : callback_set)
	{
		if (!E.key.is_valid())
		{
			callback_set.erase(E.key);
		}
	}
}


const bool GDEventBus::is_type_valid(const StringName &p_type) const
{
	StringName current_type = ClassDB::get_parent_class_nocheck(p_type);
	while (current_type != SNAME(""))
	{
		if (current_type == SNAME("GDEvent"))
		{
			return true;
		}
		else
		{
			current_type = ClassDB::get_parent_class_nocheck(current_type);
		}
	}
	return false;
}


const bool GDEventBus::is_script_valid(const Ref<Script> &p_script) const
{
	ERR_FAIL_COND_V(!p_script.is_valid(), false);
	ERR_FAIL_COND_V(!p_script->is_valid(), false);
	const StringName type = p_script->get_instance_base_type();
	return is_type_valid(type);
}


GDEventBus::Connection *GDEventBus::create_connection_object_from_type(const StringName &p_type)
{
	ERR_FAIL_COND_V(!is_type_valid(p_type), nullptr);
	ERR_FAIL_COND_V(native_connection_map.has(p_type), nullptr);

	Connection *conn = memnew(Connection);
	native_connection_map[p_type] = conn;

	const StringName parent_type = ClassDB::get_parent_class_nocheck(p_type);
	if (parent_type != SNAME("") && parent_type != SNAME("GDEvent"))
	{
		if (native_connection_map.has(parent_type))
		{
			conn->super_connection = native_connection_map[parent_type];
		}
		else
		{
			conn->super_connection = create_connection_object_from_type(parent_type);
		}
	}

	return conn;
}


GDEventBus::Connection *GDEventBus::create_connection_object_from_script(const Ref<Script> &p_script)
{
	ERR_FAIL_COND_V(!p_script.is_valid(), nullptr);
	ERR_FAIL_COND_V(!is_script_valid(p_script), nullptr);
	ERR_FAIL_COND_V(script_connection_map.has(p_script), nullptr);

	Connection *conn = memnew(Connection);
	script_connection_map[p_script] = conn;

	const Ref<Script> parent_script = p_script->get_base_script();
	if (parent_script.is_valid())
	{
		if (script_connection_map.has(parent_script))
		{
			conn->super_connection = script_connection_map[parent_script];
		}
		else
		{
			conn->super_connection = create_connection_object_from_script(parent_script);
		}
	}
	else
	{
		const StringName type = p_script->get_instance_base_type();
		if (is_type_valid(type))
		{
			if (native_connection_map.has(type))
			{
				conn->super_connection = native_connection_map[type];
			}
			else
			{
				conn->super_connection = create_connection_object_from_type(type);
			}
		}
	}

	return conn;
}


GDEventBus::Connection *GDEventBus::get_or_create_connection_for_type(const StringName &p_type)
{
	if (native_connection_map.has(p_type))
	{
		return native_connection_map[p_type];
	}
	else
	{
		ERR_FAIL_COND_V(!is_type_valid(p_type), nullptr);
		return create_connection_object_from_type(p_type);
	}
}


GDEventBus::Connection *GDEventBus::get_or_create_connection_for_script(const Ref<Script> &p_script)
{
	ERR_FAIL_COND_V(!p_script.is_valid(), nullptr);
	if (script_connection_map.has(p_script))
	{
		return script_connection_map[p_script];
	}
	else
	{
		ERR_FAIL_COND_V(!is_script_valid(p_script), nullptr);
		return create_connection_object_from_script(p_script);
	}
}


void GDEventBus::invoke(const Ref<GDEvent> &p_event)
{
	ERR_FAIL_COND(!p_event.is_valid());

	const Ref<Script> script = p_event->get_script();
	Connection *conn = nullptr;

	if (script.is_valid())
	{
		conn = get_or_create_connection_for_script(script);
	}
	else
	{
		conn = get_or_create_connection_for_type(p_event->get_class_name());
	}

	ERR_FAIL_NULL_MSG(conn, vformat(RTR("No connection could be found for event: %s"), p_event->to_string()));

	// Invoke event and all super-events
	while (conn != nullptr)
	{
		conn->erase_invalid_callbacks(); // Would it be optimal to find another moment to call this? Like some form of a garbage collector.
		for (KeyValue<Callable, uint32_t> &E : conn->callback_set)
		{
			E.key.call(p_event);
		}
		conn = conn->super_connection;
	}
}


void GDEventBus::subscribe(const Variant &p_type, const Callable &p_handler)
{
	switch (p_type.get_type())
	{
		case Variant::STRING:
		case Variant::STRING_NAME: {
			subscribe_type(p_type, p_handler);
			return;
		} break;
		case Variant::OBJECT: {
			Ref<Script> script = p_type;
			if (script.is_valid())
			{
				subscribe_script(script, p_handler);
				return;
			}
			Ref<GDEvent> event = p_type;
			if (event.is_valid())
			{
				subscribe_event(event, p_handler);
				return;
			}
#ifdef MODULE_GDSCRIPT_ENABLED
			Ref<GDScriptNativeClass> gd_class = p_type;
			if (gd_class.is_valid())
			{
				subscribe_type(gd_class->get_name(), p_handler);
			}
#endif // MODULE_GDSCRIPT_ENABLED
		} break;
	}
	ERR_PRINT(RTR("Subscription to invalid type attempted."));
}


void GDEventBus::unsubscribe(const Variant &p_type, const Callable &p_handler)
{
	switch (p_type.get_type())
	{
		case Variant::STRING:
		case Variant::STRING_NAME: {
			subscribe_type(p_type, p_handler);
			return;
		} break;
		case Variant::OBJECT: {
			Ref<Script> script = p_type;
			if (script.is_valid())
			{
				unsubscribe_script(script, p_handler);
				return;
			}
			Ref<GDEvent> event = p_type;
			if (event.is_valid())
			{
				unsubscribe_event(event, p_handler);
				return;
			}
#ifdef MODULE_GDSCRIPT_ENABLED
			Ref<GDScriptNativeClass> gd_class = p_type;
			if (gd_class.is_valid())
			{
				unsubscribe_type(gd_class->get_name(), p_handler);
			}
#endif // MODULE_GDSCRIPT_ENABLED
		} break;
	}
	ERR_PRINT(RTR("Unsubscription from invalid type attempted."));
}


void GDEventBus::subscribe_type(const StringName &p_type, const Callable &p_handler)
{
	Connection *conn = get_or_create_connection_for_type(p_type);
	ERR_FAIL_NULL(conn);

	conn->inc_callback(p_handler);
}


void GDEventBus::unsubscribe_type(const StringName &p_type, const Callable &p_handler)
{
	ERR_FAIL_COND(!native_connection_map.has(p_type));

	Connection *conn = native_connection_map[p_type];
	conn->dec_callback(p_handler);
}


void GDEventBus::subscribe_script(const Ref<Script> &p_script, const Callable &p_handler)
{
	Connection *conn = get_or_create_connection_for_script(p_script);
	ERR_FAIL_NULL(conn);

	conn->inc_callback(p_handler);
}


void GDEventBus::unsubscribe_script(const Ref<Script> &p_script, const Callable &p_handler)
{
	ERR_FAIL_COND(!script_connection_map.has(p_script));

	Connection *conn = script_connection_map[p_script];
	conn->dec_callback(p_handler);
}


void GDEventBus::subscribe_event(const Ref<GDEvent> &p_event, const Callable &p_handler)
{
	ERR_FAIL_COND(!p_event.is_valid());

	Ref<Script> script = p_event->get_script();
	if (script.is_valid())
	{
		subscribe_script(script, p_handler);
	}
	else
	{
		subscribe_type(p_event->get_class_name(), p_handler);
	}
}


void GDEventBus::unsubscribe_event(const Ref<GDEvent> &p_event, const Callable &p_handler)
{
	ERR_FAIL_COND(!p_event.is_valid());

	Ref<Script> script = p_event->get_script();
	if (script.is_valid())
	{
		unsubscribe_script(script, p_handler);
	}
	else
	{
		unsubscribe_type(p_event->get_class_name(), p_handler);
	}
}


GDEventBus::~GDEventBus()
{
	for (const KeyValue<Ref<Script>, Connection *> &E : script_connection_map)
	{
		memdelete(E.value);
	}
	for (const KeyValue<StringName, Connection *> &E : native_connection_map)
	{
		memdelete(E.value);
	}
}
