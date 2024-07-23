#ifndef MESSAGE_STATE_HPP
#define MESSAGE_STATE_HPP


#include "core/object/object.h"
#include "core/object/class_db.h"
#include "core/variant/typed_array.h"


class MessageState : public Object
{
	GDCLASS(MessageState, Object);

private:
	bool fnsh = false;

protected:
	static void _bind_methods();

public:
	void finish();
	bool is_finished() const;
	void set_variable(const StringName &p_name, const Variant &p_value);
	Variant get_variable(const StringName &p_name) const;
	bool has_variable(const StringName &p_name) const;
};


#endif // MESSAGE_STATE_HPP
