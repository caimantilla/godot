#include "data_table.hpp"
#include "data_table_manager.hpp"


void DataTable::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("get_index"), &DataTable::get_index);
	ClassDB::bind_method(D_METHOD("get_context"), &DataTable::get_context);

	ClassDB::bind_method(D_METHOD("clear"), &DataTable::bind_clear);
	ClassDB::bind_method(D_METHOD("reload"), &DataTable::bind_reload);
	ClassDB::bind_method(D_METHOD("get_invalid_record_id_number"), &DataTable::bind_get_invalid_record_id_number);
	ClassDB::bind_method(D_METHOD("get_invalid_record_id_string"), &DataTable::bind_get_invalid_record_id_string);
	ClassDB::bind_method(D_METHOD("get_record_id_number_list"), &DataTable::bind_get_record_id_number_list);
	ClassDB::bind_method(D_METHOD("get_record_id_string_list"), &DataTable::bind_get_record_id_string_list);
	ClassDB::bind_method(D_METHOD("convert_id_number_to_id_string", "record_id_number"), &DataTable::bind_convert_id_number_to_id_string);
	ClassDB::bind_method(D_METHOD("convert_id_string_to_id_number", "record_id_string"), &DataTable::bind_convert_id_string_to_id_number);

#ifdef TOOLS_ENABLED
	ClassDB::bind_method(D_METHOD("get_default_editor_category"), &DataTable::bind_get_default_editor_category);
	ClassDB::bind_method(D_METHOD("get_record_editor_name", "record_id_number"), &DataTable::bind_get_record_editor_name);
	ClassDB::bind_method(D_METHOD("get_record_editor_category", "record_id_number"), &DataTable::bind_get_record_editor_category);
	ClassDB::bind_method(D_METHOD("get_record_editor_description", "record_id_number"), &DataTable::bind_get_record_editor_description);
	ClassDB::bind_method(D_METHOD("get_record_editor_icon", "record_id_number"), &DataTable::bind_get_record_editor_icon);
#endif // TOOLS_ENABLED

	ADD_PROPERTY(PropertyInfo(Variant::INT, "index"), "", "get_index");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "context"), "", "get_context");

	ADD_SIGNAL(MethodInfo("changed"));
}


void DataTable::emit_changed()
{
	emit_signal(CoreStringName(changed));
}


void DataTable::set_index(const int64_t p_index)
{
	table_index = p_index;
}


uint64_t DataTable::get_index() const
{
	return table_index;
}


void DataTable::set_context(const int64_t p_context)
{
	context_id = p_context;
}


int64_t DataTable::get_context() const
{
	return context_id;
}


int64_t DataTable::get_invalid_record_id_number() const
{
	return -1; // I don't know if the default should be 0 or 1. -1 is generally used across Godot to denote invalid IDs, but GDScript int defaults to 0...
}


String DataTable::get_invalid_record_id_string() const
{
	return String();
}


String DataTable::convert_id_number_to_id_string(const int64_t p_record_id_number) const
{
	return get_invalid_record_id_string();
}


int64_t DataTable::convert_id_string_to_id_number(const String &p_record_id_string) const
{
	return get_invalid_record_id_number();
}


void DataTable::bind_clear()
{
	clear();
}


void DataTable::bind_reload()
{
	reload();
}


int64_t DataTable::bind_get_invalid_record_id_number() const
{
	return get_invalid_record_id_number();
}


String DataTable::bind_get_invalid_record_id_string() const
{
	return get_invalid_record_id_string();
}


PackedInt64Array DataTable::bind_get_record_id_number_list() const
{
	return get_record_id_number_list();
}


PackedStringArray DataTable::bind_get_record_id_string_list() const
{
	return get_record_id_string_list();
}


String DataTable::bind_convert_id_number_to_id_string(const int64_t p_record_id_number) const
{
	return convert_id_number_to_id_string(p_record_id_number);
}


int64_t DataTable::bind_convert_id_string_to_id_number(const String &p_record_id_string) const
{
	return convert_id_string_to_id_number(p_record_id_string);
}


#ifdef TOOLS_ENABLED
String DataTable::get_default_editor_category() const
{
	return String();
}


String DataTable::get_record_editor_name(const int64_t p_record_id_number) const
{
	return String();
}


String DataTable::get_record_editor_category(const int64_t p_record_id_number) const
{
	return String();
}


String DataTable::get_record_editor_description(const int64_t p_record_id_number) const
{
	return String();
}


Ref<Texture2D> DataTable::get_record_editor_icon(const int64_t p_record_id_number) const
{
	return Ref<Texture2D>();
}


String DataTable::bind_get_default_editor_category() const
{
	return get_default_editor_category();
}


String DataTable::bind_get_record_editor_name(const int64_t p_record_id_number) const
{
	return get_record_editor_name(p_record_id_number);
}


String DataTable::bind_get_record_editor_category(const int64_t p_record_id_number) const
{
	return get_record_editor_category(p_record_id_number);
}


String DataTable::bind_get_record_editor_description(const int64_t p_record_id_number) const
{
	return get_record_editor_description(p_record_id_number);
}


Ref<Texture2D> DataTable::bind_get_record_editor_icon(const int64_t p_record_id_number) const
{
	return get_record_editor_icon(p_record_id_number);
}
#endif // TOOLS_ENABLED


DataTable::DataTable()
{
	table_index = DataTableManager::INVALID_TABLE_INDEX;
	context_id = DataTableManager::INVALID_CONTEXT;
}
