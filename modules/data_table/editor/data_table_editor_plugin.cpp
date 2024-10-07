#include "data_table_editor_plugin.hpp"
#include "../data_table_manager.hpp"
#include "scene/scene_string_names.h"
#include "editor/editor_string_names.h"


void DataTableEditorPlugin::RecordSelectorProperty::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_READY: {
			refresh_everything();
		} break;
		case NOTIFICATION_THEME_CHANGED: {
			Ref<Texture2D> ico_clr = get_theme_icon(SNAME("Clear"), EditorStringName(EditorIcons));
			btn_clr->set_icon(ico_clr);

			Ref<Font> font = btn_sel->get_theme_font(SNAME("font"));
			if (font.is_valid())
			{
				int font_size = btn_sel->get_theme_font_size(SNAME("font_size"));
				font_size = font->get_height(font_size);
				btn_sel->add_theme_constant_override(SNAME("icon_max_width"), font_size);
			}
			else
			{
				btn_sel->remove_theme_constant_override(SNAME("icon_max_width"));
			}
		} break;
	}
}


void DataTableEditorPlugin::RecordSelectorProperty::_set_read_only(bool p_read_only)
{
	update_button_state();
}


void DataTableEditorPlugin::RecordSelectorProperty::update_property()
{
	update_button_state();
}


Variant DataTableEditorPlugin::RecordSelectorProperty::cast_value_to_property_type(const Variant &p_value) const
{
	switch (property_type)
	{
		case Variant::INT:
		case Variant::FLOAT:
			return (int64_t)p_value;
		case Variant::STRING:
		case Variant::STRING_NAME: 
			return (String)p_value;
		default: {
			CRASH_NOW();
			return Variant();
		}
	}
}


Variant DataTableEditorPlugin::RecordSelectorProperty::get_edited_property_value_casted() const
{
	return cast_value_to_property_type(get_edited_property_value());
}


Variant DataTableEditorPlugin::RecordSelectorProperty::get_invalid_id_value() const
{
	switch (id_mode)
	{
		case ID_MODE_NUMBER: return table->get_invalid_record_id_number();
		case ID_MODE_STRING: return table->get_invalid_record_id_string();
		default: {
			CRASH_NOW();
			return Variant();
		}
	}
}


void DataTableEditorPlugin::RecordSelectorProperty::refresh_everything()
{
	refresh_queue_dirty = false;
	refresh_records();
	focus_selected_record();
	update_button_state();
}


void DataTableEditorPlugin::RecordSelectorProperty::reset_popup_height()
{
	Size2i new_size = popup->get_size();
	new_size.height = 0;
	popup->set_size(new_size);
}


void DataTableEditorPlugin::RecordSelectorProperty::focus_selected_record()
{
	if (get_edited_object() == nullptr)
	{
		return;
	}
	const Variant selected_record_id = get_edited_property_value_casted();
	for (ItemList *item_list : item_lists)
	{
		for (int i = 0; i < item_list->get_item_count(); i++)
		{
			const Variant curr_rec_id = item_list->get_item_metadata(i);
			if (curr_rec_id == selected_record_id)
			{
				tabs->set_current_tab(tabs->get_tab_idx_from_control(item_list));
				item_list->select(i);
				return;
			}
		}
	}
}


void DataTableEditorPlugin::RecordSelectorProperty::refresh_records()
{
	for (ItemList *item_list : item_lists)
	{
		item_list->get_parent()->remove_child(item_list);
		item_list->queue_free();
	}
	item_lists.clear();

	ItemList *uncategorized_item_list = nullptr;
	HashMap<String, ItemList *> map_categorized_item_list;

	const PackedInt64Array record_id_no_list = table->get_record_id_number_list();
	const PackedStringArray record_id_str_list = table->get_record_id_string_list();

	int64_t record_count;
	switch (id_mode)
	{
		case ID_MODE_NUMBER: {
			record_count = record_id_no_list.size();
		} break;
		case ID_MODE_STRING: {
			record_count = record_id_str_list.size();
		} break;
		default: CRASH_NOW();
	}

	const int64_t invalid_id_no = table->get_invalid_record_id_number();
	const String invalid_id_str = table->get_invalid_record_id_string();

	for (int64_t i = 0; i < record_count; i++)
	{
		int64_t curr_record_id_no;
		String curr_record_id_str;

		switch (id_mode)
		{
			case ID_MODE_NUMBER: {
				curr_record_id_no = record_id_no_list[i];
				curr_record_id_str = itos(curr_record_id_no);
			} break;
			case ID_MODE_STRING: {
				curr_record_id_str = record_id_str_list[i];
				curr_record_id_no = table->convert_id_string_to_id_number(curr_record_id_str);
			} break;
			default: CRASH_NOW();
		}
		ERR_CONTINUE(curr_record_id_no == invalid_id_no || curr_record_id_str == invalid_id_str);

		String record_category = table->get_record_editor_category(curr_record_id_no);
		if (!category_constraint.is_empty() && (record_category.is_empty() || !category_constraint.has(record_category)))
		{
			continue;
		}

		ItemList *item_list;

		if (record_category.is_empty())
		{
			if (uncategorized_item_list == nullptr)
			{
				uncategorized_item_list = create_item_list();
			}
			item_list = uncategorized_item_list;
		}
		else
		{
			if (!map_categorized_item_list.has(record_category))
			{
				map_categorized_item_list.insert(record_category, create_item_list());
			}
			item_list = map_categorized_item_list[record_category];
		}

		String record_name = table->get_record_editor_name(curr_record_id_no);
		if (record_name.is_empty())
		{
			record_name = curr_record_id_str;
		}

		Ref<Texture2D> record_icon = table->get_record_editor_icon(curr_record_id_no);
		String record_tooltip = table->get_record_editor_description(curr_record_id_no);
		if (record_tooltip.is_empty())
		{
			record_tooltip = curr_record_id_str;
		}
		else
		{
			record_tooltip = vformat("%s\n%s", curr_record_id_str, record_tooltip);
		}

		Ref<Font> font = item_list->get_theme_font(SNAME("font"));

		if (font.is_valid())
		{
			int font_size = font->get_height(item_list->get_theme_font_size(SNAME("font_size")));
			Size2i ico_size = Size2i(font_size, font_size);
			item_list->set_fixed_icon_size(ico_size);
		}

		int item_index = item_list->add_item(record_name, record_icon);
		item_list->set_item_tooltip(item_index, record_tooltip);

		switch (id_mode)
		{
			case ID_MODE_NUMBER: {
				item_list->set_item_metadata(item_index, curr_record_id_no);
			} break;
			case ID_MODE_STRING: {
				item_list->set_item_metadata(item_index, curr_record_id_str);
			} break;
			default: CRASH_NOW();
		}
	}

	PackedStringArray sortable_category_names;
	sortable_category_names.resize(map_categorized_item_list.size());
	{
		int i = 0;
		for (HashMap<String, ItemList *>::Iterator E = map_categorized_item_list.begin(); E; ++E)
		{
			sortable_category_names.write[i] = E->key;
			i++;
		}
	}
	sortable_category_names.sort();

	if (uncategorized_item_list != nullptr)
	{
		tabs->add_child(uncategorized_item_list);
		tabs->set_tab_title(tabs->get_tab_idx_from_control(uncategorized_item_list), TTR("Uncategorized"));
	}
	for (const String &cat_name : sortable_category_names)
	{
		ItemList *item_list = map_categorized_item_list[cat_name];
		tabs->add_child(item_list);
		tabs->set_tab_title(tabs->get_tab_idx_from_control(item_list), cat_name);
	}

	if (item_lists.is_empty())
	{
		tabs->hide();
		empty_table_error->show();
	}
	else
	{
		empty_table_error->hide();
		tabs->show();
		if (item_lists.size() == 1)
		{
			tabs->set_tabs_visible(false);
		}
		else
		{
			tabs->set_tabs_visible(true);
		}
	}

	String default_category = table->get_default_editor_category();
	if (default_category.is_empty() || !map_categorized_item_list.has(default_category))
	{
		if (uncategorized_item_list != nullptr)
		{
			tabs->set_current_tab(tabs->get_tab_idx_from_control(uncategorized_item_list));
		}
	}
	else
	{
		tabs->set_current_tab(tabs->get_tab_idx_from_control(map_categorized_item_list[default_category]));
	}
}


void DataTableEditorPlugin::RecordSelectorProperty::prompt_record_selection()
{
	if (get_edited_object() == nullptr)
	{
		return;
	}

	popup->set_current_screen(btn_sel->get_window()->get_current_screen());
	popup->reset_size();

	Point2 target_size = popup->get_size();
	target_size.x = MAX(target_size.x, hbox->get_size().x);
	popup->set_size(target_size.ceil());

	Point2 target_pos = hbox->get_screen_position();
	target_pos += hbox->get_size();
	target_pos.x -= popup->get_size().x;
	popup->set_position(target_pos.ceil());

	popup->popup();
}


void DataTableEditorPlugin::RecordSelectorProperty::clear_record_selection()
{
	emit_changed(get_edited_property(), get_invalid_id_value());
}


void DataTableEditorPlugin::RecordSelectorProperty::update_button_state()
{
	bool disp_id_ok = false;
	bool sel_ok = true;

	int64_t curr_selected_id_no;
	String curr_selected_id_str;

	if (is_read_only())
	{
		sel_ok = false;
	}
	if (get_edited_object() == nullptr)
	{
		disp_id_ok = false;
		sel_ok = false;
	}
	else
	{
		disp_id_ok = true;
		switch (id_mode)
		{
			case ID_MODE_NUMBER: {
				curr_selected_id_no = get_edited_property_value_casted();
				curr_selected_id_str = itos(curr_selected_id_no);
			} break;
			case ID_MODE_STRING: {
				curr_selected_id_str = get_edited_property_value_casted();
				curr_selected_id_no = table->convert_id_string_to_id_number(curr_selected_id_str);
			} break;
			default: CRASH_NOW();
		}
	}

	btn_sel->set_disabled(!sel_ok);
	btn_clr->set_disabled(!sel_ok);

	if (!disp_id_ok)
	{
		btn_sel->set_text("--");
		btn_sel->set_tooltip_text(TTR("Not yet ready to display."));
		btn_sel->set_icon(Ref<Texture2D>());
		btn_clr->set_disabled(true);
	}
	else
	{
		bool disp_id_invalid = false;

		switch (id_mode)
		{
			case ID_MODE_NUMBER: {
				disp_id_invalid = curr_selected_id_no == table->get_invalid_record_id_number();
			} break;
			case ID_MODE_STRING: {
				disp_id_invalid = curr_selected_id_str == table->get_invalid_record_id_string();
			} break;
			default: CRASH_NOW();
		}
		if (disp_id_invalid)
		{
			btn_sel->set_text("--");
			btn_sel->set_tooltip_text(TTR("No record selected."));
			btn_sel->set_icon(Ref<Texture2D>());
			btn_clr->set_disabled(true);
		}
		else
		{
			String display_name = table->get_record_editor_name(curr_selected_id_no);
			if (display_name.is_empty())
			{
				display_name = curr_selected_id_str;
			}
			btn_sel->set_text(display_name);
			btn_sel->set_tooltip_text(curr_selected_id_str);
			btn_sel->set_icon(table->get_record_editor_icon(curr_selected_id_no));
		}
	}
}


void DataTableEditorPlugin::RecordSelectorProperty::on_item_list_visibility_changed(ItemList *p_item_list)
{
	if (p_item_list->is_visible_in_tree())
	{
		p_item_list->grab_focus();
		reset_popup_height();
	}
}


void DataTableEditorPlugin::RecordSelectorProperty::on_item_list_item_selected(int p_item, ItemList *p_item_list)
{
	popup->hide();
	Variant selected_record_id = cast_value_to_property_type(p_item_list->get_item_metadata(p_item));
	emit_changed(get_edited_property(), selected_record_id);
}


void DataTableEditorPlugin::RecordSelectorProperty::on_tabs_visibility_changed()
{
	if (tabs->is_visible_in_tree())
	{
		focus_selected_record();
	}
	else
	{
		for (ItemList *item_list : item_lists)
		{
			item_list->deselect_all();
		}
	}
}


void DataTableEditorPlugin::RecordSelectorProperty::on_table_changed()
{
	if (!refresh_queue_dirty)
	{
		refresh_queue_dirty = true;
		callable_mp(this, &RecordSelectorProperty::refresh_everything).call_deferred();
	}
}


ItemList *DataTableEditorPlugin::RecordSelectorProperty::create_item_list()
{
	ItemList *ret = memnew(ItemList);

	ret->set_auto_translate_mode(Node::AUTO_TRANSLATE_MODE_DISABLED);
	ret->set_max_columns(0);
	ret->set_same_column_width(true);
	ret->set_auto_height(true);
	ret->set_allow_reselect(true);
	ret->set_allow_search(true);
	ret->set_icon_mode(ItemList::ICON_MODE_LEFT);

	ret->connect(SNAME("item_selected"), callable_mp(this, &RecordSelectorProperty::on_item_list_item_selected).bind(ret), CONNECT_DEFERRED);
	ret->connect(SceneStringName(visibility_changed), callable_mp(this, &RecordSelectorProperty::on_item_list_visibility_changed).bind(ret), CONNECT_DEFERRED);

	item_lists.push_back(ret);
	return ret;
}


DataTableEditorPlugin::RecordSelectorProperty::RecordSelectorProperty(Variant::Type p_property_type, DataTable *p_table, HashSet<String> p_category_constraint)
{
	property_type = p_property_type;
	switch (p_property_type)
	{
		case Variant::STRING:
		case Variant::STRING_NAME: {
			id_mode = ID_MODE_STRING;
		} break;
		case Variant::INT:
		case Variant::FLOAT: {
			id_mode = ID_MODE_NUMBER;
		} break;
		default: {
			CRASH_NOW_MSG("Unsupported property type: " + itos(p_property_type));
		} return;
	}

	table = p_table;
	table->connect(CoreStringName(changed), callable_mp(this, &RecordSelectorProperty::on_table_changed));

	category_constraint = p_category_constraint;

	hbox = memnew(HBoxContainer);
	hbox->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	add_child(hbox);

	btn_sel = memnew(Button);
	btn_sel->set_auto_translate_mode(Node::AUTO_TRANSLATE_MODE_DISABLED);
	btn_sel->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	btn_sel->connect(SceneStringName(pressed), callable_mp(this, &RecordSelectorProperty::prompt_record_selection));
	hbox->add_child(btn_sel);
	add_focusable(btn_sel);

	btn_clr = memnew(Button);
	btn_clr->set_tooltip_text(TTR("Nullify the current selection."));
	btn_clr->connect(SceneStringName(pressed), callable_mp(this, &RecordSelectorProperty::clear_record_selection));
	hbox->add_child(btn_clr);
	add_focusable(btn_clr);

	popup = memnew(PopupPanel);
	popup->set_wrap_controls(true);
	add_child(popup);

	empty_table_error = memnew(Label);
	empty_table_error->set_text(TTR("Table is empty."));
	popup->add_child(empty_table_error);

	tabs = memnew(TabContainer);
	tabs->set_auto_translate_mode(Node::AUTO_TRANSLATE_MODE_DISABLED);
	tabs->set_clip_tabs(false);
	tabs->connect(SceneStringName(visibility_changed), callable_mp(this, &RecordSelectorProperty::on_tabs_visibility_changed), CONNECT_DEFERRED);
	popup->add_child(tabs);
}


bool DataTableEditorPlugin::RecordSelectorPlugin::can_handle(Object *p_object)
{
	return true;
}


bool DataTableEditorPlugin::RecordSelectorPlugin::parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide)
{
	if (p_hint == PROPERTY_HINT_NONE && (p_type == Variant::STRING || p_type == Variant::STRING_NAME || p_type == Variant::INT || p_type == Variant::FLOAT) && p_hint_text.begins_with("DATA_RECORD:"))
	{
		DataTable *table = nullptr;

		String table_name = p_hint_text.get_slicec(':', 1);
		if (!table_name.is_empty())
		{
			table = DataTableManager::get_singleton()->get_table_by_class_name(table_name);
		}
		if (table != nullptr)
		{
			HashSet<String> set_category_constraint;

			String category_constraint_str = p_hint_text.get_slicec(':', 2);
			if (!category_constraint_str.is_empty())
			{
				PackedStringArray parts = category_constraint_str.split(",", false);
				for (const String &part : parts)
				{
					set_category_constraint.insert(part);
				}
			}

			RecordSelectorProperty *prop = memnew(RecordSelectorProperty(p_type, table, set_category_constraint));
			add_property_editor(p_path, prop);
			return true;
		}
	}
	return false;
}


void DataTableEditorPlugin::reload_tables()
{
	DataTableManager::get_singleton()->reload_every_table();
	print_line(TTR("Reloaded editor DataTables."));
}


DataTableEditorPlugin::DataTableEditorPlugin()
{
	add_tool_menu_item(TTR("Reload DataTables"), callable_mp(this, &DataTableEditorPlugin::reload_tables));

	record_selector_plugin.instantiate();
	add_inspector_plugin(record_selector_plugin);
}


DataTableEditorPlugin::~DataTableEditorPlugin()
{
	remove_inspector_plugin(record_selector_plugin);

	remove_tool_menu_item(TTR("Reload DataTables"));
}
