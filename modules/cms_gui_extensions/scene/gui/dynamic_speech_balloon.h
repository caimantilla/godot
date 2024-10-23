#ifndef TALK_BALLOON_H
#define TALK_BALLOON_H


#include <array>
#include "core/math/geometry_2d.h"
#include "scene/property_list_helper.h"
#include "scene/gui/container.h"


class RichTextLabel;


class DynamicSpeechBalloon : public Container
{
	GDCLASS(DynamicSpeechBalloon, Container);

public:
	enum
	{
		INVALID_TAIL = -1,
	};

	enum TailAnchorMode
	{
		TAIL_ANCHOR_TOP_LEFT = 0,
		TAIL_ANCHOR_CENTER = 1,
	};

private:
	struct Tail final
	{
		Point2 position;
		float extension_ratio = 1.0f;
	};

private:
	static inline PropertyListHelper base_p_list_helper_tail;
	PropertyListHelper p_list_helper_tail;

	String name_text;
	HorizontalAlignment name_alignment = HORIZONTAL_ALIGNMENT_LEFT;

	TailAnchorMode tail_anchor_mode = TAIL_ANCHOR_TOP_LEFT;
	LocalVector<Tail> tail_list;

	RichTextLabel *dialogue_label;

	struct ThemeCache
	{
		Ref<Font> name_font;
		int name_font_size = 0;
		Color name_font_color;
		int name_font_outline_size = 0;
		Color name_font_outline_color;

		Ref<Font> dialogue_font_normal;
		Ref<Font> dialogue_font_italics;
		Ref<Font> dialogue_font_bold;
		Ref<Font> dialogue_font_bold_italics;
		Ref<Font> dialogue_font_mono;
	
		int dialogue_font_size = 0;
		Color dialogue_font_default_color;
		int dialogue_font_outline_size = 0;
		Color dialogue_font_outline_color;

		int dialogue_line_separation = 0;

		int inner_margin_left = 0;
		int inner_margin_top = 0;
		int inner_margin_right = 0;
		int inner_margin_bottom = 0;

		Color balloon_color;

		int balloon_outline_size = 0;
		Color balloon_outline_color;

		int tail_core_size = 16;
	} theme_cache;

	Tail &get_tail(const int p_index);
	const Tail &get_tail(const int p_index) const;

protected:
	static void _bind_methods();
	void _notification(const int p_what);
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	bool _property_can_revert(const StringName &p_name) const;
	bool _property_get_revert(const StringName &p_name, Variant &r_property) const;

public:
	bool has_tail(const int p_index) const;
	void set_tail_anchor_mode(const TailAnchorMode p_mode);
	TailAnchorMode get_tail_anchor_mode() const;
	void set_tail_count(const int p_count);
	int get_tail_count() const;
	void set_tail_position(const int p_index, const Point2 &p_point);
	Point2 get_tail_position(const int p_index) const;
	void set_tail_extension_ratio(const int p_index, const float p_ratio);
	float get_tail_extension_ratio(const int p_index) const;

	void set_name_text(const String &p_text);
	String get_name_text() const;
	void set_name_alignment(const HorizontalAlignment p_alignment);
	HorizontalAlignment get_name_alignment() const;

	void set_dialogue_text(const String &p_text);
	String get_dialogue_text() const;

	RichTextLabel *get_dialogue_label() const;

	virtual Vector<int> get_allowed_size_flags_horizontal() const override;
	virtual Vector<int> get_allowed_size_flags_vertical() const override;
	virtual Size2 get_minimum_size() const override;

	DynamicSpeechBalloon();
};


#endif // TALK_BALLOON_H
