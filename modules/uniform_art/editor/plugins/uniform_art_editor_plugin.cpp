#include "uniform_art_editor_plugin.h"
#include "core/io/dir_access.h"
#include "scene/resources/image_texture.h"
#include "scene/gui/button.h"
#include "scene/gui/texture_rect.h"
#include "editor/editor_file_system.h"
#include "editor/themes/editor_scale.h"


static Ref<Image> load_image_at_path(const String &p_path)
{
	const String path = p_path.strip_edges();
	if (path.is_empty())
	{
		return Ref<Image>();
	}
	Engine *engine = Engine::get_singleton();
	bool was_error_enabled = engine->is_printing_error_messages();
	engine->set_print_error_messages(false);

	Ref<Image> image;
	image.instantiate();
	Error load_error = image->load(path);
	if (load_error == OK)
	{
		image->convert(Image::FORMAT_RGBA8);
	}
	else
	{
		image = Ref<Image>();
	}
	engine->set_print_error_messages(was_error_enabled);
	return image;
	// ERR_FAIL_COND_V_MSG(!p_path.ends_with(".png"), Ref<Image>(), TTR("Only PNG images are supported."));
	// Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::READ);
	// if (file.is_valid())
	// {
	// 	PackedByteArray data = file->get_buffer(file->get_length());
	// 	file->close();

	// 	Ref<Image> image;
	// 	image.instantiate();
	// 	if (image->load_png_from_buffer(data) == OK)
	// 	{
	// 		image->convert(Image::FORMAT_RGBA8);
	// 		return image;
	// 	}
	// }
	// return Ref<Image>();
}


void UniformArtDataPreview::set_art_data(const Ref<UniformArtData> &p_art)
{
	if (p_art == art_data)
	{
		return;
	}
	if (art_data.is_valid())
	{
		art_data->disconnect_changed(callable_mp(this, &UniformArtDataPreview::on_art_data_changed));
	}
	art_data = p_art;
	if (art_data.is_valid())
	{
		art_data->connect_changed(callable_mp(this, &UniformArtDataPreview::on_art_data_changed));
	}
	reload_image();
}


Ref<UniformArtData> UniformArtDataPreview::get_art_data() const
{
	return art_data;
}


void UniformArtDataPreview::update_texture_display()
{
	if (!art_data.is_valid())
	{
		return;
	}

	texture_display->set_flip_h(art_data->is_flipped_h());
	texture_display->set_flip_v(art_data->is_flipped_v());

	Rect2i crop_region = art_data->get_crop_region();

	if (crop_region.size.width < 1 || crop_region.size.height < 1)
	{
		if (atlas_texture->get_atlas().is_valid())
		{
			atlas_texture->set_region(Rect2i(Point2i(0, 0), atlas_texture->get_atlas()->get_size()));
		}
		else
		{
			atlas_texture->set_region(Rect2i(0, 0, 1, 1));
		}
	}
	else
	{
		atlas_texture->set_region(crop_region);
	}
}


void UniformArtDataPreview::reload_image()
{
	atlas_texture->set_atlas(Ref<Texture2D>());
	cache_image_path = String();

	if (!art_data.is_valid())
	{
		return;
	}

	const String path = art_data->get_source_image_path();
	Ref<Image> image = load_image_at_path(path);
	if (!image.is_valid())
	{
		return;
	}
	cache_image_path = path;

	Ref<ImageTexture> tex = ImageTexture::create_from_image(image);
	atlas_texture->set_atlas(tex);

	update_texture_display();
}


void UniformArtDataPreview::on_art_data_changed()
{
	const String new_image_path = art_data.is_valid() ? art_data->get_source_image_path() : String();
	if (new_image_path != cache_image_path)
	{
		reload_image();
	}
	else
	{
		update_texture_display();
	}
}


void UniformArtDataPreview::_notification(int p_what)
{
	if (p_what == NOTIFICATION_THEME_CHANGED)
	{
		checkerboard->set_texture(get_editor_theme_icon(SNAME("Checkerboard")));
		reload_image();
	}
}


UniformArtDataPreview::UniformArtDataPreview()
{
	checkerboard = memnew(TextureRect);
	checkerboard->set_stretch_mode(TextureRect::STRETCH_TILE);
	checkerboard->set_texture_repeat(CanvasItem::TEXTURE_REPEAT_ENABLED);
	checkerboard->set_custom_minimum_size(Size2(32, 256) * EDSCALE);
	add_child(checkerboard);

	atlas_texture.instantiate();

	texture_display = memnew(TextureRect);
	texture_display->set_texture(atlas_texture);
	texture_display->set_expand_mode(TextureRect::EXPAND_IGNORE_SIZE);
	texture_display->set_stretch_mode(TextureRect::STRETCH_KEEP_ASPECT_CENTERED);
	texture_display->set_texture_filter(CanvasItem::TEXTURE_FILTER_NEAREST_WITH_MIPMAPS);
	add_child(texture_display);
}


bool EditorInspectorPluginUniformArtCollection::can_handle(Object *p_object)
{
	return Object::cast_to<UniformArtCollection>(p_object) != nullptr;
}


void EditorInspectorPluginUniformArtCollection::parse_begin(Object *p_object)
{
	Ref<UniformArtCollection> collection = Object::cast_to<UniformArtCollection>(p_object);
	if (collection != nullptr)
	{
		Button *button = memnew(Button);
		button->set_text(TTR("Rebuild Collection"));
		button->set_tooltip_text(TTR("Process and save new copies of each of the collection's textures to disk."));
		button->connect(SceneStringName(pressed), callable_mp(this, &EditorInspectorPluginUniformArtCollection::request_rebuild).bind(collection));
		button->connect(SceneStringName(theme_changed), callable_mp(this, &EditorInspectorPluginUniformArtCollection::on_rebuild_button_theme_changed).bind(button));
		add_custom_control(button);
	}
}


void EditorInspectorPluginUniformArtCollection::on_rebuild_button_theme_changed(Button *p_button)
{
	p_button->set_icon(p_button->get_editor_theme_icon(SNAME("Reload")));
}


void EditorInspectorPluginUniformArtCollection::request_rebuild(Ref<UniformArtCollection> p_collection)
{
	emit_signal(SNAME("rebuild_requested"), p_collection);
}


void EditorInspectorPluginUniformArtCollection::_bind_methods()
{
	ADD_SIGNAL(MethodInfo("rebuild_requested", PropertyInfo(Variant::OBJECT, "collection", PROPERTY_HINT_RESOURCE_TYPE, "UniformArtCollection", PROPERTY_USAGE_DEFAULT, "UniformArtCollection")));
}


bool EditorInspectorPluginUniformArtData::can_handle(Object *p_object)
{
	return Object::cast_to<UniformArtData>(p_object) != nullptr;
}


void EditorInspectorPluginUniformArtData::parse_begin(Object *p_object)
{
	Ref<UniformArtData> art_data = Object::cast_to<UniformArtData>(p_object);
	if (art_data.is_valid())
	{
		UniformArtDataPreview *preview = memnew(UniformArtDataPreview);
		preview->set_art_data(art_data);
		add_custom_control(preview);
	}
}


void UniformArtEditorPlugin::rebuild_collection(const Ref<UniformArtCollection> &p_collection)
{
	struct IterArt final
	{
		Ref<UniformArtData> art; // Reference to the art data file.
		String subfolder_name; // The name of the subfolder that the resulting image will be saved to, deeper than the main save directory.
	};

	ERR_FAIL_COND(!p_collection.is_valid());
	ERR_FAIL_COND_EDMSG(p_collection->get_process_target_count() < 1, TTR("Collection must have at least 1 process target to be rebuilt."));

	// if (!cache_unchanged_art_entry_map.has(p_collection))
	// {
	// 	cache_unchanged_art_entry_map.insert(p_collection, HashSet<Ref<UniformArtData>>());
	// }

	// HashSet<Ref<UniformArtData>> &cache_unchanged_entries = cache_unchanged_art_entry_map[p_collection];
	List<QueueGenerateCropTexture> crop_texture_generate_queue;

	List<IterArt> art_list;

	for (int i = 0; i < p_collection->get_scan_target_count(); i++)
	{
		const UniformArtCollection::ScanTarget scan_target = p_collection->get_scan_target(i);

		Error err = OK;
		Ref<DirAccess> dir = DirAccess::open(scan_target.scan_directory, &err);
		ERR_CONTINUE_EDMSG(err != OK, vformat(TTR("Failed to open directory '%s' with error code %d."), scan_target.scan_directory, err));

		err = dir->list_dir_begin();
		if (err != OK)
		{
			dir->list_dir_end();
			continue;
		}
		String current_file = dir->get_next();
		while (!current_file.is_empty())
		{
			const String path = scan_target.scan_directory.path_join(current_file);
			if (!dir->current_is_dir() && ResourceLoader::exists(path, "UniformArtData"))
			{
				Ref<UniformArtData> art = ResourceLoader::load(path, "UniformArtData");
				if (art.is_valid())
				{
					IterArt iter_art;
					iter_art.art = art;
					iter_art.subfolder_name = scan_target.save_subdirectory;
					art_list.push_back(iter_art);
				}
			}
			current_file = dir->get_next();
		}
		dir->list_dir_end();
	}

	ERR_FAIL_COND_EDMSG(art_list.is_empty(), TTR("Collection must have at least 1 art entry to be rebuilt."));

	for (int target_idx = 0; target_idx < p_collection->get_process_target_count(); target_idx++)
	{
		const UniformArtCollection::ProcessTarget target = p_collection->get_process_target(target_idx);

		String directory_images_destination = target.directory_images_destination.strip_edges();
		String directory_crop_textures_destination = target.directory_crop_textures_destination.strip_edges();

		bool crop_texture_generate_enabled = (
			DirAccess::dir_exists_absolute(directory_crop_textures_destination)
			&& target.size.width > 0
			&& target.size.height > 0
			&& target.padded_size.width > 0
			&& target.padded_size.height > 0
		);

		ERR_CONTINUE(directory_images_destination.is_empty());

		if (directory_images_destination.is_relative_path())
		{
			directory_images_destination = p_collection->get_path().get_base_dir().path_join(directory_images_destination);
		}

		bool save_directories_ok = DirAccess::dir_exists_absolute(directory_images_destination);
		for (int i = 0; i < p_collection->get_scan_target_count(); i++)
		{
			const String save_subfolder = p_collection->get_scan_target_save_subdirectory(i);
			if (!save_subfolder.is_empty())
			{
				const String save_dir_img = directory_images_destination.path_join(save_subfolder);
				const String save_dir_crop = directory_crop_textures_destination.path_join(save_subfolder);
				if (!DirAccess::dir_exists_absolute(save_dir_img))
				{
					save_directories_ok = false;
					break;
				}
				crop_texture_generate_enabled = crop_texture_generate_enabled && DirAccess::dir_exists_absolute(save_dir_crop);
			}
		}
		ERR_CONTINUE(!save_directories_ok);

		if (!directory_crop_textures_destination.is_empty() && directory_crop_textures_destination.is_relative_path())
		{
			directory_crop_textures_destination = p_collection->get_path().get_base_dir().path_join(directory_crop_textures_destination);
		}

		HashMap<Size2i, Ref<Image>> size_bg_map;
		HashMap<Size2i, Ref<Image>> size_fg_map;
		HashMap<Size2i, Ref<Image>> size_mask_map;

		Ref<Image> base_bg_img = load_image_at_path(target.image_path_background);
		Ref<Image> base_fg_img = load_image_at_path(target.image_path_foreground);
		Ref<Image> base_mask_img = load_image_at_path(target.image_path_mask);

		if (base_bg_img.is_valid())
		{
			size_bg_map.insert(base_bg_img->get_size(), base_bg_img);
		}
		if (base_fg_img.is_valid())
		{
			size_fg_map.insert(base_fg_img->get_size(), base_fg_img);
		}
		if (base_mask_img.is_valid())
		{
			size_mask_map.insert(base_mask_img->get_size(), base_mask_img);
		}

		for (const IterArt &iter_art : art_list)
		{
			const Ref<UniformArtData> art_data = iter_art.art;
			ERR_CONTINUE_MSG(art_data->is_built_in(), TTR("Subresources cannot be used as art entries.\nPlease save to external files."));

			const String filename = art_data->get_path().get_file().get_basename();
			const String save_path_image = (
				iter_art.subfolder_name.is_empty()
				? directory_images_destination.path_join(vformat("%s.png", filename))
				: directory_images_destination.path_join(iter_art.subfolder_name).path_join(vformat("%s.png", filename))
			);
			const String save_path_crop_texture = (
				iter_art.subfolder_name.is_empty()
				? directory_crop_textures_destination.path_join(vformat("%s.tres", filename))
				: directory_crop_textures_destination.path_join(iter_art.subfolder_name).path_join(vformat("%s.tres", filename))
			);

			Ref<Image> source_art_img = load_image_at_path(art_data->get_source_image_path());
			ERR_CONTINUE_MSG(!source_art_img.is_valid(), vformat(TTR("No image found at at path: \"%s\""), art_data->get_source_image_path()));

			Size2i entry_size = target.size;
			if (entry_size.width < 1 || entry_size.height < 1)
			{
				entry_size = art_data->get_crop_region().size;
			}
			if (entry_size.width < 1 || entry_size.height < 1)
			{
				entry_size = source_art_img->get_size();
			}

			double art_crop_scale;
			Rect2i art_crop_region = art_data->get_crop_region();
			if (art_crop_region.size.width < 1 || art_crop_region.size.height < 1)
			{
				art_crop_region = Rect2i(Point2i(0, 0), source_art_img->get_size());
			}

			if (art_crop_region.size.width > art_crop_region.size.height)
			{
				art_crop_scale = (double)entry_size.width / (double)art_crop_region.size.width;
			}
			else
			{
				art_crop_scale = (double)entry_size.height / (double)art_crop_region.size.height;
			}

			Rect2i scaled_art_region = Rect2i(Point2i(0, 0), art_crop_region.size);
			scaled_art_region.size = Size2i(Size2(Size2(scaled_art_region.size) * art_crop_scale).round());
			scaled_art_region.position += (scaled_art_region.size - entry_size) / 2; // integer division precision shouldn't be an issue...
			scaled_art_region.size = entry_size;

			Ref<Image> processed_art_image = source_art_img->get_region(art_crop_region);
			processed_art_image->resize(Math::round(processed_art_image->get_width() * art_crop_scale), Math::round(processed_art_image->get_height() * art_crop_scale), target.interpolation);
			processed_art_image = processed_art_image->get_region(scaled_art_region);

			if (art_data->is_flipped_h())
			{
				processed_art_image->flip_x();
			}
			if (art_data->is_flipped_v())
			{
				processed_art_image->flip_y();
			}

			// As the last step of pre-processing, apply the mask.
			if (base_mask_img.is_valid())
			{
				if (!size_mask_map.has(entry_size))
				{
					Ref<Image> scaled = base_mask_img->duplicate(false);
					scaled->resize(entry_size.width, entry_size.height, target.interpolation);
					size_mask_map.insert(entry_size, scaled);
				}
				processed_art_image->blend_rect_alpha(size_mask_map[entry_size], Rect2i(Point2i(0, 0), entry_size), Point2i(0, 0));
			}

			Ref<Image> entry_image = Image::create_empty(entry_size.width, entry_size.height, false, Image::FORMAT_RGBA8);

			if (base_bg_img.is_valid())
			{
				if (!size_bg_map.has(entry_size))
				{
					Ref<Image> scaled = base_bg_img->duplicate(false);
					scaled->resize(entry_size.width, entry_size.height, target.interpolation);
					size_bg_map.insert(entry_size, scaled);
				}
				entry_image->blit_rect(size_bg_map[entry_size], Rect2i(Point2i(0, 0), entry_size), Point2i(0, 0));
			}
			entry_image->blend_rect(processed_art_image, Rect2i(Point2i(0, 0), entry_size), Point2i(0, 0));
			if (base_fg_img.is_valid())
			{
				if (!size_fg_map.has(entry_size))
				{
					Ref<Image> scaled = base_fg_img->duplicate(false);
					scaled->resize(entry_size.width, entry_size.height, target.interpolation);
					size_fg_map.insert(entry_size, scaled);
				}
				entry_image->blend_rect(size_fg_map[entry_size], Rect2i(Point2i(0, 0), entry_size), Point2i(0, 0));
			}
			// Don't waste memory on an unneeded alpha channel.
			if (entry_image->detect_alpha() == Image::ALPHA_NONE)
			{
				entry_image->convert(Image::FORMAT_RGB8);
			}
			if (crop_texture_generate_enabled)
			{
				entry_image->crop(target.padded_size.width, target.padded_size.height);
			}
			Error image_save_error = entry_image->save_png(save_path_image);
			if (image_save_error == OK)
			{
				if (crop_texture_generate_enabled)
				{
					QueueGenerateCropTexture queue_entry;
					queue_entry.art_image_path = save_path_image;
					queue_entry.crop_texture_path = save_path_crop_texture;
					queue_entry.crop_region = Rect2i(Point2i(0, 0), entry_size);
					crop_texture_generate_queue.push_back(queue_entry);
				}
			}
		}
	}
	EditorFileSystem::get_singleton()->scan_changes();
	for (const QueueGenerateCropTexture &queued_crop : crop_texture_generate_queue)
	{
		Ref<AtlasTexture> atlas_texture;
		if (ResourceLoader::exists(queued_crop.crop_texture_path, "AtlasTexture"))
		{
			atlas_texture = ResourceLoader::load(queued_crop.crop_texture_path, "AtlasTexture");
		}
		else
		{
			atlas_texture.instantiate();
		}
		if (ResourceLoader::exists(queued_crop.art_image_path, "Texture2D"))
		{
			Ref<Texture2D> tex = ResourceLoader::load(queued_crop.art_image_path, "Texture2D");
			atlas_texture->set_atlas(tex);
		}
		else
		{
			atlas_texture->set_atlas(Ref<Texture2D>());
		}
		atlas_texture->set_region(queued_crop.crop_region);
		ResourceSaver::save(atlas_texture, queued_crop.crop_texture_path);
	}
}


UniformArtEditorPlugin::UniformArtEditorPlugin()
{
	art_collection_inspector_plugin.instantiate();
	art_collection_inspector_plugin->connect("rebuild_requested", callable_mp(this, &UniformArtEditorPlugin::rebuild_collection));
	add_inspector_plugin(art_collection_inspector_plugin);

	art_data_inspector_plugin.instantiate();
	add_inspector_plugin(art_data_inspector_plugin);
}


UniformArtEditorPlugin::~UniformArtEditorPlugin()
{
	art_collection_inspector_plugin->disconnect("rebuild_requested", callable_mp(this, &UniformArtEditorPlugin::rebuild_collection));
	remove_inspector_plugin(art_collection_inspector_plugin);
	remove_inspector_plugin(art_data_inspector_plugin);
}
