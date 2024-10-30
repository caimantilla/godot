#include "uniform_art_collection.h"
#include "core/io/image.h"


void UniformArtCollection::set_art_entries(const Vector<Ref<UniformArtData>> &p_entries)
{
	art_entries = p_entries;
}


Vector<Ref<UniformArtData>> UniformArtCollection::get_art_entries() const
{
	return art_entries;
}


void UniformArtCollection::set_process_target_count(const int p_count)
{
	ERR_FAIL_COND(p_count < 0);
	process_targets.resize(p_count);
	emit_changed();
	notify_property_list_changed();
}


int UniformArtCollection::get_process_target_count() const
{
	return process_targets.size();
}


void UniformArtCollection::set_process_target_interpolation(const int p_target, const Image::Interpolation p_interpolation)
{
	ERR_FAIL_COND(!has_process_target(p_target));
	ProcessTarget &target = get_process_target(p_target);
	if (target.interpolation == p_interpolation)
	{
		return;
	}
	target.interpolation = p_interpolation;
	emit_changed();
}


Image::Interpolation UniformArtCollection::get_process_target_interpolation(const int p_target) const
{
	ERR_FAIL_COND_V(!has_process_target(p_target), Image::INTERPOLATE_BILINEAR);
	return get_process_target(p_target).interpolation;
}


void UniformArtCollection::set_process_target_directory_images_destination(const int p_target, const String &p_path)
{
	ERR_FAIL_COND(!has_process_target(p_target));
	ProcessTarget &target = get_process_target(p_target);
	if (target.directory_images_destination == p_path)
	{
		return;
	}
	target.directory_images_destination = p_path;
	emit_changed();
}


String UniformArtCollection::get_process_target_directory_images_destination(const int p_target) const
{
	ERR_FAIL_COND_V(!has_process_target(p_target), String());
	return get_process_target(p_target).directory_images_destination;
}


void UniformArtCollection::set_process_target_directory_crop_textures_destination(const int p_target, const String &p_path)
{
	ERR_FAIL_COND(!has_process_target(p_target));
	ProcessTarget &target = get_process_target(p_target);
	if (target.directory_crop_textures_destination == p_path)
	{
		return;
	}
	target.directory_crop_textures_destination = p_path;
	emit_changed();
}


String UniformArtCollection::get_process_target_directory_crop_textures_destination(const int p_target) const
{
	ERR_FAIL_COND_V(!has_process_target(p_target), String());
	return get_process_target(p_target).directory_crop_textures_destination;
}


void UniformArtCollection::set_process_target_size(const int p_target, const Size2i &p_size)
{
	ERR_FAIL_COND(!has_process_target(p_target));
	ERR_FAIL_COND_MSG(p_size.width < 0 || p_size.height < 0, TTR("Size cannot be negative."));
	ERR_FAIL_COND_MSG(p_size.width > Image::MAX_WIDTH || p_size.height > Image::MAX_HEIGHT, vformat(TTR("Size cannot exceed %dx%d."), Image::MAX_WIDTH, Image::MAX_HEIGHT));

	ProcessTarget &target = get_process_target(p_target);
	if (target.size == p_size)
	{
		return;
	}
	target.size = p_size;
	emit_changed();
}


Size2i UniformArtCollection::get_process_target_size(const int p_target) const
{
	ERR_FAIL_COND_V(!has_process_target(p_target), Size2i());
	return get_process_target(p_target).size;
}


void UniformArtCollection::set_process_target_padded_size(const int p_target, const Size2i &p_size)
{
	ERR_FAIL_COND(!has_process_target(p_target));
	ERR_FAIL_COND_MSG(p_size.width < 0 || p_size.height < 0, TTR("Size cannot be negative."));
	ERR_FAIL_COND_MSG(p_size.width > Image::MAX_WIDTH || p_size.height > Image::MAX_HEIGHT, vformat(TTR("Size cannot exceed %dx%d."), Image::MAX_WIDTH, Image::MAX_HEIGHT));

	ProcessTarget &target = get_process_target(p_target);
	if (target.padded_size == p_size)
	{
		return;
	}
	target.padded_size = p_size;
	emit_changed();
}


Size2i UniformArtCollection::get_process_target_padded_size(const int p_target) const
{
	ERR_FAIL_COND_V(!has_process_target(p_target), Size2i());
	return get_process_target(p_target).padded_size;
}


void UniformArtCollection::set_process_target_image_path_background(const int p_target, const String &p_path)
{
	ERR_FAIL_COND(!has_process_target(p_target));
	ProcessTarget &target = get_process_target(p_target);
	if (target.image_path_background == p_path)
	{
		return;
	}
	target.image_path_background = p_path;
	emit_changed();
}


String UniformArtCollection::get_process_target_image_path_background(const int p_target) const
{
	ERR_FAIL_COND_V(!has_process_target(p_target), String());
	return get_process_target(p_target).image_path_background;
}


void UniformArtCollection::set_process_target_image_path_foreground(const int p_target, const String &p_path)
{
	ERR_FAIL_COND(!has_process_target(p_target));
	ProcessTarget &target = get_process_target(p_target);
	if (target.image_path_foreground == p_path)
	{
		return;
	}
	target.image_path_foreground = p_path;
	emit_changed();
}


String UniformArtCollection::get_process_target_image_path_foreground(const int p_target) const
{
	ERR_FAIL_COND_V(!has_process_target(p_target), String());
	return get_process_target(p_target).image_path_foreground;
}


void UniformArtCollection::set_process_target_image_path_mask(const int p_target, const String &p_path)
{
	ERR_FAIL_COND(!has_process_target(p_target));
	ProcessTarget &target = get_process_target(p_target);
	if (target.image_path_mask == p_path)
	{
		return;
	}
	target.image_path_mask = p_path;
	emit_changed();
}


String UniformArtCollection::get_process_target_image_path_mask(const int p_target) const
{
	ERR_FAIL_COND_V(!has_process_target(p_target), String());
	return get_process_target(p_target).image_path_mask;
}


bool UniformArtCollection::has_process_target(const int p_target) const
{
	return p_target > -1 && p_target < process_targets.size();
}


UniformArtCollection::ProcessTarget &UniformArtCollection::get_process_target(const int p_target)
{
	return process_targets[p_target];
}


const UniformArtCollection::ProcessTarget &UniformArtCollection::get_process_target(const int p_target) const
{
	return process_targets[p_target];
}


void UniformArtCollection::bindable_set_art_entries(const TypedArray<UniformArtData> &p_entries)
{
	art_entries.resize(p_entries.size());

	for (int i = 0; i < p_entries.size(); i++)
	{
		art_entries.write[i] = p_entries[i];
	}
}


TypedArray<UniformArtData> UniformArtCollection::bindable_get_art_entries() const
{
	TypedArray<UniformArtData> ret;
	ret.resize(art_entries.size());
	for (int i = 0; i < art_entries.size(); i++)
	{
		ret.set(i, art_entries[i]);
	}
	return ret;
}


void UniformArtCollection::_get_property_list(List<PropertyInfo> *p_list) const
{
	p_list_helper_process_targets.get_property_list(p_list);
}



bool UniformArtCollection::_set(const StringName &p_name, const Variant &p_value)
{
	return p_list_helper_process_targets.property_set_value(p_name, p_value);
}


bool UniformArtCollection::_get(const StringName &p_name, Variant &r_ret) const
{
	return p_list_helper_process_targets.property_get_value(p_name, r_ret);
}


bool UniformArtCollection::_property_can_revert(const StringName &p_name) const
{
	return p_list_helper_process_targets.property_can_revert(p_name);
}


bool UniformArtCollection::_property_get_revert(const StringName &p_name, Variant &r_property) const
{
	return p_list_helper_process_targets.property_get_revert(p_name, r_property);
}


void UniformArtCollection::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_art_entries", "entry_list"), &UniformArtCollection::bindable_set_art_entries);
	ClassDB::bind_method(D_METHOD("get_art_entries"), &UniformArtCollection::bindable_get_art_entries);

	ClassDB::bind_method(D_METHOD("set_process_target_count", "count"), &UniformArtCollection::set_process_target_count);
	ClassDB::bind_method(D_METHOD("get_process_target_count"), &UniformArtCollection::get_process_target_count);
	ClassDB::bind_method(D_METHOD("set_process_target_interpolation", "target", "interpolation_mode"), &UniformArtCollection::set_process_target_interpolation);
	ClassDB::bind_method(D_METHOD("get_process_target_interpolation", "target"), &UniformArtCollection::get_process_target_interpolation);
	ClassDB::bind_method(D_METHOD("set_process_target_directory_images_destination", "target", "path"), &UniformArtCollection::set_process_target_directory_images_destination);
	ClassDB::bind_method(D_METHOD("get_process_target_directory_images_destination", "target"), &UniformArtCollection::get_process_target_directory_images_destination);
	ClassDB::bind_method(D_METHOD("set_process_target_directory_crop_textures_destination", "target", "path"), &UniformArtCollection::set_process_target_directory_crop_textures_destination);
	ClassDB::bind_method(D_METHOD("get_process_target_directory_crop_textures_destination", "target"), &UniformArtCollection::get_process_target_directory_crop_textures_destination);
	ClassDB::bind_method(D_METHOD("set_process_target_size", "target", "size"), &UniformArtCollection::set_process_target_size);
	ClassDB::bind_method(D_METHOD("get_process_target_size", "target"), &UniformArtCollection::get_process_target_size);
	ClassDB::bind_method(D_METHOD("set_process_target_padded_size", "target", "size"), &UniformArtCollection::set_process_target_padded_size);
	ClassDB::bind_method(D_METHOD("get_process_target_padded_size", "target"), &UniformArtCollection::get_process_target_padded_size);
	ClassDB::bind_method(D_METHOD("set_process_target_image_path_background", "target", "path"), &UniformArtCollection::set_process_target_image_path_background);
	ClassDB::bind_method(D_METHOD("get_process_target_image_path_background", "target"), &UniformArtCollection::get_process_target_image_path_background);
	ClassDB::bind_method(D_METHOD("set_process_target_image_path_foreground", "target", "path"), &UniformArtCollection::set_process_target_image_path_foreground);
	ClassDB::bind_method(D_METHOD("get_process_target_image_path_foreground", "target"), &UniformArtCollection::get_process_target_image_path_foreground);
	ClassDB::bind_method(D_METHOD("set_process_target_image_path_mask", "target", "path"), &UniformArtCollection::set_process_target_image_path_mask);
	ClassDB::bind_method(D_METHOD("get_process_target_image_path_mask", "target"), &UniformArtCollection::get_process_target_image_path_mask);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "art_entries", PROPERTY_HINT_ARRAY_TYPE, MAKE_RESOURCE_TYPE_HINT("UniformArtData")), "set_art_entries", "get_art_entries");
	ADD_ARRAY_COUNT("Processing Targets", "process_target_count", "set_process_target_count", "get_process_target_count", "process_targets/");

	ProcessTarget process_target_defaults;
	base_p_list_helper_process_targets.set_prefix("process_targets/");
	base_p_list_helper_process_targets.set_array_length_getter(&UniformArtCollection::get_process_target_count);
	base_p_list_helper_process_targets.register_property(PropertyInfo(Variant::INT, "interpolation", PROPERTY_HINT_ENUM, "Nearest,Bilinear,Cubic,Trilinear,Lanczos"), process_target_defaults.interpolation, &UniformArtCollection::set_process_target_interpolation, &UniformArtCollection::get_process_target_interpolation);
	base_p_list_helper_process_targets.register_property(PropertyInfo(Variant::STRING, "directory_images_destination", PROPERTY_HINT_DIR), process_target_defaults.directory_images_destination, &UniformArtCollection::set_process_target_directory_images_destination, &UniformArtCollection::get_process_target_directory_images_destination);
	base_p_list_helper_process_targets.register_property(PropertyInfo(Variant::STRING, "directory_crop_textures_destination", PROPERTY_HINT_DIR), process_target_defaults.directory_crop_textures_destination, &UniformArtCollection::set_process_target_directory_crop_textures_destination, &UniformArtCollection::get_process_target_directory_crop_textures_destination);
	base_p_list_helper_process_targets.register_property(PropertyInfo(Variant::VECTOR2I, "size", PROPERTY_HINT_NONE, "suffix:px"), process_target_defaults.size, &UniformArtCollection::set_process_target_size, &UniformArtCollection::get_process_target_size);
	base_p_list_helper_process_targets.register_property(PropertyInfo(Variant::VECTOR2I, "padded_size", PROPERTY_HINT_NONE, "suffix:px"), process_target_defaults.padded_size, &UniformArtCollection::set_process_target_padded_size, &UniformArtCollection::get_process_target_padded_size);
	base_p_list_helper_process_targets.register_property(PropertyInfo(Variant::STRING, "image_path_background", PROPERTY_HINT_FILE, "*.png"), process_target_defaults.image_path_background, &UniformArtCollection::set_process_target_image_path_background, &UniformArtCollection::get_process_target_image_path_background);
	base_p_list_helper_process_targets.register_property(PropertyInfo(Variant::STRING, "image_path_foreground", PROPERTY_HINT_FILE, "*.png"), process_target_defaults.image_path_foreground, &UniformArtCollection::set_process_target_image_path_foreground, &UniformArtCollection::get_process_target_image_path_foreground);
	base_p_list_helper_process_targets.register_property(PropertyInfo(Variant::STRING, "image_path_mask", PROPERTY_HINT_FILE, "*.png"), process_target_defaults.image_path_mask, &UniformArtCollection::set_process_target_image_path_mask, &UniformArtCollection::get_process_target_image_path_mask);
	PropertyListHelper::register_base_helper(&base_p_list_helper_process_targets);
}


UniformArtCollection::UniformArtCollection()
{
	p_list_helper_process_targets.setup_for_instance(base_p_list_helper_process_targets, this);
}
