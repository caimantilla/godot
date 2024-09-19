#include "data_table.hpp"
#include "data_table_manager.hpp"


void DataTable::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("_iter_init"), &DataTable::_iter_init);
	ClassDB::bind_method(D_METHOD("_iter_next"), &DataTable::_iter_next);
	ClassDB::bind_method(D_METHOD("_iter_get"), &DataTable::_iter_get);

	ClassDB::bind_method(D_METHOD("get_index"), &DataTable::get_index);
	ClassDB::bind_method(D_METHOD("get_context"), &DataTable::get_context);

	ClassDB::bind_method(D_METHOD("clear"), &DataTable::bind_clear);
	ClassDB::bind_method(D_METHOD("reload"), &DataTable::bind_reload);
	ClassDB::bind_method(D_METHOD("get_id_list"), &DataTable::bind_get_id_list);

	ClassDB::bind_method(D_METHOD("get_default_category"), &DataTable::bind_get_default_category);
	ClassDB::bind_method(D_METHOD("get_record_name", "id"), &DataTable::bind_get_record_name);
	ClassDB::bind_method(D_METHOD("get_record_category", "id"), &DataTable::bind_get_record_category);
	ClassDB::bind_method(D_METHOD("get_record_description", "id"), &DataTable::bind_get_record_description);
	ClassDB::bind_method(D_METHOD("get_record_icon", "id"), &DataTable::bind_get_record_icon);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "index"), "", "get_index");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "context"), "", "get_context");

	ADD_SIGNAL(MethodInfo("changed"));
}


bool DataTable::_iter_init(const Array &p_iter)
{
	iter_idx = 0;
	iter_cache_id_count = get_record_count();
	return iter_idx < iter_cache_id_count;
}


bool DataTable::_iter_next(const Array &p_iter)
{
	iter_idx++;
	return iter_idx < iter_cache_id_count;
}


String DataTable::_iter_get(const Variant &p_iter)
{
	return get_record_id_at(iter_idx);
}


void DataTable::set_index(const uint64_t p_index)
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


void DataTable::bind_clear()
{
	clear();
}


void DataTable::bind_reload()
{
	reload();
}


PackedStringArray DataTable::bind_get_id_list() const
{
	PackedStringArray ret = get_id_list(); // please tell me that this copies the array
	return ret;
}


String DataTable::bind_get_default_category() const
{
	return editor_get_default_category();
}


String DataTable::bind_get_record_name(const String &p_id) const
{
	return editor_get_record_name(p_id);
}


String DataTable::bind_get_record_category(const String &p_id) const
{
	return editor_get_record_category(p_id);
}


String DataTable::bind_get_record_description(const String &p_id) const
{
	return editor_get_record_description(p_id);
}


Ref<Texture2D> DataTable::bind_get_record_icon(const String &p_id) const
{
	return editor_get_record_icon(p_id);
}


DataTable::DataTable()
{
	table_index = 0;
	context_id = DataTableManager::INVALID_CONTEXT;
}
