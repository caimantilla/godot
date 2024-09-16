#ifndef DATA_TABLE_MANAGER_HPP
#define DATA_TABLE_MANAGER_HPP


#include <typeindex>
#include <type_traits>
#include "data_table.hpp"
#include "data_table_custom.hpp"
#include "core/object/object.h"
#include "core/object/script_language.h"


class DataTableManager final : public Object
{
	GDCLASS(DataTableManager, Object);

	struct NativeTypeHasher final
	{
		static _FORCE_INLINE_ uint32_t hash(const std::type_index &p_key) { return p_key.hash_code(); }
	};

	struct Context final
	{
		int64_t context_id;

		mutable Vector<DataTable *> list_table;
		mutable HashMap<std::type_index, DataTable *, NativeTypeHasher> map_type_table;
		mutable HashMap<StringName, DataTable *> map_native_class_name_table;
		mutable HashMap<Ref<Script>, DataTableCustom *> map_script_table;
	};

public:
	enum : int64_t
	{
		GLOBAL_CONTEXT = 0,
		INVALID_CONTEXT = -1,
	};

private:
	static DataTableManager *singleton;

	mutable HashMap<int64_t, Context> map_context;

	Context &get_or_create_context(const int64_t p_context) const;
	DataTable *bind_get_table(const Variant &p_key, const int64_t p_context = GLOBAL_CONTEXT) const;
	void init_table(Context &p_context, DataTable *p_table) const;

protected:
	static void _bind_methods();

public:
	static DataTableManager *get_singleton();

	int64_t get_open_context_slot() const;
	uint64_t get_table_count(const int64_t p_context = GLOBAL_CONTEXT) const;
	DataTable *get_table_at(const uint64_t p_idx, const int64_t p_context = GLOBAL_CONTEXT) const;
	DataTable *get_table_by_class_name(const StringName &p_type_name, const int64_t p_context = GLOBAL_CONTEXT) const;
	DataTable *get_table_by_script(const Ref<Script> &p_script, const int64_t p_context = GLOBAL_CONTEXT) const;
	void clear_every_table(const int64_t p_context = GLOBAL_CONTEXT);
	void reload_every_table(const int64_t p_context = GLOBAL_CONTEXT);

	template <typename T>
	typename std::enable_if<std::is_base_of<DataTable, T>::value, T>::type *
	get_table(const int64_t p_context = GLOBAL_CONTEXT) const
	{
		Context &context = get_or_create_context(p_context);
		std::type_index type_index = std::type_index(typeid(T));
		if (!context.map_type_table.has(type_index))
		{
			DataTable *table = memnew(T);

			context.map_type_table[type_index] = table;
			context.map_native_class_name_table[table->get_class_name()] = table;

			init_table(context, table);
			return table;
		}
		else
		{
			return context.map_type_table[type_index];
		}
	}

	DataTableManager();
	~DataTableManager();
};


#endif // DATA_TABLE_MANAGER_HPP
