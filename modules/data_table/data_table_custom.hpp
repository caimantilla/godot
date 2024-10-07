#ifndef DATA_TABLE_CUSTOM_HPP
#define DATA_TABLE_CUSTOM_HPP


#include "data_table.hpp"
#include "core/object/gdvirtual.gen.inc"


class DataTableCustom final : public DataTable
{
	GDCLASS(DataTableCustom, DataTable);

private:
	PackedInt64Array cache_id_number_list;
	PackedStringArray cache_id_string_list;

protected:
	static void _bind_methods();

	GDVIRTUAL0(_clear_table);
	GDVIRTUAL0(_load_table);
	GDVIRTUAL0RC(int64_t, _get_invalid_record_id_number);
	GDVIRTUAL0RC(String, _get_invalid_record_id_string);
	GDVIRTUAL0RC(PackedInt64Array, _get_record_id_number_list);
	GDVIRTUAL0RC(PackedStringArray, _get_record_id_string_list);
	GDVIRTUAL1RC(int64_t, _convert_id_string_to_id_number, String);

#ifdef TOOLS_ENABLED
	GDVIRTUAL0RC(String, _get_default_editor_category);
	GDVIRTUAL1RC(String, _get_record_editor_name, int64_t);
	GDVIRTUAL1RC(String, _get_record_editor_category, int64_t);
	GDVIRTUAL1RC(String, _get_record_editor_description, int64_t);
	GDVIRTUAL1RC(Ref<Texture2D>, _get_record_editor_icon, int64_t);
#endif // TOOLS_ENABLED

public:
	virtual void clear() override;
	virtual void reload() override;
	virtual int64_t get_invalid_record_id_number() const override;
	virtual String get_invalid_record_id_string() const override;
	virtual PackedInt64Array get_record_id_number_list() const override;
	virtual PackedStringArray get_record_id_string_list() const override;
	virtual int64_t convert_id_string_to_id_number(const String &p_record_id_string) const override;

#ifdef TOOLS_ENABLED
	virtual String get_default_editor_category() const override;
	virtual String get_record_editor_name(const int64_t p_record_id_number) const override;
	virtual String get_record_editor_category(const int64_t p_record_id_number) const override;
	virtual String get_record_editor_description(const int64_t p_record_id_number) const override;
	virtual Ref<Texture2D> get_record_editor_icon(const int64_t p_record_id_number) const override;
#endif // TOOLS_ENABLED
};


#endif // DATA_TABLE_CUSTOM_HPP
