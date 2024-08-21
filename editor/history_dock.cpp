/**************************************************************************/
/*  history_dock.cpp                                                      */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "history_dock.h"

#include "editor/editor_node.h"
#include "editor/editor_settings.h"
#include "editor/editor_string_names.h"
#include "editor/editor_undo_redo_manager.h"
#include "scene/gui/item_list.h"

struct SortActionsByTimestamp {
	bool operator()(const EditorUndoRedoManager::Action &l, const EditorUndoRedoManager::Action &r) const {
		return l.timestamp > r.timestamp;
	}
};

void HistoryDock::on_history_changed() {
	if (is_visible_in_tree()) {
		refresh_history();
	} else {
		need_refresh = true;
	}
}

void HistoryDock::refresh_history() {
	action_list->clear();

	if (!is_any_history_visible()) {
		action_list->add_item(TTR("The Beginning"));
		return;
	}

	List<const EditorUndoRedoManager::History *> active_histories;
	ur_manager->get_active_history_list(&active_histories);

	Vector<EditorUndoRedoManager::Action> full_history;

	int full_size = 0;
	for (const EditorUndoRedoManager::History *history : active_histories) {
		if (is_history_context_visible(history->context)) {
			full_size += history->redo_stack.size() + history->undo_stack.size();
		}
	}
	full_history.resize(full_size);

	int i = 0;
	for (const EditorUndoRedoManager::History *history : active_histories) {
		if (is_history_context_visible(history->context)) {
			for (const EditorUndoRedoManager::Action &action : history->redo_stack) {
				full_history.write[i] = action;
				i++;
			}
			for (const EditorUndoRedoManager::Action &action : history->undo_stack) {
				full_history.write[i] = action;
				i++;
			}
		}
	}

	full_history.sort_custom<SortActionsByTimestamp>();
	for (const EditorUndoRedoManager::Action &E : full_history) {
		action_list->add_item(E.action_name);
		if (E.history_id == EditorUndoRedoManager::GLOBAL_HISTORY) {
			action_list->set_item_custom_fg_color(-1, get_theme_color(SNAME("accent_color"), EditorStringName(Editor)));
		}
	}

	action_list->add_item(TTR("The Beginning"));
	refresh_version();
}

void HistoryDock::on_version_changed() {
	if (is_visible_in_tree()) {
		refresh_version();
	} else {
		need_refresh = true;
	}
}

void HistoryDock::refresh_version() {
	int idx = 0;

	if (!is_any_history_visible()) {
		current_version = idx;
		action_list->set_current(idx);
		return;
	}

	List<const EditorUndoRedoManager::History *> active_histories;
	ur_manager->get_active_history_list(&active_histories);

	double newest_undo_timestamp = 0;

	for (const EditorUndoRedoManager::History *history : active_histories) {
		if (history->undo_stack.is_empty() || !is_history_context_visible(history->context)) {
			continue;
		}
		newest_undo_timestamp = MAX(newest_undo_timestamp, history->undo_stack.front()->get().timestamp);
	}

	for (const EditorUndoRedoManager::History *history : active_histories) {
		if (!is_history_context_visible(history->context)) {
			continue;
		}
		int skip = 0;
		for (const EditorUndoRedoManager::Action &action : history->redo_stack) {
			if (action.timestamp < newest_undo_timestamp) {
				skip++;
			} else {
				break;
			}
		}
		idx += history->redo_stack.size() - skip;
	}

	current_version = idx;
	action_list->set_current(idx);
}

void HistoryDock::seek_history(int p_index) {
	if (!is_any_history_visible()) {
		return;
	}

	int context_filter = get_history_context_filter();

	while (current_version < p_index) {
		if (!ur_manager->undo(context_filter)) {
			ERR_PRINT(TTR("Failed to execute all undo operations. Clearing history."));
			ur_manager->clear_history();
			break;
		}
	}

	while (current_version > p_index) {
		if (!ur_manager->redo(context_filter)) {
			ERR_PRINT(TTR("Failed to execute all redo operations. Clearing history."));
			ur_manager->clear_history();
			break;
		}
	}

	action_list->select(current_version);
}

void HistoryDock::_notification(int p_notification) {
	switch (p_notification) {
		case NOTIFICATION_READY: {
			EditorNode::get_singleton()->connect("scene_changed", callable_mp(this, &HistoryDock::on_history_changed));
		} break;

		case NOTIFICATION_VISIBILITY_CHANGED: {
			if (is_visible_in_tree() && need_refresh) {
				refresh_history();
			}
		} break;
	}
}

void HistoryDock::save_options() {
	for (const HistoryContextToggle *toggle : history_context_filter_toggles) {
		EditorSettings::get_singleton()->set_project_metadata("history", vformat("include_%s", toggle->id), toggle->is_pressed());
	}
}

HistoryDock::HistoryContextToggle *HistoryDock::create_history_context_toggle(const String &p_id, const int p_filter_bit, const String &p_text, const String &p_tooltip_text) {
	bool init_pressed = EditorSettings::get_singleton()->get_project_metadata("history", vformat("include_%s", p_id), true);

	HistoryContextToggle *toggle = memnew(HistoryContextToggle);
	toggle->id = p_id;
	toggle->filter_bit = p_filter_bit;
	toggle->set_pressed(init_pressed);
	toggle->set_text(p_text);
	toggle->set_tooltip_text(p_tooltip_text);
	toggle->set_h_size_flags(SIZE_EXPAND_FILL);
	toggle->connect("toggled", callable_mp(this, &HistoryDock::refresh_history).unbind(1));
	toggle->connect("toggled", callable_mp(this, &HistoryDock::save_options).unbind(1));

	history_context_filter_toggles.push_back(toggle);
	return toggle;
}

const bool HistoryDock::is_history_context_visible(const int p_context) const {
	for (const HistoryContextToggle *toggle : history_context_filter_toggles) {
		if (toggle->filter_bit == p_context && toggle->is_pressed()) {
			return true;
		}
	}
	return false;
}

const bool HistoryDock::is_any_history_visible() const {
	for (const HistoryContextToggle *toggle : history_context_filter_toggles) {
		if (toggle->is_pressed()) {
			return true;
		}
	}
	return false;
}

const int HistoryDock::get_history_context_filter() const {
	int bits = 0;
	for (const HistoryContextToggle *toggle : history_context_filter_toggles) {
		if (toggle->is_pressed()) {
			bits |= toggle->filter_bit;
		}
	}
	return bits;
}

HistoryDock::HistoryDock() {
	set_name("History");

	ur_manager = EditorUndoRedoManager::get_singleton();
	ur_manager->connect("history_changed", callable_mp(this, &HistoryDock::on_history_changed));
	ur_manager->connect("version_changed", callable_mp(this, &HistoryDock::on_version_changed));

	HBoxContainer *context_filter_hbox = memnew(HBoxContainer);
	add_child(context_filter_hbox);

	context_filter_hbox->add_child(create_history_context_toggle("global", EditorUndoRedoManager::CONTEXT_GLOBAL, TTR("Global"), TTR("Toggle visibility of the global history.")));
	context_filter_hbox->add_child(create_history_context_toggle("scene", EditorUndoRedoManager::CONTEXT_SCENE, TTR("Scene"), TTR("Toggle visibility of the scene edit history.")));
	context_filter_hbox->add_child(create_history_context_toggle("custom", EditorUndoRedoManager::CONTEXT_CUSTOM, TTR("Custom"), TTR("Toggles visibility of the history of custom editors.")));

	action_list = memnew(ItemList);
	action_list->set_auto_translate_mode(AUTO_TRANSLATE_MODE_DISABLED);
	add_child(action_list);
	action_list->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	action_list->connect("item_selected", callable_mp(this, &HistoryDock::seek_history));
}
