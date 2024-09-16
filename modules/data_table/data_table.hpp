#ifndef DATA_TABLE_HPP
#define DATA_TABLE_HPP


#include "core/object/object.h"
#include "core/object/class_db.h"
#include "scene/resources/texture.h"


class DataTable : public Object
{
	friend class DataTableManager;

	GDCLASS(DataTable, Object);

private:
	uint64_t table_index;
	int64_t context_id;

	uint64_t iter_idx;
	uint64_t iter_cache_id_count;

	bool _iter_init(const Array &p_iter);
	bool _iter_next(const Array &p_iter);
	String _iter_get(const Variant &p_iter);

	void bind_clear();
	void bind_reload();
	PackedStringArray bind_get_id_list() const;

protected:
	static void _bind_methods();

public:
	void set_index(const uint64_t p_index);
	uint64_t get_index() const;
	void set_context(const int64_t p_context);
	int64_t get_context() const;

	virtual void clear() = 0;
	virtual void reload() = 0;
	virtual const PackedStringArray &get_id_list() const = 0;
	virtual const uint64_t get_record_count() const = 0;
	virtual const String &get_record_id_at(const uint64_t p_idx) const = 0;

	virtual String editor_get_default_category() const { return ""; }
	virtual String editor_get_record_name(const String &p_id) const { return p_id; }
	virtual String editor_get_record_category(const String &p_id) const { return ""; }
	virtual String editor_get_record_description(const String &p_id) const { return ""; }
	virtual Ref<Texture2D> editor_get_record_icon(const String &p_id) const { return Ref<Texture2D>(); }

	DataTable();
};


#endif // DATA_TABLE_HPP
