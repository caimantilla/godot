#ifndef DATA_TABLE_CUSTOM_HPP
#define DATA_TABLE_CUSTOM_HPP


#include "data_table.hpp"
#include "core/object/gdvirtual.gen.inc"


class DataTableCustom final : public DataTable
{
	GDCLASS(DataTableCustom, DataTable);

private:
	PackedStringArray cache_id_list;

protected:
	static void _bind_methods();

	GDVIRTUAL0(_clear_table);
	GDVIRTUAL0(_load_table);
	GDVIRTUAL0RC(PackedStringArray, _get_record_id_list);

	GDVIRTUAL0RC(String, _editor_get_default_category);
	GDVIRTUAL1RC(String, _editor_get_record_name, String);
	GDVIRTUAL1RC(String, _editor_get_record_category, String);
	GDVIRTUAL1RC(String, _editor_get_record_description, String);
	GDVIRTUAL1RC(Ref<Texture2D>, _editor_get_record_icon, String);

public:
	virtual void clear() override;
	virtual void reload() override;
	virtual const PackedStringArray &get_id_list() const override;
	virtual const uint64_t get_record_count() const override;
	virtual const String &get_record_id_at(const uint64_t p_idx) const override;

	virtual String editor_get_default_category() const override;
	virtual String editor_get_record_name(const String &p_id) const override;
	virtual String editor_get_record_category(const String &p_id) const override;
	virtual String editor_get_record_description(const String &p_id) const override;
	virtual Ref<Texture2D> editor_get_record_icon(const String &p_id) const override;
};


#endif // DATA_TABLE_CUSTOM_HPP
