#ifndef DATA_TABLE_EDITOR_PLUGIN_HPP
#define DATA_TABLE_EDITOR_PLUGIN_HPP


#include "../data_table.hpp"
#include "scene/gui/label.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/popup.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/item_list.h"
#include "editor/plugins/editor_plugin.h"
#include "editor/editor_inspector.h"


class EditorPropertyDataTableRecordSelector final : public EditorProperty
{
	GDCLASS(EditorPropertyDataTableRecordSelector, EditorProperty);

private:
	enum IDMode
	{
		ID_MODE_NUMBER = 0,
		ID_MODE_STRING = 1,
	};

private:
	double cfg_record_color_tint_bg = 0.3;
	double cfg_record_color_tint_text = 0.05;

	IDMode id_mode;
	Variant::Type property_type;
	DataTable *table;
	HashSet<String> category_constraint;

	bool refresh_queue_dirty = false;

	HBoxContainer *hbox;
	Button *btn_sel;
	Button *btn_clr;
	PopupPanel *popup;
	Label *empty_table_error;
	TabContainer *tabs;
	Vector<ItemList *> item_lists;

	Variant cast_value_to_property_type(const Variant &p_value) const;
	Variant get_edited_property_value_casted() const;
	Variant get_invalid_id_value() const;

	ItemList *create_item_list();

	void update_button_state();
	void refresh_records();
	void refresh_everything();
	void focus_selected_record();
	void prompt_record_selection();
	void clear_record_selection();
	void reset_popup_height();
	void queue_refresh_everything();
	void update_configuration();

	void on_editor_settings_changed();
	void on_table_changed();
	void on_tabs_visibility_changed();
	void on_item_list_visibility_changed(ItemList *p_item_list);
	void on_item_list_item_selected(int p_item, ItemList *p_item_list);

protected:
	static void _bind_methods();
	void _notification(int p_what);
	virtual void _set_read_only(bool p_read_only) override;

public:
	virtual void update_property() override;

	EditorPropertyDataTableRecordSelector(Variant::Type p_property_type, DataTable *p_table, HashSet<String> p_category_constraint);
};


class EditorInspectorPluginDataTableRecordSelector final : public EditorInspectorPlugin 
{
	GDCLASS(EditorInspectorPluginDataTableRecordSelector, EditorInspectorPlugin);

public:
	virtual bool can_handle(Object *p_object) override;
	virtual bool parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide = false) override;
};


class DataTableEditorPlugin final : public EditorPlugin
{
	GDCLASS(DataTableEditorPlugin, EditorPlugin);

private:
	String reload_tables_tool_menu_text;
	Ref<EditorInspectorPluginDataTableRecordSelector> record_selector_plugin;

	void reload_tables();

protected:
	void _notification(int p_what);

public:
	DataTableEditorPlugin();
	~DataTableEditorPlugin();
};


#endif // DATA_TABLE_EDITOR_PLUGIN_HPP
