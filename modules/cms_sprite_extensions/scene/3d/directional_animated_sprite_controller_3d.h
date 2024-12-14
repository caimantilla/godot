#ifndef DIRECTIONAL_ANIMATED_SPRITE_CONTROLLER_3D_H
#define DIRECTIONAL_ANIMATED_SPRITE_CONTROLLER_3D_H


#include "scene/3d/node_3d.h"


class AnimatedSprite3D;


class DirectionalAnimatedSpriteController3D : public Node3D
{
	friend class DirectionalAnimatedSpriteController3DEditorPlugin;

	GDCLASS(DirectionalAnimatedSpriteController3D, Node3D);

public:
	enum BillboardMode
	{
		BILLBOARD_DISABLE,
		BILLBOARD_COPY_CAMERA_ANGLE,
		BILLBOARD_LOOK_AT_CAMERA,
	};

	enum PixelAnchorMode
	{
		PIXEL_ANCHOR_DISABLE,
		PIXEL_ANCHOR_CENTER,
		PIXEL_ANCHOR_BOTTOM_MIDDLE,
	};

private:
	struct AnimDirection
	{
		StringName animation_name;
		Vector3 angle;
		Point2i pixel_offset;
		bool flip_h = false;
		bool flip_v = false;
		String notes;

		Vector3 cache_angle_point;
	};
	struct AnimData
	{
		Point2i pixel_offset;
		String notes;
		LocalVector<AnimDirection> direction_list;
	};

	bool disabled = false;
	NodePath path_sprite; // path to the AnimatedSprite3D
	BillboardMode billboard_mode = BILLBOARD_DISABLE;
	NodePath path_custom_camera; // a custom camera can be used by setting this. the angle will automatically be synced to the camera each frame
	bool use_custom_angle = false; // camera angles is ignored if this is enabled, with custom_angle being used instead
	Vector3 custom_angle;
	StringName current_animation_name;
	PixelAnchorMode pixel_anchor_mode = PIXEL_ANCHOR_DISABLE;
	HashMap<StringName, AnimData> anim_data_map;

	mutable PackedStringArray cache_animation_name_list;
	mutable bool cache_animation_name_list_dirty = false;
	Vector3 cache_angle;
	ObjectID cache_sprite_id;
	ObjectID cache_custom_camera_id;
	int last_animation_direction_index = -1;

	const Camera3D *get_camera_node() const;
	AnimatedSprite3D *get_sprite_node() const;
	const PackedStringArray get_sprite_animation_name_list() const;

	AnimData &get_or_create_anim_data(const StringName &p_anim);
	AnimData &get_anim_data(const StringName &p_anim);
	const AnimData &get_anim_data(const StringName &p_anim) const;
	bool has_anim_data(const StringName &p_anim) const;

	AnimDirection &get_anim_dir(const StringName &p_anim, const int p_dir);
	const AnimDirection &get_anim_dir(const StringName &p_anim, const int p_dir) const;
	bool has_anim_dir(const StringName &p_anim, const int p_dir) const;

	bool ensure_anim_exist(const StringName &p_anim);

	bool internal_add_animation(const StringName &p_name, const bool p_emit);
	bool internal_remove_animation(const StringName &p_name, const bool p_emit);

	void update_process();
	void update_cache();
	void update_animation();
	void update_direction(const bool p_reset_loop = false);
	void update_pixel_offset();

	void update_angle();

protected:
	static void _bind_methods();
	void _notification(int p_what);
	void _validate_property(PropertyInfo &p_property) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_path, const Variant &p_value);
	bool _get(const StringName &p_path, Variant &r_ret) const;
	bool _property_can_revert(const StringName &p_name) const;
	bool _property_get_revert(const StringName &p_name, Variant &r_value) const;

public:
	void set_disabled(const bool p_disabled);
	bool is_disabled() const;
	void set_sprite(const NodePath &p_path);
	NodePath get_sprite() const;
	void set_billboard_mode(const BillboardMode p_mode);
	BillboardMode get_billboard_mode() const;
	void set_custom_camera(const NodePath &p_path);
	NodePath get_custom_camera() const;
	void set_use_custom_angle(const bool p_enabled);
	bool is_using_custom_angle() const;
	void set_custom_angle(const Vector3 &p_angle);
	Vector3 get_custom_angle() const;
	void set_animation(const StringName &p_name);
	StringName get_animation() const;
	void set_pixel_anchor_mode(const PixelAnchorMode p_mode);
	PixelAnchorMode get_pixel_anchor_mode() const;

	void set_animation_pixel_offset(const StringName &p_animation, const Point2i &p_ofs);
	Point2i get_animation_pixel_offset(const StringName &p_animation) const;
	void set_animation_notes(const StringName &p_animation, const String &p_notes);
	String get_animation_notes(const StringName &p_animation) const;
	void set_animation_direction_count(const StringName &p_animation, const int p_count);
	int get_animation_direction_count(const StringName &p_animation) const;

	void set_animation_direction_animation_name(const StringName &p_animation, const int p_direction, const StringName &p_sprite_anim_name);
	StringName get_animation_direction_animation_name(const StringName &p_animation, const int p_direction) const;
	void set_animation_direction_angle(const StringName &p_animation, const int p_direction, const Vector3 &p_angle);
	Vector3 get_animation_direction_angle(const StringName &p_animation, const int p_direction) const;
	void set_animation_direction_pixel_offset(const StringName &p_animation, const int p_direction, const Point2i &p_ofs);
	Point2i get_animation_direction_pixel_offset(const StringName &p_animation, const int p_direction) const;
	void set_animation_direction_flip_h(const StringName &p_animation, const int p_direction, const bool p_enabled);
	bool is_animation_direction_flipped_h(const StringName &p_animation, const int p_direction) const;
	void set_animation_direction_flip_v(const StringName &p_animation, const int p_direction, const bool p_enabled);
	bool is_animation_direction_flipped_v(const StringName &p_animation, const int p_direction) const;
	void set_animation_direction_notes(const StringName &p_animation, const int p_direction, const String &p_notes);
	String get_animation_direction_notes(const StringName &p_animation, const int p_direction) const;

	bool rename_animation(const StringName &p_current_name, const StringName &p_new_name);
	bool has_animation(const StringName &p_name) const;
	bool has_animation_and_direction(const StringName &p_name, const int p_direction) const;
	bool can_add_animation(const StringName &p_name) const;
	bool add_animation(const StringName &p_name);
	bool can_remove_animation(const StringName &p_name) const;
	bool remove_animation(const StringName &p_name);
	PackedStringArray get_animation_names() const;
	int get_animation_count() const;

	void play(const StringName &p_anim_name, const double p_custom_speed = 1.0);

	PackedStringArray get_configuration_warnings() const override;

	DirectionalAnimatedSpriteController3D();
};


VARIANT_ENUM_CAST(DirectionalAnimatedSpriteController3D::BillboardMode);
VARIANT_ENUM_CAST(DirectionalAnimatedSpriteController3D::PixelAnchorMode);


#endif // DIRECTIONAL_ANIMATED_SPRITE_CONTROLLER_3D_H
