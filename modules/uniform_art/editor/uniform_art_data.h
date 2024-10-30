#ifndef UNIFORM_ART_DATA_H
#define UNIFORM_ART_DATA_H


#include "core/io/resource.h"


class UniformArtData final : public Resource
{
	GDCLASS(UniformArtData, Resource);

private:
	String source_image_path;
	Rect2i crop_region;
	bool flip_h = false;
	bool flip_v = false;

protected:
	static void _bind_methods();

public:
	void set_source_image_path(const String &p_path);
	String get_source_image_path() const;
	void set_crop_region(const Rect2i &p_rect);
	Rect2i get_crop_region() const;
	void set_flip_h(bool p_enabled);
	bool is_flipped_h() const;
	void set_flip_v(bool p_enabled);
	bool is_flipped_v() const;
};


#endif // UNIFORM_ART_DATA_H
