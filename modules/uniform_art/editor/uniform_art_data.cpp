#include "uniform_art_data.h"


void UniformArtData::set_source_image_path(const String &p_path)
{
	if (p_path == source_image_path)
	{
		return;	
	}
	source_image_path = p_path;
	emit_changed();
}


String UniformArtData::get_source_image_path() const
{
	return source_image_path;
}


void UniformArtData::set_crop_region(const Rect2i &p_rect)
{
	ERR_FAIL_COND_MSG(p_rect.size.width < 0 || p_rect.size.height < 0, TTR("Size cannot be negative."));
	if (p_rect == crop_region)
	{
		return;
	}
	crop_region = p_rect;
	emit_changed();
}


Rect2i UniformArtData::get_crop_region() const
{
	return crop_region;
}


void UniformArtData::set_flip_h(bool p_enabled)
{
	if (p_enabled == flip_h)
	{
		return;
	}
	flip_h = p_enabled;
	emit_changed();
}


bool UniformArtData::is_flipped_h() const
{
	return flip_h;
}


void UniformArtData::set_flip_v(bool p_enabled)
{
	if (p_enabled == flip_v)
	{
		return;
	}
	flip_v = p_enabled;
	emit_changed();
}


bool UniformArtData::is_flipped_v() const
{
	return flip_v;
}


void UniformArtData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_source_image_path", "path"), &UniformArtData::set_source_image_path);
	ClassDB::bind_method(D_METHOD("get_source_image_path"), &UniformArtData::get_source_image_path);
	ClassDB::bind_method(D_METHOD("set_crop_region", "region"), &UniformArtData::set_crop_region);
	ClassDB::bind_method(D_METHOD("get_crop_region"), &UniformArtData::get_crop_region);
	ClassDB::bind_method(D_METHOD("set_flip_h", "enabled"), &UniformArtData::set_flip_h);
	ClassDB::bind_method(D_METHOD("is_flipped_h"), &UniformArtData::is_flipped_h);
	ClassDB::bind_method(D_METHOD("set_flip_v", "enabled"), &UniformArtData::set_flip_v);
	ClassDB::bind_method(D_METHOD("is_flipped_v"), &UniformArtData::is_flipped_v);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "source_image_path", PROPERTY_HINT_FILE, "*.png"), "set_source_image_path", "get_source_image_path");
	ADD_PROPERTY(PropertyInfo(Variant::RECT2I, "crop_region"), "set_crop_region", "get_crop_region");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flip_h"), "set_flip_h", "is_flipped_h");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flip_v"), "set_flip_v", "is_flipped_v");
}
