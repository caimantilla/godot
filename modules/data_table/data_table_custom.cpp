#include "data_table_custom.hpp"


void DataTableCustom::_bind_methods()
{
	GDVIRTUAL_BIND(_clear_table);
	GDVIRTUAL_BIND(_load_table);
	GDVIRTUAL_BIND(_get_record_id_list);

	GDVIRTUAL_BIND(_editor_get_default_category);
	GDVIRTUAL_BIND(_editor_get_record_name, "id");
	GDVIRTUAL_BIND(_editor_get_record_category, "id");
	GDVIRTUAL_BIND(_editor_get_record_description, "id");
	GDVIRTUAL_BIND(_editor_get_record_icon, "id");
}


void DataTableCustom::clear()
{
	if (!GDVIRTUAL_CALL(_clear_table))
	{
		ERR_PRINT(vformat(RTR("_clear_table must be implemented for table: %s"), this->to_string()));
		return;
	}
	cache_id_list.clear();
	emit_signal(CoreStringName(changed));
}


void DataTableCustom::reload()
{
	if (!GDVIRTUAL_CALL(_clear_table))
	{
		ERR_PRINT(vformat(RTR("_clear_table must be implemented for table: %s"), this->to_string()));
		return;
	}
	bool load_ok = true;
	if (!GDVIRTUAL_CALL(_load_table))
	{
		ERR_PRINT(RTR("_load_table must be implemented for table: %s") + this->to_string());
		load_ok = false;
	}
	if (!GDVIRTUAL_CALL(_get_record_id_list, cache_id_list))
	{
		ERR_PRINT(RTR("_get_record_id_list must be implemented for table: %s") + this->to_string());
		load_ok = false;
	}
	if (!load_ok)
	{
		cache_id_list.clear();
	}
	emit_signal(CoreStringName(changed));
}


const PackedStringArray &DataTableCustom::get_id_list() const
{
	return cache_id_list;
}


const uint64_t DataTableCustom::get_record_count() const
{
	return cache_id_list.size();
}


const String &DataTableCustom::get_record_id_at(const uint64_t p_idx) const
{
	static const String dummy = "";
	ERR_FAIL_INDEX_V(p_idx, cache_id_list.size(), dummy);
	return cache_id_list[p_idx];
}


String DataTableCustom::editor_get_default_category() const
{
	String virtual_result;
	GDVIRTUAL_CALL(_editor_get_default_category, virtual_result);
	return virtual_result;
}


String DataTableCustom::editor_get_record_name(const String &p_id) const
{
	String virtual_result;
	GDVIRTUAL_CALL(_editor_get_record_name, p_id, virtual_result);
	return virtual_result;
}


String DataTableCustom::editor_get_record_category(const String &p_id) const
{
	String virtual_result;
	GDVIRTUAL_CALL(_editor_get_record_category, p_id, virtual_result);
	return virtual_result;
}


String DataTableCustom::editor_get_record_description(const String &p_id) const
{
	String virtual_result;
	GDVIRTUAL_CALL(_editor_get_record_description, p_id, virtual_result);
	return virtual_result;
}


Ref<Texture2D> DataTableCustom::editor_get_record_icon(const String &p_id) const
{
	Ref<Texture2D> virtual_result;
	GDVIRTUAL_CALL(_editor_get_record_icon, p_id, virtual_result);
	return virtual_result;
}
