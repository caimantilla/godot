#include "dynamic_speech_balloon.h"
#include "scene/gui/rich_text_label.h"
#include "scene/theme/theme_db.h"


bool DynamicSpeechBalloon::has_tail(int p_index) const
{
	return p_index > -1 && p_index < tail_list.size();
}


void DynamicSpeechBalloon::set_tail_anchor_mode(const TailAnchorMode p_mode)
{
	if (p_mode == tail_anchor_mode)
	{
		return;
	}
	tail_anchor_mode = p_mode;
	queue_redraw();
}


DynamicSpeechBalloon::TailAnchorMode DynamicSpeechBalloon::get_tail_anchor_mode() const
{
	return tail_anchor_mode;
}


void DynamicSpeechBalloon::set_tail_count(int p_count)
{
	ERR_FAIL_COND(p_count < 0);
	if (p_count == tail_list.size())
	{
		return;
	}
	tail_list.resize(p_count);
	notify_property_list_changed();
	queue_redraw();
}


int DynamicSpeechBalloon::get_tail_count() const
{
	return tail_list.size();
}


void DynamicSpeechBalloon::set_tail_position(int p_index, const Point2 &p_point)
{
	ERR_FAIL_COND_MSG(!has_tail(p_index), vformat(RTR("Tail No. %d does not exist."), p_index));
	get_tail(p_index).position = p_point;
	queue_redraw();
}


Point2 DynamicSpeechBalloon::get_tail_position(int p_index) const
{
	ERR_FAIL_COND_V_MSG(!has_tail(p_index), Point2(), vformat(RTR("Tail No. %d does not exist."), p_index));
	return get_tail(p_index).position;
}


void DynamicSpeechBalloon::set_tail_extension_ratio(int p_index, float p_ratio)
{
	ERR_FAIL_COND_MSG(!has_tail(p_index), vformat(RTR("Tail No. %d does not exist."), p_index));
	ERR_FAIL_COND_MSG(p_ratio < 0.0f || p_ratio > 1.0f, RTR("Tail extension ratio must be between 0-1."));
	get_tail(p_index).extension_ratio = p_ratio;
	queue_redraw();
}


float DynamicSpeechBalloon::get_tail_extension_ratio(int p_index) const
{
	ERR_FAIL_COND_V_MSG(!has_tail(p_index), 0.0f, vformat(RTR("Tail No. %d does not exist."), p_index));
	return get_tail(p_index).extension_ratio;
}


void DynamicSpeechBalloon::set_name_text(const String &p_text)
{
	if (p_text == name_text)
	{
		return;
	}
	name_text = p_text;
	queue_redraw();
}


String DynamicSpeechBalloon::get_name_text() const
{
	return name_text;
}


void DynamicSpeechBalloon::set_name_alignment(const HorizontalAlignment p_alignment)
{
	if (p_alignment == name_alignment)
	{
		return;
	}
	name_alignment = p_alignment;
	queue_redraw();
}


HorizontalAlignment DynamicSpeechBalloon::get_name_alignment() const
{
	return name_alignment;
}


void DynamicSpeechBalloon::set_dialogue_text(const String &p_text)
{
	dialogue_label->set_text(p_text);
}


String DynamicSpeechBalloon::get_dialogue_text() const
{
	return dialogue_label->get_text();
}


RichTextLabel *DynamicSpeechBalloon::get_dialogue_label() const
{
	return dialogue_label;
}


DynamicSpeechBalloon::Tail &DynamicSpeechBalloon::get_tail(const int p_index)
{
	return tail_list[p_index];
}


const DynamicSpeechBalloon::Tail &DynamicSpeechBalloon::get_tail(const int p_index) const
{
	return tail_list[p_index];
}


Vector<int> DynamicSpeechBalloon::get_allowed_size_flags_horizontal() const
{
	Vector<int> ret;
	ret.push_back(Control::SIZE_EXPAND_FILL);
	return ret;
}


Vector<int> DynamicSpeechBalloon::get_allowed_size_flags_vertical() const
{
	Vector<int> ret;
	ret.push_back(Control::SIZE_EXPAND_FILL);
	return ret;
}


Size2 DynamicSpeechBalloon::get_minimum_size() const
{
	Size2 ret;
	for (int i = 0; i < get_child_count(true); i++)
	{
		Control *child = Object::cast_to<Control>(get_child(i, true));
		if (child != nullptr)
		{
			ret = ret.max(child->get_combined_minimum_size());
		}
	}
	ret.x += (theme_cache.inner_margin_left + theme_cache.inner_margin_right);
	ret.y += (theme_cache.inner_margin_top + theme_cache.inner_margin_bottom);
	return ret;
}


void DynamicSpeechBalloon::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_THEME_CHANGED: {
			dialogue_label->add_theme_color_override("default_color", theme_cache.dialogue_font_default_color);

			dialogue_label->add_theme_color_override("font_outline_color", theme_cache.dialogue_font_outline_color);
			dialogue_label->add_theme_constant_override("outline_size", theme_cache.dialogue_font_outline_size);

			dialogue_label->add_theme_font_override("normal_font", theme_cache.dialogue_font_normal);
			dialogue_label->add_theme_font_override("italics_font", theme_cache.dialogue_font_italics);
			dialogue_label->add_theme_font_override("bold_font", theme_cache.dialogue_font_bold);
			dialogue_label->add_theme_font_override("bold_italics_font", theme_cache.dialogue_font_bold_italics);
			dialogue_label->add_theme_font_override("mono_font", theme_cache.dialogue_font_mono);

			dialogue_label->add_theme_font_size_override("normal_font_size", theme_cache.dialogue_font_size);
			dialogue_label->add_theme_font_size_override("italics_font_size", theme_cache.dialogue_font_size);
			dialogue_label->add_theme_font_size_override("bold_font_size", theme_cache.dialogue_font_size);
			dialogue_label->add_theme_font_size_override("bold_italics_font_size", theme_cache.dialogue_font_size);
			dialogue_label->add_theme_font_size_override("mono_font_size", theme_cache.dialogue_font_size);

			dialogue_label->add_theme_constant_override("line_separation", theme_cache.dialogue_line_separation);

			update_minimum_size();
			queue_sort();
			queue_redraw();
		} break;
		case NOTIFICATION_SORT_CHILDREN: {
			const Size2 size = get_size();
			Rect2 inner_rect;
			inner_rect.set_position(Point2(theme_cache.inner_margin_left, theme_cache.inner_margin_top));
			inner_rect.set_end(size - Size2(theme_cache.inner_margin_right, theme_cache.inner_margin_bottom));
			for (int i = 0; i < get_child_count(true); i++)
			{
				Control *child = Object::cast_to<Control>(get_child(i, true));
				if (child != nullptr)
				{
					fit_child_in_rect(child, inner_rect);
				}
			}
		} break;
		case NOTIFICATION_DRAW: {
			Rect2 own_rect = get_rect();
			Point2 middle_point = own_rect.size * 0.5;

			Vector<Point2> balloon_poly;
			balloon_poly.resize(4);

			balloon_poly.write[0] = Point2(0, 0);
			balloon_poly.write[1] = Point2(own_rect.size.x, 0);
			balloon_poly.write[2] = own_rect.size;
			balloon_poly.write[3] = Point2(0, own_rect.size.y);

			Vector<Point2> tail_poly;
			tail_poly.resize(4);

			for (int i = 0; i < get_tail_count(); i++)
			{
				const Tail &tail = get_tail(i);
				Point2 tail_point;
				switch (tail_anchor_mode)
				{
					case TAIL_ANCHOR_CENTER: {
						tail_point = tail.position + middle_point;
					} break;
					case TAIL_ANCHOR_TOP_LEFT:
					default: {
						tail_point = tail.position;
					} break;
				}
				float tail_extension = CLAMP(tail.extension_ratio, 0.0f, 1.0f);
				float angle = middle_point.angle_to_point(tail_point);

				tail_poly.write[0] = (Point2(theme_cache.tail_core_size, 0).rotated(angle) + middle_point).lerp(tail_point, tail_extension);
				tail_poly.write[1] = Point2(0, theme_cache.tail_core_size).rotated(angle) + middle_point;
				tail_poly.write[2] = Point2(-theme_cache.tail_core_size, 0).rotated(angle) + middle_point;
				tail_poly.write[3] = Point2(0, -theme_cache.tail_core_size).rotated(angle) + middle_point;

				// NOTE: No clue why this method returns a vector. Only the first element is needed.
				balloon_poly = Geometry2D::merge_polygons(balloon_poly, tail_poly)[0];
			}
			draw_colored_polygon(balloon_poly, theme_cache.balloon_color);
			if (theme_cache.balloon_outline_size != 0)
			{
				int outline_size = MAX(theme_cache.balloon_outline_size, -1);
				Vector<Point2> outline_poly = balloon_poly;
				outline_poly.push_back(outline_poly[0]);
				// suppress some warning about not supporting anti-aliasing for thin/pixel-width lines
				bool anti_alias = outline_size > 0;
				draw_polyline(outline_poly, theme_cache.balloon_outline_color, outline_size, anti_alias);
			}

			// Draw the name
			if (!name_text.is_empty() && theme_cache.name_font.is_valid())
			{
				Size2 text_size = theme_cache.name_font->get_string_size(name_text, name_alignment, -1, theme_cache.name_font_size);
				Point2 name_position = Point2(0, 0);
				switch (name_alignment)
				{
					case HORIZONTAL_ALIGNMENT_RIGHT: {
						name_position.x = own_rect.size.width - text_size.width;
					} break;
					case HORIZONTAL_ALIGNMENT_CENTER: {
						name_position.x = (own_rect.size.width * 0.5) - (text_size.width * 0.5);
					} break;
					case HORIZONTAL_ALIGNMENT_LEFT:
					default: {
						name_position.x = 0;
					} break;
				}
				if (theme_cache.name_font_outline_size > 0 && !Math::is_zero_approx(theme_cache.name_font_outline_color.a))
				{
					draw_string_outline(theme_cache.name_font, name_position, name_text, name_alignment, -1, theme_cache.name_font_size, theme_cache.name_font_outline_size, theme_cache.name_font_outline_color);
				}
				draw_string(theme_cache.name_font, name_position, name_text, name_alignment, -1, theme_cache.name_font_size, theme_cache.name_font_color);
			}
		} break;
		default: break;
	}
}


void DynamicSpeechBalloon::_get_property_list(List<PropertyInfo> *p_list) const
{
	p_list_helper_tail.get_property_list(p_list);
}


bool DynamicSpeechBalloon::_set(const StringName &p_name, const Variant &p_value)
{
	return p_list_helper_tail.property_set_value(p_name, p_value);
}


bool DynamicSpeechBalloon::_get(const StringName &p_name, Variant &r_ret) const
{
	return p_list_helper_tail.property_get_value(p_name, r_ret);
}


bool DynamicSpeechBalloon::_property_can_revert(const StringName &p_name) const
{
	return p_list_helper_tail.property_can_revert(p_name);
}


bool DynamicSpeechBalloon::_property_get_revert(const StringName &p_name, Variant &r_property) const
{
	return p_list_helper_tail.property_get_revert(p_name, r_property);
}


void DynamicSpeechBalloon::_bind_methods()
{
	BIND_CONSTANT(INVALID_TAIL);

	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, DynamicSpeechBalloon, name_font);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT_SIZE, DynamicSpeechBalloon, name_font_size);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, DynamicSpeechBalloon, name_font_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, DynamicSpeechBalloon, name_font_outline_size);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, DynamicSpeechBalloon, name_font_outline_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, DynamicSpeechBalloon, dialogue_font_normal);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, DynamicSpeechBalloon, dialogue_font_italics);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, DynamicSpeechBalloon, dialogue_font_bold);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, DynamicSpeechBalloon, dialogue_font_bold_italics);
	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT, DynamicSpeechBalloon, dialogue_font_mono);

	BIND_THEME_ITEM(Theme::DATA_TYPE_FONT_SIZE, DynamicSpeechBalloon, dialogue_font_size);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, DynamicSpeechBalloon, dialogue_font_default_color);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, DynamicSpeechBalloon, dialogue_font_outline_size);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, DynamicSpeechBalloon, dialogue_font_outline_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, DynamicSpeechBalloon, dialogue_line_separation);

	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, DynamicSpeechBalloon, inner_margin_left);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, DynamicSpeechBalloon, inner_margin_top);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, DynamicSpeechBalloon, inner_margin_right);
	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, DynamicSpeechBalloon, inner_margin_bottom);

	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, DynamicSpeechBalloon, balloon_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, DynamicSpeechBalloon, balloon_outline_size);
	BIND_THEME_ITEM(Theme::DATA_TYPE_COLOR, DynamicSpeechBalloon, balloon_outline_color);

	BIND_THEME_ITEM(Theme::DATA_TYPE_CONSTANT, DynamicSpeechBalloon, tail_core_size);

	ClassDB::bind_method(D_METHOD("has_tail", "index"), &DynamicSpeechBalloon::has_tail);
	ClassDB::bind_method(D_METHOD("set_tail_count", "count"), &DynamicSpeechBalloon::set_tail_count);
	ClassDB::bind_method(D_METHOD("get_tail_count"), &DynamicSpeechBalloon::get_tail_count);
	ClassDB::bind_method(D_METHOD("set_tail_position", "index", "point"), &DynamicSpeechBalloon::set_tail_position);
	ClassDB::bind_method(D_METHOD("get_tail_position", "index"), &DynamicSpeechBalloon::get_tail_position);
	ClassDB::bind_method(D_METHOD("set_tail_extension_ratio", "index", "ratio"), &DynamicSpeechBalloon::set_tail_extension_ratio);
	ClassDB::bind_method(D_METHOD("get_tail_extension_ratio", "index"), &DynamicSpeechBalloon::get_tail_extension_ratio);

	ClassDB::bind_method(D_METHOD("set_name_text", "name"), &DynamicSpeechBalloon::set_name_text);
	ClassDB::bind_method(D_METHOD("get_name_text"), &DynamicSpeechBalloon::get_name_text);
	ClassDB::bind_method(D_METHOD("set_name_alignment", "alignment"), &DynamicSpeechBalloon::set_name_alignment);
	ClassDB::bind_method(D_METHOD("get_name_alignment"), &DynamicSpeechBalloon::get_name_alignment);

	ClassDB::bind_method(D_METHOD("set_dialogue_text", "dialogue"), &DynamicSpeechBalloon::set_dialogue_text);
	ClassDB::bind_method(D_METHOD("get_dialogue_text"), &DynamicSpeechBalloon::get_dialogue_text);
	ClassDB::bind_method(D_METHOD("get_dialogue_label"), &DynamicSpeechBalloon::get_dialogue_label);

	ADD_GROUP("Name", "name_");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "name_text"), "set_name_text", "get_name_text");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "name_alignment", PROPERTY_HINT_ENUM, "Left,Center,Right"), "set_name_alignment", "get_name_alignment");

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "dialogue_text", PROPERTY_HINT_MULTILINE_TEXT), "set_dialogue_text", "get_dialogue_text");

	ADD_ARRAY_COUNT("Tails", "tail_count", "set_tail_count", "get_tail_count", "tails/");
	Tail tail_defaults;
	base_p_list_helper_tail.set_prefix("tails/");
	base_p_list_helper_tail.set_array_length_getter(&DynamicSpeechBalloon::get_tail_count);
	base_p_list_helper_tail.register_property(PropertyInfo(Variant::VECTOR2, "position"), tail_defaults.position, &DynamicSpeechBalloon::set_tail_position, &DynamicSpeechBalloon::get_tail_position);
	base_p_list_helper_tail.register_property(PropertyInfo(Variant::FLOAT, "extension_ratio", PROPERTY_HINT_RANGE, "0.0,1.0"), tail_defaults.extension_ratio, &DynamicSpeechBalloon::set_tail_extension_ratio, &DynamicSpeechBalloon::get_tail_extension_ratio);
	PropertyListHelper::register_base_helper(&base_p_list_helper_tail);
}


DynamicSpeechBalloon::DynamicSpeechBalloon()
{
	p_list_helper_tail.setup_for_instance(base_p_list_helper_tail, this);

	set_clip_contents(false);

	dialogue_label = memnew(RichTextLabel);
	dialogue_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	dialogue_label->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	dialogue_label->set_fit_content(true);
	dialogue_label->set_use_bbcode(true);
	dialogue_label->set_autowrap_mode(TextServer::AUTOWRAP_OFF);
	dialogue_label->set_scroll_active(false);
	add_child(dialogue_label, false, INTERNAL_MODE_FRONT);
}
