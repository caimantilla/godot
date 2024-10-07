#include "data_table_manager.hpp"
#include <limits>


DataTableManager *DataTableManager::singleton = nullptr;


void DataTableManager::_bind_methods()
{
	BIND_CONSTANT(GLOBAL_CONTEXT);
	BIND_CONSTANT(INVALID_CONTEXT);

	BIND_CONSTANT(INVALID_TABLE_INDEX);

	ClassDB::bind_method(D_METHOD("get_open_context_slot"), &DataTableManager::get_open_context_slot);
	ClassDB::bind_method(D_METHOD("get_table_count", "context"), &DataTableManager::get_table_count, DEFVAL(GLOBAL_CONTEXT));
	ClassDB::bind_method(D_METHOD("get_table_at", "idx", "context"), &DataTableManager::get_table_at, DEFVAL(GLOBAL_CONTEXT));
	ClassDB::bind_method(D_METHOD("get_table", "key", "context"), &DataTableManager::bind_get_table, DEFVAL(GLOBAL_CONTEXT));
	ClassDB::bind_method(D_METHOD("get_table_by_script", "script", "context"), &DataTableManager::get_table_by_script, DEFVAL(GLOBAL_CONTEXT));
	ClassDB::bind_method(D_METHOD("get_table_by_class_name", "class_name", "context"), &DataTableManager::get_table_by_class_name, DEFVAL(GLOBAL_CONTEXT));
	ClassDB::bind_method(D_METHOD("clear_every_table", "context"), &DataTableManager::clear_every_table, DEFVAL(GLOBAL_CONTEXT));
	ClassDB::bind_method(D_METHOD("reload_every_table", "context"), &DataTableManager::reload_every_table, DEFVAL(GLOBAL_CONTEXT));
}


DataTableManager *DataTableManager::get_singleton()
{
	return singleton;
}


DataTableManager::Context &DataTableManager::get_or_create_context(const int64_t p_context) const
{
	if (!map_context.has(p_context))
	{
		map_context[p_context].context_id = p_context;
	}
	return map_context[p_context];
}


int64_t DataTableManager::get_open_context_slot() const
{
	for (int64_t i = 1; i < std::numeric_limits<int64_t>::max(); i++)
	{
		if (!map_context.has(i))
		{
			return i;
		}
	}
	ERR_FAIL_V_MSG(INVALID_CONTEXT, RTR("No more context slots available. Returning the invalid context slot; please do not use it."));
}


DataTable *DataTableManager::bind_get_table(const Variant &p_key, const int64_t p_context) const
{
	switch (p_key.get_type())
	{
		case Variant::INT: {
			return get_table_at(p_key, p_context);
		} break;
		case Variant::OBJECT: {
			Ref<Script> script = p_key;
			if (script.is_valid())
			{
				return get_table_by_script(script, p_context);
			}
		} break;
		case Variant::STRING:
		case Variant::STRING_NAME: {
			StringName table_name = p_key;
			return get_table_by_class_name(table_name, p_context);
		} break;
		default: break;
	}
	ERR_FAIL_V(nullptr);
}


int64_t DataTableManager::get_table_count(const int64_t p_context) const
{
	Context &context = get_or_create_context(p_context);
	return context.list_table.size();
}


DataTable *DataTableManager::get_table_at(const int64_t p_idx, const int64_t p_context) const
{
	Context &context = get_or_create_context(p_context);
	ERR_FAIL_INDEX_V(p_idx, context.list_table.size(), nullptr);
	return context.list_table[p_idx];
}


DataTable *DataTableManager::get_table_by_class_name(const StringName &p_type_name, const int64_t p_context) const
{
	if (ClassDB::class_exists(p_type_name))
	{
		ERR_FAIL_COND_V(!ClassDB::is_parent_class(p_type_name, SNAME("DataTable")), nullptr);

		Context &context = get_or_create_context(p_context);

		if (context.map_native_class_name_table.has(p_type_name))
		{
			return context.map_native_class_name_table[p_type_name];
		}
		DataTable *table = Object::cast_to<DataTable>(ClassDB::instantiate(p_type_name));
		CRASH_COND(table == nullptr);

		std::type_index type_id = std::type_index(typeid(table));
		context.map_type_table.insert(type_id, table);
		context.map_native_class_name_table.insert(p_type_name, table);

		init_table(context, table);
		return table;
	}
	else if (ScriptServer::is_global_class(p_type_name))
	{
		ERR_FAIL_COND_V(ScriptServer::get_global_class_native_base(p_type_name) != SNAME("DataTableCustom"), nullptr);
		String path = ScriptServer::get_global_class_path(p_type_name);
		if (ResourceLoader::exists(path, "Script"))
		{
			Ref<Script> script = ResourceLoader::load(path, "Script");
			if (script.is_valid())
			{
				return get_table_by_script(script);
			}
		}
	}
	ERR_FAIL_V(nullptr);
}


DataTable *DataTableManager::get_table_by_script(const Ref<Script> &p_script, const int64_t p_context) const
{
	ERR_FAIL_COND_V(!p_script.is_valid(), nullptr);
	ERR_FAIL_COND_V(p_script->get_instance_base_type() != SNAME("DataTableCustom"), nullptr);

	Context &context = get_or_create_context(p_context);

	if (context.map_script_table.has(p_script))
	{
		return context.map_script_table[p_script];
	}
	else
	{
		DataTableCustom *table = memnew(DataTableCustom);
		table->set_script(p_script);

		context.map_script_table[p_script] = table;

		init_table(context, table);
		return table;
	}
}


void DataTableManager::clear_every_table(const int64_t p_context)
{
	Context &context = get_or_create_context(p_context);
	for (DataTable *table : context.list_table)
	{
		table->clear();
	}
}


void DataTableManager::reload_every_table(const int64_t p_context)
{
	Context &context = get_or_create_context(p_context);
	for (DataTable *table : context.list_table)
	{
		table->reload();
	}
}


void DataTableManager::init_table(Context &p_context, DataTable *p_table) const
{
	p_table->set_index(p_context.list_table.size());
	p_table->set_context(p_context.context_id);

	p_context.list_table.push_back(p_table);
	p_table->reload();
}


DataTableManager::DataTableManager()
{
	CRASH_COND_MSG(singleton != nullptr, RTR("DataTableManager is a singleton. Do not instantiate it multiple times."));
	singleton = this;
}


DataTableManager::~DataTableManager()
{
	for (KeyValue<int64_t, Context> &E : map_context)
	{
		for (DataTable *table : E.value.list_table)
		{
			memdelete(table);
		}
	}
	singleton = nullptr;
}
