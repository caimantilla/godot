#ifndef DATA_TABLE_HPP
#define DATA_TABLE_HPP


#include "core/object/object.h"
#include "core/object/class_db.h"
#include "scene/resources/texture.h"


class DataTable : public Object
{
	GDCLASS(DataTable, Object);

private:
	int64_t table_index;
	int64_t context_id;

	void bind_clear();
	void bind_reload();
	int64_t bind_get_invalid_record_id_number() const;
	String bind_get_invalid_record_id_string() const;
	PackedInt64Array bind_get_record_id_number_list() const;
	PackedStringArray bind_get_record_id_string_list() const;
	int64_t bind_convert_id_string_to_id_number(const String &p_record_id_string) const;

#ifdef TOOLS_ENABLED
	String bind_get_default_editor_category() const;
	String bind_get_record_editor_name(const int64_t p_record_id_number) const;
	String bind_get_record_editor_category(const int64_t p_record_id_number) const;
	String bind_get_record_editor_description(const int64_t p_record_id_number) const;
	Ref<Texture2D> bind_get_record_editor_icon(const int64_t p_record_id_number) const;
#endif // TOOLS_ENABLED

protected:
	static void _bind_methods();

public:
	void emit_changed();

	void set_index(const int64_t p_index);
	uint64_t get_index() const;
	void set_context(const int64_t p_context);
	int64_t get_context() const;

	virtual void clear() = 0;
	virtual void reload() = 0;
	virtual int64_t get_invalid_record_id_number() const;
	virtual String get_invalid_record_id_string() const;
	virtual PackedInt64Array get_record_id_number_list() const = 0;
	virtual PackedStringArray get_record_id_string_list() const = 0;
	virtual int64_t convert_id_string_to_id_number(const String &p_record_id_string) const;

#ifdef TOOLS_ENABLED
	virtual String get_default_editor_category() const;
	virtual String get_record_editor_name(const int64_t p_record_id_number) const;
	virtual String get_record_editor_category(const int64_t p_record_id_number) const;
	virtual String get_record_editor_description(const int64_t p_record_id_number) const;
	virtual Ref<Texture2D> get_record_editor_icon(const int64_t p_record_id_number) const;
#endif // TOOLS_ENABLED

	DataTable();
};


#endif // DATA_TABLE_HPP
