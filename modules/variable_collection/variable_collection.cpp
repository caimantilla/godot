#include "variable_collection.hpp"


void VariableCollection::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("_iter_init"), &VariableCollection::_iter_init);
	ClassDB::bind_method(D_METHOD("_iter_next"), &VariableCollection::_iter_next);
	ClassDB::bind_method(D_METHOD("_iter_get"), &VariableCollection::_iter_get);

	ClassDB::bind_method(D_METHOD("set_variable", "key", "value"), &VariableCollection::set_variable);
	ClassDB::bind_method(D_METHOD("get_variable", "key", "warn_if_missing"), &VariableCollection::get_variable, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("has_variable", "key"), &VariableCollection::has_variable);
	ClassDB::bind_method(D_METHOD("erase_variable", "key"), &VariableCollection::erase_variable);
	ClassDB::bind_method(D_METHOD("get_keys"), &VariableCollection::bind_get_keys);
	ClassDB::bind_method(D_METHOD("clear"), &VariableCollection::clear);

	ADD_SIGNAL(MethodInfo("variable_added", PropertyInfo(Variant::STRING, "key")));
	ADD_SIGNAL(MethodInfo("removing_variable", PropertyInfo(Variant::STRING, "key")));
	ADD_SIGNAL(MethodInfo("variable_removed", PropertyInfo(Variant::STRING, "key")));
	ADD_SIGNAL(MethodInfo("variable_changed", PropertyInfo(Variant::STRING, "key"), PropertyInfo(Variant::NIL, "from"), PropertyInfo(Variant::NIL, "to")));
}


bool VariableCollection::_iter_init(const Array &p_iter)
{
	update_cache();
	iter_idx = 0;
	return iter_idx < cache_name_list.size();
}


bool VariableCollection::_iter_next(const Array &p_iter)
{
	iter_idx++;
	return iter_idx < cache_name_list.size();
}


String VariableCollection::_iter_get(const Variant &p_iter)
{
	return cache_name_list[iter_idx];
}


void VariableCollection::set_variable(const String &p_key, const Variant &p_value)
{
	ERR_FAIL_COND_MSG(p_key.is_empty(), RTR("Key cannot be empty."));
	if (value_map.has(p_key))
	{
		Variant prev = value_map[p_key];
		if (prev != p_value)
		{
			value_map[p_key] = p_value;
			emit_signal(SNAME("variable_changed"), p_key, prev, p_value);
		}
	}
	else
	{
		cache_dirty = true;
		value_map[p_key] = p_value;
		emit_signal(SNAME("variable_added"), p_key);
	}
}


Variant VariableCollection::get_variable(const String &p_key, const bool p_err_if_missing) const
{
	ERR_FAIL_COND_V_MSG(p_key.is_empty(), Variant(), RTR("Key cannot be empty."));
	if (p_err_if_missing)
	{
		ERR_FAIL_COND_V_MSG(!value_map.has(p_key), Variant(), vformat(RTR("Key \"%s\" is not present in the collection."), p_key));
	}
	else if (!value_map.has(p_key))
	{
		return Variant();
	}
	return value_map[p_key];
}


bool VariableCollection::erase_variable(const String &p_key)
{
	ERR_FAIL_COND_V_MSG(p_key.is_empty(), false, RTR("Key cannot be empty."));
	ERR_FAIL_COND_V_MSG(!value_map.has(p_key), false, vformat(RTR("Key \"%s\" is not present in the collection."), p_key));

	emit_signal(SNAME("removing_variable"), p_key);
	cache_dirty = true;
	value_map.erase(p_key);
	emit_signal(SNAME("variable_removed"), p_key);
	return true;
}


bool VariableCollection::has_variable(const String &p_key) const
{
	ERR_FAIL_COND_V_MSG(p_key.is_empty(), false, RTR("Key cannot be empty."));
	return value_map.has(p_key);
}


const PackedStringArray &VariableCollection::get_keys() const
{
	update_cache();
	return cache_name_list;
}


void VariableCollection::clear()
{
	if (value_map.is_empty())
	{
		return;
	}

	update_cache();

	for (const String &key : cache_name_list)
	{
		emit_signal(SNAME("removing_variable"), key);
	}

	value_map.clear();
	cache_dirty = true;

	for (const String &key : cache_name_list)
	{
		emit_signal(SNAME("variable_removed"), key);
	}
}


void VariableCollection::update_cache() const
{
	if (!cache_dirty)
	{
		return;
	}
	cache_dirty = false;
	cache_name_list.resize(value_map.size());
	int i = 0;
	for (HashMap<String, Variant>::Iterator E = value_map.begin(); E; ++E)
	{
		cache_name_list.write[i] = E->key;
		i++;
	}
}


PackedStringArray VariableCollection::bind_get_keys() const
{
	return get_keys(); // return a copy rather than a reference
}
