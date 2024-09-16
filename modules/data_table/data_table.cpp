#include "data_table.hpp"
#include "data_table_manager.hpp"


void DataTable::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("_iter_init"), &DataTable::_iter_init);
	ClassDB::bind_method(D_METHOD("_iter_get"), &DataTable::_iter_get);
	ClassDB::bind_method(D_METHOD("_iter_next"), &DataTable::_iter_next);

	ClassDB::bind_method(D_METHOD("get_index"), &DataTable::get_index);
	ClassDB::bind_method(D_METHOD("get_context"), &DataTable::get_context);

	ClassDB::bind_method(D_METHOD("clear"), &DataTable::bind_clear);
	ClassDB::bind_method(D_METHOD("reload"), &DataTable::bind_reload);
	ClassDB::bind_method(D_METHOD("get_id_list"), &DataTable::bind_get_id_list);

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


DataTable::DataTable()
{
	table_index = 0;
	context_id = DataTableManager::INVALID_CONTEXT;
}
