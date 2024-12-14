#include "directional_animated_sprite_controller_3d.h"
#include "scene/main/viewport.h"
#include "scene/3d/sprite_3d.h"
#include "scene/3d/camera_3d.h"


void DirectionalAnimatedSpriteController3D::play(const StringName &p_anim_name, const double p_custom_speed)
{
	AnimatedSprite3D *sprite = get_sprite_node();
	if (sprite != nullptr)
	{
		sprite->set_speed_scale(p_custom_speed);
	}
	update_animation();
}


bool DirectionalAnimatedSpriteController3D::rename_animation(const StringName &p_current, const StringName &p_new)
{
	if (p_current == p_new)
	{
		return false;
	}

	ERR_FAIL_COND_V_MSG(!has_animation(p_current), false, vformat(RTR("Animation \"%s\" does not exist."), p_current));
	ERR_FAIL_COND_V_MSG(!can_add_animation(p_new), false, vformat(RTR("Cannot change the name of \"%s\" to \"%s\"."), p_current, p_new));

	bool was_playing_changed_anim = current_animation_name == p_current;
	const AnimData anim_copy = anim_data_map[p_current];

	emit_signal(SNAME("removing_animation"), p_current);
	anim_data_map.erase(p_current);
	cache_animation_name_list_dirty = true;
	emit_signal(SNAME("animation_removed"), p_current);
	anim_data_map.insert(p_new, anim_copy);
	cache_animation_name_list_dirty = true;
	emit_signal(SNAME("animation_added"), p_new);
	if (was_playing_changed_anim)
	{
		set_animation(p_new);
	}
	else
	{
		set_animation(StringName());
	}
	notify_property_list_changed();
	return true;
}


bool DirectionalAnimatedSpriteController3D::has_animation(const StringName &p_name) const
{
	return anim_data_map.has(p_name);
}


bool DirectionalAnimatedSpriteController3D::has_animation_and_direction(const StringName &p_name, const int p_direction) const
{
	return p_direction > -1 && anim_data_map.has(p_name) && p_direction < anim_data_map[p_name].direction_list.size();
}


bool DirectionalAnimatedSpriteController3D::can_add_animation(const StringName &p_name) const
{
	return !anim_data_map.has(p_name) && String(p_name).is_valid_identifier();
}


bool DirectionalAnimatedSpriteController3D::add_animation(const StringName &p_name)
{
	return internal_add_animation(p_name, true);
}


bool DirectionalAnimatedSpriteController3D::can_remove_animation(const StringName &p_name) const
{
	return anim_data_map.has(p_name);
}


bool DirectionalAnimatedSpriteController3D::remove_animation(const StringName &p_name)
{
	return internal_remove_animation(p_name, true);
}


PackedStringArray DirectionalAnimatedSpriteController3D::get_animation_names() const
{
	if (cache_animation_name_list_dirty)
	{
		cache_animation_name_list_dirty = false;
		cache_animation_name_list.resize(anim_data_map.size());
		int i = 0;
		for (const KeyValue<StringName, AnimData> &E : anim_data_map)
		{
			cache_animation_name_list.write[i] = String(E.key);
			i++;
		}
		cache_animation_name_list.sort();
	}
	return cache_animation_name_list;
}


int DirectionalAnimatedSpriteController3D::get_animation_count() const
{
	return anim_data_map.size();
}


void DirectionalAnimatedSpriteController3D::set_disabled(const bool p_disabled)
{
	if (p_disabled == disabled)
	{
		return;
	}
	disabled = p_disabled;
	update_configuration_warnings();
	update_process();
	if (!disabled)
	{
		update_animation();
	}
}


bool DirectionalAnimatedSpriteController3D::is_disabled() const
{
	return disabled;
}


void DirectionalAnimatedSpriteController3D::set_sprite(const NodePath &p_path)
{
	if (p_path == path_sprite)
	{
		return;
	}
	path_sprite = p_path;
	notify_property_list_changed();
	update_configuration_warnings();
	update_cache();
	update_animation();
}


NodePath DirectionalAnimatedSpriteController3D::get_sprite() const
{
	return path_sprite;
}


void DirectionalAnimatedSpriteController3D::set_billboard_mode(const BillboardMode p_mode)
{
	if (p_mode == billboard_mode)
	{
		return;
	}
	billboard_mode = p_mode;
	notify_property_list_changed();
	update_configuration_warnings();
	update_direction();
}


DirectionalAnimatedSpriteController3D::BillboardMode DirectionalAnimatedSpriteController3D::get_billboard_mode() const
{
	return billboard_mode;
}


void DirectionalAnimatedSpriteController3D::set_custom_camera(const NodePath &p_path)
{
	if (p_path == path_custom_camera)
	{
		return;
	}
	path_custom_camera = p_path;
	notify_property_list_changed();
	update_cache();
	update_direction();
}


NodePath DirectionalAnimatedSpriteController3D::get_custom_camera() const
{
	return path_custom_camera;
}


void DirectionalAnimatedSpriteController3D::set_use_custom_angle(const bool p_enabled)
{
	if (p_enabled == use_custom_angle)
	{
		return;
	}
	use_custom_angle = p_enabled;
	notify_property_list_changed();
	update_direction();
}


bool DirectionalAnimatedSpriteController3D::is_using_custom_angle() const
{
	return use_custom_angle;
}


void DirectionalAnimatedSpriteController3D::set_custom_angle(const Vector3 &p_angle)
{
	custom_angle = p_angle;
	update_direction();
}


Vector3 DirectionalAnimatedSpriteController3D::get_custom_angle() const
{
	return custom_angle;
}


void DirectionalAnimatedSpriteController3D::set_animation(const StringName &p_name)
{
	if (p_name == current_animation_name)
	{
		return;
	}
	current_animation_name = p_name;
	update_animation();
}


StringName DirectionalAnimatedSpriteController3D::get_animation() const
{
	return current_animation_name;
}


void DirectionalAnimatedSpriteController3D::set_pixel_anchor_mode(const PixelAnchorMode p_mode)
{
	if (p_mode == pixel_anchor_mode)
	{
		return;
	}
	pixel_anchor_mode = p_mode;
	notify_property_list_changed();
	update_animation();
}


DirectionalAnimatedSpriteController3D::PixelAnchorMode DirectionalAnimatedSpriteController3D::get_pixel_anchor_mode() const
{
	return pixel_anchor_mode;
}


void DirectionalAnimatedSpriteController3D::set_animation_pixel_offset(const StringName &p_anim, const Point2i &p_ofs)
{
	ERR_FAIL_COND(!has_anim_data(p_anim));
	AnimData &anim = get_anim_data(p_anim);
	if (anim.pixel_offset == p_ofs)
	{
		return;
	}
	anim.pixel_offset = p_ofs;
	update_animation();
}


Point2i DirectionalAnimatedSpriteController3D::get_animation_pixel_offset(const StringName &p_anim) const
{
	ERR_FAIL_COND_V(!has_anim_data(p_anim), Point2i());
	return get_anim_data(p_anim).pixel_offset;
}


void DirectionalAnimatedSpriteController3D::set_animation_notes(const StringName &p_anim, const String &p_notes)
{
	ERR_FAIL_COND(!has_anim_data(p_anim));
	AnimData &anim = get_anim_data(p_anim);
	if (anim.notes == p_notes)
	{
		return;
	}
	anim.notes = p_notes;
}


String DirectionalAnimatedSpriteController3D::get_animation_notes(const StringName &p_anim) const
{
	ERR_FAIL_COND_V(!has_anim_data(p_anim), String());
	return get_anim_data(p_anim).notes;
}


void DirectionalAnimatedSpriteController3D::set_animation_direction_count(const StringName &p_anim, const int p_count)
{
	ERR_FAIL_COND(!has_anim_data(p_anim));
	ERR_FAIL_COND(p_count < 0);
	get_anim_data(p_anim).direction_list.resize(p_count);
	notify_property_list_changed();
	update_direction();
}


int DirectionalAnimatedSpriteController3D::get_animation_direction_count(const StringName &p_anim) const
{
	ERR_FAIL_COND_V(!has_anim_data(p_anim), 0);
	return get_anim_data(p_anim).direction_list.size();
}


void DirectionalAnimatedSpriteController3D::set_animation_direction_animation_name(const StringName &p_anim, const int p_direction, const StringName &p_sprite_anim_name)
{
	ERR_FAIL_COND(!has_anim_dir(p_anim, p_direction));
	AnimDirection &dir = get_anim_dir(p_anim, p_direction);
	if (p_sprite_anim_name == dir.animation_name)
	{
		return;
	}
	dir.animation_name = p_sprite_anim_name;
	update_direction();
}


StringName DirectionalAnimatedSpriteController3D::get_animation_direction_animation_name(const StringName &p_anim, const int p_direction) const
{
	ERR_FAIL_COND_V(!has_anim_dir(p_anim, p_direction), StringName());
	return get_anim_dir(p_anim, p_direction).animation_name;
}


void DirectionalAnimatedSpriteController3D::set_animation_direction_angle(const StringName &p_anim, const int p_direction, const Vector3 &p_angle)
{
	ERR_FAIL_COND(!has_anim_dir(p_anim, p_direction));
	get_anim_dir(p_anim, p_direction).angle = p_angle;
	update_direction();
}


Vector3 DirectionalAnimatedSpriteController3D::get_animation_direction_angle(const StringName &p_anim, const int p_direction) const
{
	ERR_FAIL_COND_V(!has_anim_dir(p_anim, p_direction), Vector3());
	return get_anim_dir(p_anim, p_direction).angle;
}


void DirectionalAnimatedSpriteController3D::set_animation_direction_pixel_offset(const StringName &p_anim, const int p_direction, const Point2i &p_ofs)
{
	ERR_FAIL_COND(!has_anim_dir(p_anim, p_direction));
	AnimDirection &dir = get_anim_dir(p_anim, p_direction);
	if (dir.pixel_offset == p_ofs)
	{
		return;
	}
	dir.pixel_offset = p_ofs;
	update_direction();
}


Point2i DirectionalAnimatedSpriteController3D::get_animation_direction_pixel_offset(const StringName &p_anim, const int p_direction) const
{
	ERR_FAIL_COND_V(!has_anim_dir(p_anim, p_direction), Point2i());
	return get_anim_dir(p_anim, p_direction).pixel_offset;
}


void DirectionalAnimatedSpriteController3D::set_animation_direction_flip_h(const StringName &p_anim, const int p_direction, const bool p_flip)
{
	ERR_FAIL_COND(!has_anim_dir(p_anim, p_direction));
	AnimDirection &dir = get_anim_dir(p_anim, p_direction);
	if (p_flip == dir.flip_h)
	{
		return;
	}
	dir.flip_h = p_flip;
	update_direction();
}


bool DirectionalAnimatedSpriteController3D::is_animation_direction_flipped_h(const StringName &p_anim, const int p_direction) const
{
	ERR_FAIL_COND_V(!has_anim_dir(p_anim, p_direction), false);
	return get_anim_dir(p_anim, p_direction).flip_h;
}


void DirectionalAnimatedSpriteController3D::set_animation_direction_flip_v(const StringName &p_anim, const int p_direction, const bool p_flip)
{
	ERR_FAIL_COND(!has_anim_dir(p_anim, p_direction));
	AnimDirection &dir = get_anim_dir(p_anim, p_direction);
	if (p_flip == dir.flip_v)
	{
		return;
	}
	dir.flip_v = p_flip;
	update_direction();
}


bool DirectionalAnimatedSpriteController3D::is_animation_direction_flipped_v(const StringName &p_anim, const int p_direction) const
{
	ERR_FAIL_COND_V(!has_anim_dir(p_anim, p_direction), false);
	return get_anim_dir(p_anim, p_direction).flip_v;
}


void DirectionalAnimatedSpriteController3D::set_animation_direction_notes(const StringName &p_anim, const int p_direction, const String &p_notes)
{
	ERR_FAIL_COND(!has_anim_dir(p_anim, p_direction));
	AnimDirection &dir = get_anim_dir(p_anim, p_direction);
	if (dir.notes == p_notes)
	{
		return;
	}
	dir.notes = p_notes;
	update_direction();
}


String DirectionalAnimatedSpriteController3D::get_animation_direction_notes(const StringName &p_anim, const int p_direction) const
{
	ERR_FAIL_COND_V(!has_anim_dir(p_anim, p_direction), String());
	return get_anim_dir(p_anim, p_direction).notes;
}


bool DirectionalAnimatedSpriteController3D::internal_add_animation(const StringName &p_name, const bool p_emit)
{
	ERR_FAIL_COND_V(!can_add_animation(p_name), false);
	anim_data_map.insert(p_name, AnimData());
	cache_animation_name_list_dirty = true;
	if (p_emit)
	{
		emit_signal(SNAME("animation_added"), p_name);
		set_animation(p_name); // Should the animation autoplay when p_emit is true?? hmmmmmm (i think yes)
		notify_property_list_changed();
	}
	return true;
}


bool DirectionalAnimatedSpriteController3D::internal_remove_animation(const StringName &p_name, const bool p_emit)
{
	ERR_FAIL_COND_V(!can_remove_animation(p_name), false);
	if (p_emit)
	{
		emit_signal(SNAME("removing_animation"), p_name);
	}

	// If the currently-playing animation is deleted, the animation should be changed.
	// Nullification should only occur if there are no other animations available.
	bool removing_playing_animation = current_animation_name == p_name;
	StringName new_anim_name;

	if (removing_playing_animation)
	{
		const PackedStringArray anim_names = get_animation_names();
		const int alpha_idx_to_remove = anim_names.find(p_name);

		if (alpha_idx_to_remove != -1)
		{
			if (alpha_idx_to_remove < anim_names.size() - 1)
			{
				new_anim_name = anim_names[alpha_idx_to_remove + 1];
			}
			else if (alpha_idx_to_remove > 0)
			{
				new_anim_name = anim_names[alpha_idx_to_remove - 1];
			}
		}
	}

	anim_data_map.erase(p_name);
	cache_animation_name_list_dirty = true;

	if (p_emit)
	{
		emit_signal(SNAME("animation_removed"), p_name);
		if (removing_playing_animation)
		{
			set_animation(new_anim_name);
		}
		notify_property_list_changed();
	}

	return true;
}


void DirectionalAnimatedSpriteController3D::update_process()
{
	set_process(!disabled);
}


void DirectionalAnimatedSpriteController3D::update_cache()
{
	if (!is_inside_tree())
	{
		return;
	}

	if (cache_sprite_id.is_valid())
	{
		AnimatedSprite3D *existing_sprite = Object::cast_to<AnimatedSprite3D>(ObjectDB::get_instance(cache_sprite_id));
		if (existing_sprite != nullptr)
		{
			Callable callable_update_ofs = callable_mp(this, &DirectionalAnimatedSpriteController3D::update_pixel_offset);
			existing_sprite->disconnect(SceneStringName(animation_changed), callable_update_ofs);
			existing_sprite->disconnect(SceneStringName(frame_changed), callable_update_ofs);
		}
	}

	cache_custom_camera_id = ObjectID();
	cache_sprite_id = ObjectID();

	if (has_node(path_custom_camera))
	{
		const Camera3D *new_camera = Object::cast_to<Camera3D>(get_node(path_custom_camera));
		if (new_camera != nullptr)
		{
			cache_custom_camera_id = new_camera->get_instance_id();
		}
	}
	if (has_node(path_sprite))
	{
		AnimatedSprite3D *new_sprite = Object::cast_to<AnimatedSprite3D>(get_node(path_sprite));
		if (new_sprite != nullptr)
		{
			cache_sprite_id = new_sprite->get_instance_id();

			Callable callable_update_ofs = callable_mp(this, &DirectionalAnimatedSpriteController3D::update_pixel_offset);
			new_sprite->connect(SceneStringName(animation_changed), callable_update_ofs);
			new_sprite->connect(SceneStringName(frame_changed), callable_update_ofs);
		}
	}
}


void DirectionalAnimatedSpriteController3D::update_animation()
{
	if (disabled)
	{
		return;
	}
	update_direction(true);
}


void DirectionalAnimatedSpriteController3D::update_direction(const bool p_reset_loop)
{
	if (disabled)
	{
		return;
	}
	if (!is_inside_tree())
	{
		return;
	}

	AnimatedSprite3D *sprite = get_sprite_node();
	if (sprite == nullptr || !sprite->is_inside_tree())
	{
		return;
	}
	const Ref<SpriteFrames> sprite_frames = sprite->get_sprite_frames();
	if (!sprite_frames.is_valid())
	{
		return;
	}

	update_angle();

	if (anim_data_map.has(current_animation_name))
	{
		const EulerOrder rot_order = get_rotation_order();

		const AnimData &anim = anim_data_map[current_animation_name];
		const Vector3 used_angle = cache_angle;

		int closest_idx = -1;
		real_t closest_dist = INFINITY;

		for (int dir_idx = 0; dir_idx < anim.direction_list.size(); dir_idx++)
		{
			const Vector3 dir_angle = Basis::from_euler(anim.direction_list[dir_idx].angle, rot_order).orthonormalized().get_euler(rot_order);
			const real_t dir_dist = used_angle.distance_squared_to(dir_angle);
			if (dir_dist < closest_dist)
			{
				closest_dist = dir_dist;
				closest_idx = dir_idx;
			}
		}

		// for use in other methods
		last_animation_direction_index = closest_idx;

		if (closest_idx != -1)
		{
			const AnimDirection &anim_dir = anim.direction_list[closest_idx];
			const StringName new_sprite_anim_name = anim_dir.animation_name;

			sprite->set_flip_h(anim_dir.flip_h);
			sprite->set_flip_v(anim_dir.flip_v);

			// If the loop should be reset, then the animation matching the already-playing one shouldn't matter, as it needs to be reset!!
			if (!p_reset_loop && new_sprite_anim_name == sprite->get_animation())
			{
				return;
			}
			// Also, fail if the new animation doesn't exist.
			if (!sprite_frames->has_animation(new_sprite_anim_name))
			{
				return;
			}

			const int frame = sprite->get_frame();
			const real_t frame_prog = sprite->get_frame_progress();

			sprite->play(new_sprite_anim_name, sprite->get_speed_scale());
			if (!p_reset_loop)
			{
				sprite->set_frame_and_progress(frame, frame_prog);
			}
		}
	}
}


void DirectionalAnimatedSpriteController3D::update_pixel_offset()
{
	if (disabled)
	{
		return;
	}
	if (pixel_anchor_mode == PIXEL_ANCHOR_DISABLE || !is_inside_tree() || !has_anim_dir(current_animation_name, last_animation_direction_index))
	{
		return;
	}
	AnimatedSprite3D *sprite = get_sprite_node();
	if (sprite == nullptr || !sprite->is_inside_tree())
	{
		return;
	}
	const StringName curr_sprite_anim_name = sprite->get_animation();
	const int curr_sprite_frame_index = sprite->get_frame();
	const Ref<SpriteFrames> sprite_frames = sprite->get_sprite_frames();
	if (!sprite_frames.is_valid() || !sprite_frames->has_animation(curr_sprite_anim_name) || curr_sprite_frame_index < 0 || curr_sprite_frame_index >= sprite_frames->get_frame_count(curr_sprite_anim_name))
	{
		return;
	}
	const Ref<Texture2D> frame_tex = sprite_frames->get_frame_texture(curr_sprite_anim_name, curr_sprite_frame_index);
	if (!frame_tex.is_valid())
	{
		return;
	}
	const Size2i frame_tex_size = frame_tex->get_size().floor();

	const AnimData &anim = anim_data_map[current_animation_name];
	const AnimDirection &anim_dir = anim.direction_list[last_animation_direction_index];

	Point2i pixel_ofs;

	switch (pixel_anchor_mode)
	{
		case PIXEL_ANCHOR_CENTER: {
			pixel_ofs.x = frame_tex_size.width / 2;
			pixel_ofs.y = (-frame_tex_size.height) / 2;
		} break;
		case PIXEL_ANCHOR_BOTTOM_MIDDLE: {
			pixel_ofs.x = frame_tex_size.width / 2;
		} break;
		default: break;
	}

	pixel_ofs += anim.pixel_offset;
	pixel_ofs += anim_dir.pixel_offset;

	sprite->set_centered(false);
	sprite->set_offset(pixel_ofs);
}


void DirectionalAnimatedSpriteController3D::update_angle()
{
	const Camera3D *camera = get_camera_node();
	AnimatedSprite3D *sprite = get_sprite_node();
	const EulerOrder rot_order = get_rotation_order();

	if (camera == nullptr || sprite == nullptr || !camera->is_inside_tree() || !sprite->is_inside_tree())
	{
		return;
	}

	if (use_custom_angle)
	{
		Basis intermediate_basis = Basis::from_euler(custom_angle, rot_order);
		intermediate_basis = intermediate_basis.orthonormalized();
		cache_angle = intermediate_basis.get_euler(rot_order);
	}
	else
	{
		const Transform3D camera_trans = camera->get_global_transform();
		const Transform3D own_trans = get_global_transform();
		const Transform3D trans_looking = own_trans.looking_at(camera_trans.origin);
		// const Transform3D trans_looking = camera_trans.looking_at(own_trans.origin);

		Transform3D adjusted_trans = own_trans;
		// invert to get the desired result... i guess
		adjusted_trans.basis = Basis(own_trans.basis.get_rotation_quaternion() * trans_looking.basis.get_rotation_quaternion());
		adjusted_trans.basis = adjusted_trans.basis.orthonormalized();

		cache_angle = adjusted_trans.basis.get_euler(rot_order);
	}

	switch (billboard_mode)
	{
		case BILLBOARD_COPY_CAMERA_ANGLE: {
			const Vector3 camera_rot = camera->get_global_transform().basis.get_euler(sprite->get_rotation_order());
			sprite->set_global_rotation(camera_rot);
		} break;
		case BILLBOARD_LOOK_AT_CAMERA: {
			sprite->look_at(camera->get_global_transform().origin, camera->get_global_transform().basis.get_column(Vector3::AXIS_Y));
		} break;
		default: break;
	}
}


const Camera3D *DirectionalAnimatedSpriteController3D::get_camera_node() const
{
	const Camera3D *cam = nullptr;
	if (cache_custom_camera_id.is_valid())
	{
		cam = Object::cast_to<Camera3D>(ObjectDB::get_instance(cache_custom_camera_id));
	}
	if (cam == nullptr && is_inside_tree())
	{
		Viewport *vp = get_viewport();
		if (vp != nullptr)
		{
			cam = vp->get_camera_3d();
		}
	}
	if (cam != nullptr && !cam->is_inside_tree())
	{
		cam = nullptr;
	}
	return cam;
}


AnimatedSprite3D *DirectionalAnimatedSpriteController3D::get_sprite_node() const
{
	if (cache_sprite_id.is_valid())
	{
		return Object::cast_to<AnimatedSprite3D>(ObjectDB::get_instance(cache_sprite_id));
	}
	return nullptr;
}


const PackedStringArray DirectionalAnimatedSpriteController3D::get_sprite_animation_name_list() const
{
	static const PackedStringArray dummy_ret;
	const AnimatedSprite3D *sprite_node = get_sprite_node();

	if (sprite_node != nullptr)
	{
		const Ref<SpriteFrames> frames = sprite_node->get_sprite_frames();
		if (frames.is_valid())
		{
			return frames->get_animation_names();
		}
	}

	return dummy_ret;
}


DirectionalAnimatedSpriteController3D::AnimData &DirectionalAnimatedSpriteController3D::get_or_create_anim_data(const StringName &p_anim)
{
	static AnimData dummy_ret;
	if (!anim_data_map.has(p_anim))
	{
		internal_add_animation(p_anim, false);
	}
	ERR_FAIL_COND_V(!anim_data_map.has(p_anim), dummy_ret);
	return anim_data_map[p_anim];
}


DirectionalAnimatedSpriteController3D::AnimData &DirectionalAnimatedSpriteController3D::get_anim_data(const StringName &p_anim)
{
	static AnimData dummy_ret;
	ERR_FAIL_COND_V(!anim_data_map.has(p_anim), dummy_ret);
	return anim_data_map[p_anim];
}


const DirectionalAnimatedSpriteController3D::AnimData &DirectionalAnimatedSpriteController3D::get_anim_data(const StringName &p_anim) const
{
	static const AnimData dummy_ret;
	ERR_FAIL_COND_V(!anim_data_map.has(p_anim), dummy_ret);
	return anim_data_map[p_anim];
}


bool DirectionalAnimatedSpriteController3D::has_anim_data(const StringName &p_anim) const
{
	return anim_data_map.has(p_anim);
}


DirectionalAnimatedSpriteController3D::AnimDirection &DirectionalAnimatedSpriteController3D::get_anim_dir(const StringName &p_anim, int p_dir)
{
	static AnimDirection dummy_ret;
	ERR_FAIL_COND_V(!has_anim_dir(p_anim, p_dir), dummy_ret);
	return anim_data_map[p_anim].direction_list[p_dir];
}


const DirectionalAnimatedSpriteController3D::AnimDirection &DirectionalAnimatedSpriteController3D::get_anim_dir(const StringName &p_anim, int p_dir) const
{
	static const AnimDirection dummy_ret;
	ERR_FAIL_COND_V(!has_anim_dir(p_anim, p_dir), dummy_ret);
	return anim_data_map[p_anim].direction_list[p_dir];
}


bool DirectionalAnimatedSpriteController3D::has_anim_dir(const StringName &p_anim, int p_dir) const
{
	return (p_dir > -1 && anim_data_map.has(p_anim) && p_dir < anim_data_map[p_anim].direction_list.size());
}


bool DirectionalAnimatedSpriteController3D::ensure_anim_exist(const StringName &p_anim)
{
	if (anim_data_map.has(p_anim))
	{
		return true;
	}
	else
	{
		return internal_add_animation(p_anim, false);
	}
}


PackedStringArray DirectionalAnimatedSpriteController3D::get_configuration_warnings() const
{
	PackedStringArray warnings = Node::get_configuration_warnings();

	if (disabled)
	{
		warnings.push_back(RTR("The controller is disabled."));
	}
	if (!has_node(path_sprite) || nullptr == Object::cast_to<AnimatedSprite3D>(get_node(path_sprite)))
	{
		warnings.push_back(RTR("Path property must point to a valid AnimatedSprite3D node to work."));
	}
	else
	{
		AnimatedSprite3D *sprite = Object::cast_to<AnimatedSprite3D>(get_node(path_sprite));
		if (sprite->is_ancestor_of(this))
		{
			warnings.push_back(RTR("The sprite being an ancestor of the controller may cause malfunctions. Please rearrange the scene."));
		}
		else if (is_ancestor_of(sprite))
		{
			warnings.push_back(RTR("The sprite being a descendant of the controller may cause malfunctions. Please rearrange the scene."));
		}
	}

	return warnings;
}


bool DirectionalAnimatedSpriteController3D::_set(const StringName &p_name, const Variant &p_value)
{
	String name = p_name;
	if (name.begins_with("animations/"))
	{
		const StringName anim_name = name.get_slicec('/', 1);
		ERR_FAIL_COND_V(!ensure_anim_exist(anim_name), false);

		const String anim_property = name.get_slicec('/', 2);

		if (anim_property == "name")
		{
			rename_animation(anim_name, p_value);
			return true;
		}
		else if (anim_property == "pixel_offset")
		{
			set_animation_pixel_offset(anim_name, p_value);
			return true;
		}
		else if (anim_property == "notes")
		{
			set_animation_notes(anim_name, p_value);
			return true;
		}
		else if (anim_property == "direction_count")
		{
			set_animation_direction_count(anim_name, p_value);
			return true;
		}
		else if (anim_property == "directions")
		{
			const int dir_idx = name.get_slicec('/', 3).to_int();
			ERR_FAIL_COND_V(!has_animation_and_direction(anim_name, dir_idx), false);
			const String dir_property = name.get_slicec('/', 4);

			if (dir_property == "sprite_animation_name")
			{
				set_animation_direction_animation_name(anim_name, dir_idx, p_value);
				return true;
			}
			else if (dir_property == "angle")
			{
				set_animation_direction_angle(anim_name, dir_idx, p_value);
				return true;
			}
			else if (dir_property == "pixel_offset")
			{
				set_animation_direction_pixel_offset(anim_name, dir_idx, p_value);
				return true;
			}
			else if (dir_property == "flip_h")
			{
				set_animation_direction_flip_h(anim_name, dir_idx, p_value);
				return true;
			}
			else if (dir_property == "flip_v")
			{
				set_animation_direction_flip_v(anim_name, dir_idx, p_value);
				return true;
			}
			else if (dir_property == "notes")
			{
				set_animation_direction_notes(anim_name, dir_idx, p_value);
				return true;
			}
		}
	}
	return false;
}


bool DirectionalAnimatedSpriteController3D::_get(const StringName &p_name, Variant &r_ret) const
{
	String name = p_name;
	if (name.begins_with("animations/"))
	{
		const String anim_name = name.get_slicec('/', 1);
#ifdef TOOLS_ENABLED
		if (anim_name.begins_with("_editor_hook"))
		{
			return false;
		}
#endif // TOOLS_ENABLED
		ERR_FAIL_COND_V(!has_animation(anim_name), false);
		const String anim_property = name.get_slicec('/', 2);

		if (anim_property == "name")
		{
			r_ret = anim_name;
			return true;
		}
		else if (anim_property == "pixel_offset")
		{
			r_ret = get_animation_pixel_offset(anim_name);
			return true;
		}
		else if (anim_property == "notes")
		{
			r_ret = get_animation_notes(anim_name);
			return true;
		}
		else if (anim_property == "direction_count")
		{
			r_ret = get_animation_direction_count(anim_name);
			return true;
		}
		else if (anim_property == "directions")
		{
			const int dir_idx = name.get_slicec('/', 3).to_int();
			ERR_FAIL_COND_V(!has_animation_and_direction(anim_name, dir_idx), false);
			const String dir_property = name.get_slicec('/', 4);

			if (dir_property == "sprite_animation_name")
			{
				r_ret = get_animation_direction_animation_name(anim_name, dir_idx);
				return true;
			}
			else if (dir_property == "angle")
			{
				r_ret = get_animation_direction_angle(anim_name, dir_idx);
				return true;
			}
			else if (dir_property == "pixel_offset")
			{
				r_ret = get_animation_direction_pixel_offset(anim_name, dir_idx);
				return true;
			}
			else if (dir_property == "flip_h")
			{
				r_ret = is_animation_direction_flipped_h(anim_name, dir_idx);
				return true;
			}
			else if (dir_property == "flip_v")
			{
				r_ret = is_animation_direction_flipped_v(anim_name, dir_idx);
				return true;
			}
			else if (dir_property == "notes")
			{
				r_ret = get_animation_direction_notes(anim_name, dir_idx);
				return true;
			}
		}
	}
	return false;
}


bool DirectionalAnimatedSpriteController3D::_property_can_revert(const StringName &p_name) const
{
	return String(p_name).begins_with("animations/");
}


bool DirectionalAnimatedSpriteController3D::_property_get_revert(const StringName &p_name, Variant &r_ret) const
{
	String name = p_name;
	if (name.begins_with("animations/"))
	{
		if (name.ends_with("/pixel_offset"))
		{
			r_ret = Point2i();
			return true;
		}
		else if (name.ends_with("/notes"))
		{
			r_ret = String();
			return true;
		}
		else if (name.ends_with("/direction_count"))
		{
			r_ret = 0;
			return true;
		}
		else if (name.ends_with("/sprite_animation_name"))
		{
			r_ret = StringName();
			return true;
		}
		else if (name.ends_with("/angle"))
		{
			r_ret = Vector3();
			return true;
		}
		else if (name.ends_with("/flip_h"))
		{
			r_ret = false;
			return true;
		}
		else if (name.ends_with("/flip_v"))
		{
			r_ret = false;
			return true;
		}
	}
	return false;
}


void DirectionalAnimatedSpriteController3D::_get_property_list(List<PropertyInfo> *p_list) const
{
	const PackedStringArray sprite_animation_name_list = get_sprite_animation_name_list();
	const String sprite_animation_name_hint_string = sprite_animation_name_list.is_empty() ? String() : String::chr(',').join(sprite_animation_name_list);

	const PropertyInfo pi_group_animations = PropertyInfo(Variant::NIL, "Animations", PROPERTY_HINT_NONE, "animations/", PROPERTY_USAGE_GROUP);
	// p_list->push_back(pi_group_animations);

#ifdef TOOLS_ENABLED
	if (Engine::get_singleton()->is_editor_hint())
	{
		p_list->push_back(PropertyInfo(Variant::NIL, "animations/_editor_hook_animation_list_begin", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_EDITOR));
	}
#endif // TOOLS_ENABLED

	for (const KeyValue<StringName, AnimData> &E : anim_data_map)
	{
		const String prefix_anim = "animations/" + E.key + "/";
		const AnimData &anim_data = E.value;

		const PropertyInfo pi_subgroup_anim = PropertyInfo(Variant::NIL, "animations/" + E.key, PROPERTY_HINT_NONE, prefix_anim, PROPERTY_USAGE_SUBGROUP);
		PropertyInfo pi_anim_pixel_offset = PropertyInfo(Variant::VECTOR2I, prefix_anim + "pixel_offset");
		const PropertyInfo pi_anim_notes = PropertyInfo(Variant::STRING, prefix_anim + "notes", PROPERTY_HINT_MULTILINE_TEXT);
		const PropertyInfo pi_anim_direction_count = PropertyInfo(Variant::INT, prefix_anim + "direction_count", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_ARRAY, "Directions," + prefix_anim + "directions/");

		if (pixel_anchor_mode == PIXEL_ANCHOR_DISABLE)
		{
			pi_anim_pixel_offset.usage = PROPERTY_USAGE_NONE;
		}

#ifdef TOOLS_ENABLED
		if (Engine::get_singleton()->is_editor_hint())
		{
			p_list->push_back(PropertyInfo(Variant::NIL, prefix_anim + "_editor_hook_animation", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_EDITOR));
		}
#endif // TOOLS_ENABLED

		// p_list->push_back(pi_subgroup_anim);
		p_list->push_back(pi_anim_pixel_offset);
		p_list->push_back(pi_anim_notes);
		p_list->push_back(pi_anim_direction_count);

		for (int dir_idx = 0; dir_idx < anim_data.direction_list.size(); dir_idx++)
		{
			const String prefix_dir = prefix_anim + "directions/" + itos(dir_idx) + "/";

			PropertyInfo pi_dir_animation_name = PropertyInfo(Variant::STRING_NAME, prefix_dir + "sprite_animation_name");
			const PropertyInfo pi_dir_angle = PropertyInfo(Variant::VECTOR3, prefix_dir + "angle", PROPERTY_HINT_RANGE, "-360,360,0.1,radians_as_degrees");
			PropertyInfo pi_dir_pixel_offset = PropertyInfo(Variant::VECTOR2I, prefix_dir + "pixel_offset");
			const PropertyInfo pi_dir_flip_h = PropertyInfo(Variant::BOOL, prefix_dir + "flip_h");
			const PropertyInfo pi_dir_flip_v = PropertyInfo(Variant::BOOL, prefix_dir + "flip_v");
			const PropertyInfo pi_dir_notes = PropertyInfo(Variant::STRING, prefix_dir + "notes", PROPERTY_HINT_MULTILINE_TEXT);

			if (!sprite_animation_name_hint_string.is_empty())
			{
				pi_dir_animation_name.hint = PROPERTY_HINT_ENUM_SUGGESTION;
				pi_dir_animation_name.hint_string = sprite_animation_name_hint_string;
			}
			if (pixel_anchor_mode == PIXEL_ANCHOR_DISABLE)
			{
				pi_dir_pixel_offset.usage = PROPERTY_USAGE_NONE;
			}

			p_list->push_back(pi_dir_animation_name);
			p_list->push_back(pi_dir_angle);
			p_list->push_back(pi_dir_pixel_offset);
			p_list->push_back(pi_dir_flip_h);
			p_list->push_back(pi_dir_flip_v);
			p_list->push_back(pi_dir_notes);
		}
	}

#ifdef TOOLS_ENABLED
	if (Engine::get_singleton()->is_editor_hint())
	{
		p_list->push_back(PropertyInfo(Variant::NIL, "animations/_editor_hook_animation_list_end", PROPERTY_HINT_NONE, String(), PROPERTY_USAGE_EDITOR));
	}
#endif // TOOLS_ENABLED
}


void DirectionalAnimatedSpriteController3D::_validate_property(PropertyInfo &p_property) const
{
	if (p_property.name == "custom_angle")
	{
		if (!use_custom_angle)
		{
			p_property.usage = PROPERTY_USAGE_NONE;
		}
	}
	else if (p_property.name == "animation")
	{
		const PackedStringArray anim_names = get_animation_names();
		if (anim_names.is_empty())
		{
			p_property.usage = PROPERTY_USAGE_NONE;
		}
		else
		{
			p_property.hint = PROPERTY_HINT_ENUM_SUGGESTION;
			p_property.hint_string = String::chr(',').join(anim_names);
		}
	}
}


void DirectionalAnimatedSpriteController3D::_notification(int p_what)
{
	switch (p_what)
	{
		case NOTIFICATION_ENTER_TREE: {
			update_cache();
		} break;
		case NOTIFICATION_EDITOR_PRE_SAVE: {
			if (billboard_mode != BILLBOARD_DISABLE)
			{
				AnimatedSprite3D *sprite = get_sprite_node();
				if (sprite != nullptr)
				{
					Vector3 rot_revert = sprite->property_get_revert(SNAME("rotation"));
					sprite->set_rotation(rot_revert);
				}
			}
		} break;
		case NOTIFICATION_POST_ENTER_TREE: {
			update_animation();
		} break;
		case NOTIFICATION_PROCESS: {
			update_direction();
		} break;
		default: break;
	}
}


void DirectionalAnimatedSpriteController3D::_bind_methods()
{
	BIND_ENUM_CONSTANT(BILLBOARD_DISABLE);
	BIND_ENUM_CONSTANT(BILLBOARD_COPY_CAMERA_ANGLE);
	BIND_ENUM_CONSTANT(BILLBOARD_LOOK_AT_CAMERA);

	BIND_ENUM_CONSTANT(PIXEL_ANCHOR_DISABLE);
	BIND_ENUM_CONSTANT(PIXEL_ANCHOR_CENTER);
	BIND_ENUM_CONSTANT(PIXEL_ANCHOR_BOTTOM_MIDDLE);

	ClassDB::bind_method(D_METHOD("set_disabled", "disabled"), &DirectionalAnimatedSpriteController3D::set_disabled);
	ClassDB::bind_method(D_METHOD("is_disabled"), &DirectionalAnimatedSpriteController3D::is_disabled);
	ClassDB::bind_method(D_METHOD("set_sprite", "node_path"), &DirectionalAnimatedSpriteController3D::set_sprite);
	ClassDB::bind_method(D_METHOD("get_sprite"), &DirectionalAnimatedSpriteController3D::get_sprite);
	ClassDB::bind_method(D_METHOD("set_billboard_mode", "mode"), &DirectionalAnimatedSpriteController3D::set_billboard_mode);
	ClassDB::bind_method(D_METHOD("get_billboard_mode"), &DirectionalAnimatedSpriteController3D::get_billboard_mode);
	ClassDB::bind_method(D_METHOD("set_custom_camera", "node_path"), &DirectionalAnimatedSpriteController3D::set_custom_camera);
	ClassDB::bind_method(D_METHOD("get_custom_camera"), &DirectionalAnimatedSpriteController3D::get_custom_camera);
	ClassDB::bind_method(D_METHOD("set_use_custom_angle", "enabled"), &DirectionalAnimatedSpriteController3D::set_use_custom_angle);
	ClassDB::bind_method(D_METHOD("is_using_custom_angle"), &DirectionalAnimatedSpriteController3D::is_using_custom_angle);
	ClassDB::bind_method(D_METHOD("set_custom_angle", "angle_rad"), &DirectionalAnimatedSpriteController3D::set_custom_angle);
	ClassDB::bind_method(D_METHOD("get_custom_angle"), &DirectionalAnimatedSpriteController3D::get_custom_angle);
	ClassDB::bind_method(D_METHOD("set_animation", "anim_name"), &DirectionalAnimatedSpriteController3D::set_animation);
	ClassDB::bind_method(D_METHOD("get_animation"), &DirectionalAnimatedSpriteController3D::get_animation);
	ClassDB::bind_method(D_METHOD("set_pixel_anchor_mode", "mode"), &DirectionalAnimatedSpriteController3D::set_pixel_anchor_mode);
	ClassDB::bind_method(D_METHOD("get_pixel_anchor_mode"), &DirectionalAnimatedSpriteController3D::get_pixel_anchor_mode);

	ClassDB::bind_method(D_METHOD("set_animation_pixel_offset", "anim_name", "ofs"), &DirectionalAnimatedSpriteController3D::set_animation_pixel_offset);
	ClassDB::bind_method(D_METHOD("get_animation_pixel_offset", "anim_name"), &DirectionalAnimatedSpriteController3D::get_animation_pixel_offset);
	ClassDB::bind_method(D_METHOD("set_animation_notes", "anim_name", "notes"), &DirectionalAnimatedSpriteController3D::set_animation_notes);
	ClassDB::bind_method(D_METHOD("get_animation_notes", "anim_name"), &DirectionalAnimatedSpriteController3D::get_animation_notes);
	ClassDB::bind_method(D_METHOD("set_animation_direction_count", "anim_name", "dir_count"), &DirectionalAnimatedSpriteController3D::set_animation_direction_count);
	ClassDB::bind_method(D_METHOD("get_animation_direction_count", "anim_name"), &DirectionalAnimatedSpriteController3D::get_animation_direction_count);

	ClassDB::bind_method(D_METHOD("set_animation_direction_animation_name", "anim_name", "dir_idx", "sprite_frames_anim_name"), &DirectionalAnimatedSpriteController3D::set_animation_direction_animation_name);
	ClassDB::bind_method(D_METHOD("get_animation_direction_animation_name", "anim_name", "dir_idx"), &DirectionalAnimatedSpriteController3D::get_animation_direction_animation_name);
	ClassDB::bind_method(D_METHOD("set_animation_direction_angle", "anim_name", "dir_idx", "angle_rad"), &DirectionalAnimatedSpriteController3D::set_animation_direction_angle);
	ClassDB::bind_method(D_METHOD("get_animation_direction_angle", "anim_name", "dir_idx"), &DirectionalAnimatedSpriteController3D::get_animation_direction_angle);
	ClassDB::bind_method(D_METHOD("set_animation_direction_pixel_offset", "anim_name", "dir_idx", "ofs"), &DirectionalAnimatedSpriteController3D::set_animation_direction_pixel_offset);
	ClassDB::bind_method(D_METHOD("get_animation_direction_pixel_offset", "anim_name", "dir_idx"), &DirectionalAnimatedSpriteController3D::get_animation_direction_pixel_offset);
	ClassDB::bind_method(D_METHOD("set_animation_direction_flip_h", "anim_name", "dir_idx", "flip"), &DirectionalAnimatedSpriteController3D::set_animation_direction_flip_h);
	ClassDB::bind_method(D_METHOD("is_animation_direction_flipped_h", "anim_name", "dir_idx"), &DirectionalAnimatedSpriteController3D::is_animation_direction_flipped_h);
	ClassDB::bind_method(D_METHOD("set_animation_direction_flip_v", "anim_name", "dir_idx", "flip"), &DirectionalAnimatedSpriteController3D::set_animation_direction_flip_v);
	ClassDB::bind_method(D_METHOD("is_animation_direction_flipped_v", "anim_name", "dir_idx"), &DirectionalAnimatedSpriteController3D::is_animation_direction_flipped_v);
	ClassDB::bind_method(D_METHOD("set_animation_direction_notes", "anim_name", "dir_idx", "notes"), &DirectionalAnimatedSpriteController3D::set_animation_direction_notes);
	ClassDB::bind_method(D_METHOD("get_animation_direction_notes", "anim_name", "dir_idx"), &DirectionalAnimatedSpriteController3D::get_animation_direction_notes);

	ClassDB::bind_method(D_METHOD("rename_animation", "from", "to"), &DirectionalAnimatedSpriteController3D::rename_animation);
	ClassDB::bind_method(D_METHOD("has_animation", "anim_name"), &DirectionalAnimatedSpriteController3D::has_animation);
	ClassDB::bind_method(D_METHOD("has_animation_and_direction", "anim_name", "dir_idx"), &DirectionalAnimatedSpriteController3D::has_animation_and_direction);
	ClassDB::bind_method(D_METHOD("can_add_animation", "anim_name"), &DirectionalAnimatedSpriteController3D::can_add_animation);
	ClassDB::bind_method(D_METHOD("add_animation", "anim_name"), &DirectionalAnimatedSpriteController3D::add_animation);
	ClassDB::bind_method(D_METHOD("remove_animation", "anim_name"), &DirectionalAnimatedSpriteController3D::remove_animation);
	ClassDB::bind_method(D_METHOD("get_animation_names"), &DirectionalAnimatedSpriteController3D::get_animation_names);
	ClassDB::bind_method(D_METHOD("get_animation_count"), &DirectionalAnimatedSpriteController3D::get_animation_count);

	ClassDB::bind_method(D_METHOD("play", "anim_name", "custom_speed"), &DirectionalAnimatedSpriteController3D::play, DEFVAL(1.0));

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "disabled"), "set_disabled", "is_disabled");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "sprite", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "AnimatedSprite3D"), "set_sprite", "get_sprite");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "billboard_mode", PROPERTY_HINT_ENUM, "Ignore,Mirror Camera Angle,Look at Camera"), "set_billboard_mode", "get_billboard_mode");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "custom_camera", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Camera3D", PROPERTY_USAGE_NONE), "set_custom_camera", "get_custom_camera");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_custom_angle"), "set_use_custom_angle", "is_using_custom_angle");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "custom_angle", PROPERTY_HINT_RANGE, "-360,360,0.1,radians_as_degrees"), "set_custom_angle", "get_custom_angle");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "animation"), "set_animation", "get_animation");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "pixel_anchor_mode", PROPERTY_HINT_ENUM, "Disabled,Centered,Bottom-Middle"), "set_pixel_anchor_mode", "get_pixel_anchor_mode");

	ADD_SIGNAL(MethodInfo("animation_changed"));
	ADD_SIGNAL(MethodInfo("animation_added", PropertyInfo(Variant::STRING_NAME, "anim_name")));
	ADD_SIGNAL(MethodInfo("removing_animation", PropertyInfo(Variant::STRING_NAME, "anim_name")));
	ADD_SIGNAL(MethodInfo("animation_removed", PropertyInfo(Variant::STRING_NAME, "anim_name")));
}


DirectionalAnimatedSpriteController3D::DirectionalAnimatedSpriteController3D()
{
	update_process();
}
