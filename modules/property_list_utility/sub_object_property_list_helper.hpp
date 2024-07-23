#ifndef SUB_OBJECT_PROPERTY_LIST_HELPER_HPP
#define SUB_OBJECT_PROPERTY_LIST_HELPER_HPP


#include "core/object/class_db.h"
#include "core/object/ref_counted.h"
#include "core/variant/typed_array.h"
#include "core/variant/callable.h"


class SubObjectPropertyListHelper : public RefCounted
{
	GDCLASS(SubObjectPropertyListHelper, RefCounted);

private:
	String prefix;
	Callable array_count_getter;
	Callable array_element_getter;
	PackedInt64Array property_usage_flag_blacklist;
	PackedInt64Array property_usage_flag_required_list;

	mutable StringName parse_last_property_cache;
	mutable int64_t parse_idx;
	mutable StringName parse_sub_property;

	bool parse_property(const StringName &p_property) const;
	bool is_property_info_valid(const PropertyInfo &p_info) const;
	int64_t get_element_count() const;
	Object *get_element_at(const int64_t p_idx) const;

protected:
	static void _bind_methods();

	TypedArray<Dictionary> bound_instance_get_property_list() const;
	Variant bound_instance_property_get_value(const StringName &p_property) const;
	Variant bound_instance_property_get_revert(const StringName &p_property) const;

public:
	void set_prefix(const String &p_prefix);
	String get_prefix() const;
	void set_array_count_getter(const Callable &p_callback);
	Callable get_array_count_getter() const;
	void set_array_element_getter(const Callable &p_callback);
	Callable get_array_element_getter() const;
	void set_property_usage_flag_blacklist(const PackedInt64Array &p_list);
	PackedInt64Array get_property_usage_flag_blacklist() const;
	void set_property_usage_flag_required_list(const PackedInt64Array &p_list);
	PackedInt64Array get_property_usage_flag_required_list() const;

	void require_property_usage_flag(const int64_t p_flag);
	void blacklist_property_usage_flag(const int64_t p_flag);

	void instance_get_property_list(List<PropertyInfo> *p_list) const;
	bool instance_has_property(const StringName &p_property) const;
	bool instance_property_set_value(const StringName &p_property, const Variant &p_value);
	bool instance_property_get_value(const StringName &p_property, Variant &r_ret) const;
	bool instance_property_can_revert(const StringName &p_property) const;
	bool instance_property_get_revert(const StringName &p_property, Variant &r_ret) const;
};


#endif // SUB_OBJECT_PROPERTY_LIST_HELPER_HPP
