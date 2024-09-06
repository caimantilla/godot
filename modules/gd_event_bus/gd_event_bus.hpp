#ifndef GD_EVENT_BUS_HPP
#define GD_EVENT_BUS_HPP


#include "core/object/script_language.h"
#include "scene/main/node.h"
#include "gd_event.hpp"


class GDEventBus final : public Node
{
	GDCLASS(GDEventBus, Node);

	class Connection final
	{
	public:
		Connection *super_connection = nullptr; // reference to the superclass if it exists
		HashMap<Callable, uint32_t> callback_set;

		const bool inc_callback(const Callable &p_ref);
		const bool dec_callback(const Callable &p_ref);
		const bool erase_callback(const Callable &p_ref);
		void erase_invalid_callbacks();
	};

private:
	HashMap<StringName, Connection *> native_connection_map;
	HashMap<Ref<Script>, Connection *> script_connection_map;

	Connection *create_connection_object_from_type(const StringName &p_type);
	Connection *create_connection_object_from_script(const Ref<Script> &p_script);
	const bool is_type_valid(const StringName &p_type) const;
	const bool is_script_valid(const Ref<Script> &p_script) const;

	Connection *get_or_create_connection_for_type(const StringName &p_type);
	Connection *get_or_create_connection_for_script(const Ref<Script> &p_script);

protected:
	static void _bind_methods();

public:
	void invoke(const Ref<GDEvent> &p_event);

	void subscribe(const Variant &p_type, const Callable &p_handler);
	void unsubscribe(const Variant &p_type, const Callable &p_handler);

	void subscribe_type(const StringName &p_type, const Callable &p_handler);
	void unsubscribe_type(const StringName &p_type, const Callable &p_handler);
	void subscribe_script(const Ref<Script> &p_script, const Callable &p_handler);
	void unsubscribe_script(const Ref<Script> &p_script, const Callable &p_handler);
	void subscribe_event(const Ref<GDEvent> &p_event, const Callable &p_handler);
	void unsubscribe_event(const Ref<GDEvent> &p_event, const Callable &p_handler);

	~GDEventBus();
};


#endif // GD_EVENT_BUS_HPP
