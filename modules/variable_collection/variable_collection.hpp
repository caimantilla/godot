#ifndef VARIABLE_COLLECTION_HPP
#define VARIABLE_COLLECTION_HPP


#include "core/object/ref_counted.h"


class VariableCollection final : public RefCounted
{
	GDCLASS(VariableCollection, RefCounted);

private:
	uint64_t iter_idx;

	mutable HashMap<String, Variant> value_map;
	mutable bool cache_dirty = false;
	mutable PackedStringArray cache_name_list;

	void update_cache() const;
	PackedStringArray bind_get_keys() const;

	bool _iter_init(const Array &p_iter);
	bool _iter_next(const Array &p_iter);
	String _iter_get(const Variant &p_iter);

protected:
	static void _bind_methods();

public:
	void set_variable(const String &p_key, const Variant &p_value);
	Variant get_variable(const String &p_key, const bool p_err_if_missing = true) const;
	bool has_variable(const String &p_key) const;
	bool erase_variable(const String &p_key);
	const PackedStringArray &get_keys() const;
	void clear();
};


#endif // VARIABLE_COLLECTION_HPP
