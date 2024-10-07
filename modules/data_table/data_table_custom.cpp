#include "data_table_custom.hpp"


void DataTableCustom::_bind_methods()
{
	GDVIRTUAL_BIND(_clear_table);
	GDVIRTUAL_BIND(_load_table);
	GDVIRTUAL_BIND(_get_invalid_record_id_number);
	GDVIRTUAL_BIND(_get_invalid_record_id_string);
	GDVIRTUAL_BIND(_get_record_id_number_list);
	GDVIRTUAL_BIND(_get_record_id_string_list);
	GDVIRTUAL_BIND(_convert_id_string_to_id_number, "record_id_string");

#ifdef TOOLS_ENABLED
	GDVIRTUAL_BIND(_get_default_editor_category);
	GDVIRTUAL_BIND(_get_record_editor_name, "record_id_number");
	GDVIRTUAL_BIND(_get_record_editor_category, "record_id_number");
	GDVIRTUAL_BIND(_get_record_editor_description, "record_id_number");
	GDVIRTUAL_BIND(_get_record_editor_icon, "record_id_number");
#endif // TOOLS_ENABLED
}


void DataTableCustom::clear()
{
	if (!GDVIRTUAL_CALL(_clear_table))
	{
		ERR_PRINT(RTR("_clear_table must be implemented."));
		return;
	}
	cache_id_number_list.clear();
	cache_id_string_list.clear();
	emit_changed();
}


void DataTableCustom::reload()
{
	if (!GDVIRTUAL_CALL(_clear_table))
	{
		ERR_PRINT(RTR("_clear_table must be implemented."));
		return;
	}
	bool load_ok = true;
	if (!GDVIRTUAL_CALL(_load_table))
	{
		ERR_PRINT(RTR("_load_table must be implemented."));
		load_ok = false;
	}
	if (!GDVIRTUAL_CALL(_get_record_id_number_list, cache_id_number_list))
	{
		cache_id_number_list.clear();
	}
	if (!GDVIRTUAL_CALL(_get_record_id_string_list, cache_id_string_list))
	{
		cache_id_string_list.clear();
	}
	if (!load_ok)
	{
		cache_id_number_list.clear();
		cache_id_string_list.clear();
	}
	emit_changed();
}


int64_t DataTableCustom::get_invalid_record_id_number() const
{
	int64_t ret;
	if (!GDVIRTUAL_CALL(_get_invalid_record_id_number, ret))
	{
		ret = DataTable::get_invalid_record_id_number();
	}
	return ret;
}


String DataTableCustom::get_invalid_record_id_string() const
{
	String ret;
	if (!GDVIRTUAL_CALL(_get_invalid_record_id_string, ret))
	{
		ret = DataTable::get_invalid_record_id_string();
	}
	return ret;
}


PackedInt64Array DataTableCustom::get_record_id_number_list() const
{
	return cache_id_number_list;
}


PackedStringArray DataTableCustom::get_record_id_string_list() const
{
	return cache_id_string_list;
}


int64_t DataTableCustom::convert_id_string_to_id_number(const String &p_record_id_string) const
{
	int64_t ret;
	if (!GDVIRTUAL_CALL(_convert_id_string_to_id_number, p_record_id_string, ret))
	{
		ERR_PRINT(RTR("_convert_id_string_to_id_number must be implemented."));
		ret = get_invalid_record_id_number();
	}
	return ret;
}


#ifdef TOOLS_ENABLED
String DataTableCustom::get_default_editor_category() const
{
	String ret;
	GDVIRTUAL_CALL(_get_default_editor_category, ret);
	return ret;
}


String DataTableCustom::get_record_editor_name(const int64_t p_record_id_number) const
{
	String ret;
	GDVIRTUAL_CALL(_get_record_editor_name, p_record_id_number, ret);
	return ret;
}


String DataTableCustom::get_record_editor_category(const int64_t p_record_id_number) const
{
	String ret;
	GDVIRTUAL_CALL(_get_record_editor_category, p_record_id_number, ret);
	return ret;
}


String DataTableCustom::get_record_editor_description(const int64_t p_record_id_number) const
{
	String ret;
	GDVIRTUAL_CALL(_get_record_editor_description, p_record_id_number, ret);
	return ret;
}


Ref<Texture2D> DataTableCustom::get_record_editor_icon(const int64_t p_record_id_number) const
{
	Ref<Texture2D> ret;
	GDVIRTUAL_CALL(_get_record_editor_icon, p_record_id_number, ret);
	return ret;
}
#endif // TOOLS_ENABLED
