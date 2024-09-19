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
	String selected_id = get_edited_property_value();
	if (selected_id.is_empty())
	{
		return;
	}
	for (ItemList *item_list : item_lists)
	{
		for (int i = 0; i < item_list->get_item_count(); i++)
		{
			String curr_rec_id = item_list->get_item_metadata(i);
			if (curr_rec_id == selected_id)
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

	const PackedStringArray &record_id_list = table->get_id_list();

	for (const String &record_id : record_id_list)
	{
		String record_category = table->editor_get_record_category(record_id);
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

		String record_name = table->editor_get_record_name(record_id);
		if (record_name.is_empty())
		{
			record_name = record_id;
		}

		Ref<Texture2D> record_icon = table->editor_get_record_icon(record_id);
		String record_tooltip = table->editor_get_record_description(record_id);
		if (record_tooltip.is_empty())
		{
			record_tooltip = record_id;
		}

		Ref<Font> font = item_list->get_theme_font(SNAME("font"));

		if (font.is_valid())
		{
			int font_size = font->get_height(item_list->get_theme_font_size(SNAME("font_size")));
			Size2i ico_size = Size2i(font_size, font_size);
			item_list->set_fixed_icon_size(ico_size);
		}

		int record_index = item_list->add_item(record_name, record_icon);
		item_list->set_item_metadata(record_index, record_id);
		item_list->set_item_tooltip(record_index, record_tooltip);
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

	String default_category = table->editor_get_default_category();
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
	emit_changed(get_edited_property(), String());
}


void DataTableEditorPlugin::RecordSelectorProperty::update_button_state()
{
	String curr_selected_id;
	bool sel_ok = true;
	if (is_read_only())
	{
		sel_ok = false;
	}
	if (get_edited_object() == nullptr)
	{
		sel_ok = false;
	}
	else
	{
		curr_selected_id = get_edited_property_value();
	}

	btn_sel->set_disabled(!sel_ok);
	btn_clr->set_disabled(!sel_ok);

	if (curr_selected_id.is_empty())
	{
		btn_sel->set_text("--");
		btn_sel->set_tooltip_text(TTR("No record selected."));
		btn_clr->set_disabled(true);
	}
	else
	{
		String display_name = table->editor_get_record_name(curr_selected_id);
		if (display_name.is_empty())
		{
			display_name = curr_selected_id;
		}
		btn_sel->set_text(display_name);
		btn_sel->set_tooltip_text(curr_selected_id);
	}

	btn_sel->set_icon(table->editor_get_record_icon(curr_selected_id));
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
	String selected_id = p_item_list->get_item_metadata(p_item);
	emit_changed(get_edited_property(), selected_id);
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


DataTableEditorPlugin::RecordSelectorProperty::RecordSelectorProperty(DataTable *p_table, HashSet<String> p_category_constraint)
{
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
	if (p_hint == PROPERTY_HINT_NONE && (p_type == Variant::STRING || p_type == Variant::STRING_NAME) && p_hint_text.begins_with("DATA_RECORD:"))
	{
		DataTable *table = nullptr;

		String table_name = p_hint_text.get_slicec(':', 1).get_slicec('/', 0);
		if (!table_name.is_empty())
		{
			table = DataTableManager::get_singleton()->get_table_by_class_name(table_name);
		}
		if (table != nullptr)
		{
			HashSet<String> set_category_constraint;

			String category_constraint_str = p_hint_text.get_slicec('/', 1);
			if (!category_constraint_str.is_empty())
			{
				PackedStringArray parts = category_constraint_str.split(",", false);
				for (const String &part : parts)
				{
					set_category_constraint.insert(part);
				}
			}

			RecordSelectorProperty *prop = memnew(RecordSelectorProperty(table, set_category_constraint));
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
