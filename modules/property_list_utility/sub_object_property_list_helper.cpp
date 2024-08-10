#include "sub_object_property_list_helper.hpp"


void SubObjectPropertyListHelper::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_prefix", "prefix"), &SubObjectPropertyListHelper::set_prefix);
	ClassDB::bind_method(D_METHOD("get_prefix"), &SubObjectPropertyListHelper::get_prefix);
	ClassDB::bind_method(D_METHOD("set_array_count_getter", "callback"), &SubObjectPropertyListHelper::set_array_count_getter);
	ClassDB::bind_method(D_METHOD("get_array_count_getter"), &SubObjectPropertyListHelper::get_array_count_getter);
	ClassDB::bind_method(D_METHOD("set_array_element_getter", "callback"), &SubObjectPropertyListHelper::set_array_element_getter);
	ClassDB::bind_method(D_METHOD("get_array_element_getter"), &SubObjectPropertyListHelper::get_array_element_getter);
	ClassDB::bind_method(D_METHOD("set_property_usage_flag_blacklist", "flags"), &SubObjectPropertyListHelper::set_property_usage_flag_blacklist);
	ClassDB::bind_method(D_METHOD("get_property_usage_flag_blacklist"), &SubObjectPropertyListHelper::get_property_usage_flag_blacklist);
	ClassDB::bind_method(D_METHOD("set_property_usage_flag_required_list", "flags"), &SubObjectPropertyListHelper::set_property_usage_flag_required_list);
	ClassDB::bind_method(D_METHOD("get_property_usage_flag_required_list"), &SubObjectPropertyListHelper::get_property_usage_flag_required_list);

	ClassDB::bind_method(D_METHOD("require_property_usage_flag", "flag"), &SubObjectPropertyListHelper::require_property_usage_flag);
	ClassDB::bind_method(D_METHOD("blacklist_property_usage_flag", "flag"), &SubObjectPropertyListHelper::blacklist_property_usage_flag);

	ClassDB::bind_method(D_METHOD("instance_get_property_list"), &SubObjectPropertyListHelper::bound_instance_get_property_list);
	ClassDB::bind_method(D_METHOD("instance_has_property", "property"), &SubObjectPropertyListHelper::instance_has_property);
	ClassDB::bind_method(D_METHOD("instance_property_set_value", "property", "value"), &SubObjectPropertyListHelper::instance_property_set_value);
	ClassDB::bind_method(D_METHOD("instance_property_get_value", "property"), &SubObjectPropertyListHelper::bound_instance_property_get_value);
	ClassDB::bind_method(D_METHOD("instance_property_can_revert", "property"), &SubObjectPropertyListHelper::instance_property_can_revert);
	ClassDB::bind_method(D_METHOD("instance_property_get_revert", "property"), &SubObjectPropertyListHelper::bound_instance_property_get_revert);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "prefix"), "set_prefix", "get_prefix");
	ADD_PROPERTY(PropertyInfo(Variant::CALLABLE, "array_count_getter"), "set_array_count_getter", "get_array_count_getter");
	ADD_PROPERTY(PropertyInfo(Variant::CALLABLE, "array_element_getter"), "set_array_element_getter", "get_array_element_getter");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT64_ARRAY, "property_usage_flag_blacklist"), "set_property_usage_flag_blacklist", "get_property_usage_flag_blacklist");
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT64_ARRAY, "property_usage_flag_required_list"), "set_property_usage_flag_required_list", "get_property_usage_flag_required_list");
}


TypedArray<Dictionary> SubObjectPropertyListHelper::bound_instance_get_property_list() const
{
	List<PropertyInfo> p_list;
	instance_get_property_list(&p_list);
	return convert_property_list(&p_list);
}


Variant SubObjectPropertyListHelper::bound_instance_property_get_value(const StringName &p_property) const
{
	Variant ret;
	if (instance_property_get_value(p_property, ret))
		return ret;
	else
		return Variant();
}


Variant SubObjectPropertyListHelper::bound_instance_property_get_revert(const StringName &p_property) const
{
	Variant ret;
	if (instance_property_get_revert(p_property, ret))
		return ret;
	else
		return Variant();
}


bool SubObjectPropertyListHelper::parse_property(const StringName &p_property) const
{
	if (p_property == parse_last_property_cache)
		return true;
	
	String name = p_property;
	if (name.begins_with(prefix))
	{
		parse_last_property_cache = p_property;

		String latter = name.substr(prefix.length());
		String idx_str = latter.get_slicec('/', 0);
		parse_idx = idx_str.to_int();
		parse_sub_property = latter.substr(idx_str.length() + 1);

		return true;
	}

	parse_last_property_cache = StringName();
	return false;
}


bool SubObjectPropertyListHelper::is_property_info_valid(const PropertyInfo &p_info) const
{
	if (p_info.type == Variant::NIL || p_info.name == "script")
		return false;
	
	uint32_t usage = p_info.usage;
	
	for (int64_t flag : property_usage_flag_blacklist)
	{
		if (usage & flag)
			return false;
	}
	for (int64_t flag : property_usage_flag_required_list)
	{
		if (!(usage & flag))
			return false;
	}

	return true;
}


int64_t SubObjectPropertyListHelper::get_element_count() const
{
	ERR_FAIL_COND_V_MSG(!array_count_getter.is_valid(), 0, "A valid array count getter must be assigned.");
	Variant count_var;
	Callable::CallError call_err;
	array_count_getter.callp(nullptr, 0, count_var, call_err);
	ERR_FAIL_COND_V(call_err.error != Callable::CallError::CALL_OK, 0);
	ERR_FAIL_COND_V_MSG(count_var.get_type() != Variant::INT, 0, "Array count getter must return an int.");
	return (int64_t)count_var;
}


Object *SubObjectPropertyListHelper::get_element_at(const int64_t p_idx) const
{
	ERR_FAIL_COND_V_MSG(!array_element_getter.is_valid(), nullptr, "A valid array element getter must be assigned.");

	Variant arg = p_idx;
	const Variant **argptr = (const Variant **)alloca(sizeof(Variant *));
	argptr[0] = &arg;

	Variant element_var;
	Callable::CallError call_err;
	array_element_getter.callp(argptr, 1, element_var, call_err);

	ERR_FAIL_COND_V(call_err.error != Callable::CallError::CALL_OK, nullptr);
	ERR_FAIL_COND_V_MSG(element_var.get_type() != Variant::OBJECT, nullptr, "Array element getter must return an object.");

	Object *element = element_var;
	ERR_FAIL_NULL_V_MSG(element, nullptr, "Array element getter should not return null.");
	return element;
}


void SubObjectPropertyListHelper::set_prefix(const String &p_prefix)
{
	prefix = p_prefix;
}


String SubObjectPropertyListHelper::get_prefix() const
{
	return prefix;
}


void SubObjectPropertyListHelper::set_array_count_getter(const Callable &p_callback)
{
	array_count_getter = p_callback;
}


Callable SubObjectPropertyListHelper::get_array_count_getter() const
{
	return array_count_getter;
}


void SubObjectPropertyListHelper::set_array_element_getter(const Callable &p_callback)
{
	array_element_getter = p_callback;
}


Callable SubObjectPropertyListHelper::get_array_element_getter() const
{
	return array_element_getter;
}


void SubObjectPropertyListHelper::set_property_usage_flag_blacklist(const PackedInt64Array &p_list)
{
	property_usage_flag_blacklist = p_list;
}


PackedInt64Array SubObjectPropertyListHelper::get_property_usage_flag_blacklist() const
{
	return property_usage_flag_blacklist;
}


void SubObjectPropertyListHelper::set_property_usage_flag_required_list(const PackedInt64Array &p_list)
{
	property_usage_flag_required_list = p_list;
}


PackedInt64Array SubObjectPropertyListHelper::get_property_usage_flag_required_list() const
{
	return property_usage_flag_required_list;
}


void SubObjectPropertyListHelper::require_property_usage_flag(const int64_t p_flag)
{
	property_usage_flag_required_list.push_back(p_flag);
}


void SubObjectPropertyListHelper::blacklist_property_usage_flag(const int64_t p_flag)
{
	property_usage_flag_blacklist.push_back(p_flag);
}


void SubObjectPropertyListHelper::instance_get_property_list(List<PropertyInfo> *p_list) const
{
	ERR_FAIL_NULL(p_list);

	int64_t count = get_element_count();
	for (int64_t i = 0; i < count; i++)
	{
		Object *element = get_element_at(i);
		ERR_FAIL_NULL(element);

		List<PropertyInfo> element_p_list;
		element->get_property_list(&element_p_list);

		for (PropertyInfo &e_p_info : element_p_list)
		{
			if (!is_property_info_valid(e_p_info))
				continue;
			// Configure nested property arrays, eg. "Emotions,emotion_" -> "Emotions,character_0/emotion_"
			if (e_p_info.usage & PROPERTY_USAGE_ARRAY)
			{
				String og_cn = e_p_info.class_name; // cast to string once instead of twice :3
				String left = og_cn.get_slicec(',', 0);
				String right = og_cn.get_slicec(',', 1);
				e_p_info.class_name = left + ',' + prefix + itos(i) + '/' + right;
			}
			e_p_info.name = (prefix + itos(i) + '/' + e_p_info.name);
			p_list->push_back(e_p_info);
		}
	}
}


bool SubObjectPropertyListHelper::instance_has_property(const StringName &p_property) const
{
	Variant dummy;
	return instance_property_get_value(p_property, dummy);
}


bool SubObjectPropertyListHelper::instance_property_set_value(const StringName &p_property, const Variant &p_value)
{
	if (parse_property(p_property))
	{
		Object *element = get_element_at(parse_idx);
		ERR_FAIL_NULL_V(element, false);

		bool ret = false;
		element->set(parse_sub_property, p_value, &ret);
		return ret;
	}
	return false;
}


bool SubObjectPropertyListHelper::instance_property_get_value(const StringName &p_property, Variant &r_ret) const
{
	if (parse_property(p_property))
	{
		Object *element = get_element_at(parse_idx);
		ERR_FAIL_NULL_V(element, false);

		bool ret = false;
		r_ret = element->get(parse_sub_property, &ret);
		return ret;
	}
	return false;
}


bool SubObjectPropertyListHelper::instance_property_can_revert(const StringName &p_property) const
{
	if (parse_property(p_property))
	{
		Object *element = get_element_at(parse_idx);
		ERR_FAIL_NULL_V(element, false);
		return element->property_can_revert(parse_sub_property);
	}
	return false;
}


bool SubObjectPropertyListHelper::instance_property_get_revert(const StringName &p_property, Variant &r_ret) const
{
	if (parse_property(p_property))
	{
		Object *element = get_element_at(parse_idx);
		ERR_FAIL_NULL_V(element, false);
		r_ret = element->property_get_revert(parse_sub_property);
		return true;
	}
	return false;
}
