#ifndef UNIFORM_ART_COLLECTION_H
#define UNIFORM_ART_COLLECTION_H


#include "uniform_art_data.h"
#include "core/io/resource.h"
#include "core/io/image.h"
#include "core/variant/typed_array.h"
#include "scene/property_list_helper.h"


class UniformArtCollection final : public Resource
{
	GDCLASS(UniformArtCollection, Resource);

public:
	struct ScanTarget final
	{
		String name; // This is only used for organizational purposes.
		String scan_directory; // The directory that will be scanned.
		String save_subdirectory; // The subfolder that images from this folder will be saved to.
	};

	struct ProcessTarget final
	{
		String name; // This is only for organizational purposes.
		Image::Interpolation interpolation = Image::INTERPOLATE_BILINEAR;
		String directory_images_destination;
		String directory_crop_textures_destination;
		Size2i size;
		Size2i padded_size;
		String image_path_background;
		String image_path_foreground;
		String image_path_mask;
	};

private:
	static inline PropertyListHelper base_p_list_helper_scan_targets;
	PropertyListHelper p_list_helper_scan_targets;
	static inline PropertyListHelper base_p_list_helper_process_targets;
	PropertyListHelper p_list_helper_process_targets;

	LocalVector<ScanTarget, int> scan_targets;
	LocalVector<ProcessTarget, int> process_targets;

protected:
	static void _bind_methods();
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	bool _property_can_revert(const StringName &p_name) const;
	bool _property_get_revert(const StringName &p_name, Variant &r_property) const;

public:
	void set_scan_target_count(const int p_count);
	int get_scan_target_count() const;
	void set_scan_target_name(const int p_target, const String &p_name);
	String get_scan_target_name(const int p_target) const;
	void set_scan_target_scan_directory(const int p_target, const String &p_folder);
	String get_scan_target_scan_directory(const int p_target) const;
	void set_scan_target_save_subdirectory(const int p_target, const String &p_subdirectory_name);
	String get_scan_target_save_subdirectory(const int p_target) const;

	void set_process_target_count(const int p_count);
	int get_process_target_count() const;
	void set_process_target_name(const int p_target, const String &p_name);
	String get_process_target_name(const int p_target) const;
	void set_process_target_interpolation(const int p_target, const Image::Interpolation p_interpolation_mode);
	Image::Interpolation get_process_target_interpolation(const int p_target) const;
	void set_process_target_directory_images_destination(const int p_target, const String &p_path);
	String get_process_target_directory_images_destination(const int p_target) const;
	void set_process_target_directory_crop_textures_destination(const int p_target, const String &p_path);
	String get_process_target_directory_crop_textures_destination(const int p_target) const;
	void set_process_target_size(const int p_target, const Size2i &p_size);
	Size2i get_process_target_size(const int p_target) const;
	void set_process_target_padded_size(const int p_target, const Size2i &p_size);
	Size2i get_process_target_padded_size(const int p_target) const;
	void set_process_target_image_path_background(const int p_target, const String &p_path);
	String get_process_target_image_path_background(const int p_target) const;
	void set_process_target_image_path_foreground(const int p_target, const String &p_path);
	String get_process_target_image_path_foreground(const int p_target) const;
	void set_process_target_image_path_mask(const int p_target, const String &p_path);
	String get_process_target_image_path_mask(const int p_target) const;

	bool has_scan_target(const int p_target) const;
	ScanTarget &get_scan_target(const int p_target);
	const ScanTarget &get_scan_target(const int p_target) const;

	bool has_process_target(const int p_target) const;
	ProcessTarget &get_process_target(const int p_target);
	const ProcessTarget &get_process_target(const int p_target) const;

	UniformArtCollection();
};


#endif // UNIFORM_ART_COLLECTION_H
