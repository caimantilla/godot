#ifndef UNIFORM_ART_EDITOR_PLUGIN_H
#define UNIFORM_ART_EDITOR_PLUGIN_H


#include "../uniform_art_collection.h"
#include "../uniform_art_data.h"
#include "scene/gui/margin_container.h"
#include "scene/resources/atlas_texture.h"
#include "editor/editor_inspector.h"
#include "editor/plugins/editor_plugin.h"


class TextureRect;


class UniformArtDataPreview final : public MarginContainer
{
	GDCLASS(UniformArtDataPreview, MarginContainer);

private:
	Ref<UniformArtData> art_data;
	TextureRect *checkerboard;
	Ref<AtlasTexture> atlas_texture;
	TextureRect *texture_display;

	String cache_image_path;

	void update_texture_display();
	void reload_image();
	void on_art_data_changed();

protected:
	void _notification(int p_what);

public:
	void set_art_data(const Ref<UniformArtData> &p_art);
	Ref<UniformArtData> get_art_data() const;

	UniformArtDataPreview();
};


class EditorInspectorPluginUniformArtCollection final : public EditorInspectorPlugin
{
	GDCLASS(EditorInspectorPluginUniformArtCollection, EditorInspectorPlugin);

private:
	void request_rebuild(Ref<UniformArtCollection> p_collection);
	void on_rebuild_button_theme_changed(Button *p_button);

protected:
	static void _bind_methods();

public:
	virtual bool can_handle(Object *p_object) override;
	virtual void parse_begin(Object *p_object) override;
};


class EditorInspectorPluginUniformArtData final : public EditorInspectorPlugin
{
	GDCLASS(EditorInspectorPluginUniformArtData, EditorInspectorPlugin);

public:
	virtual bool can_handle(Object *p_object) override;
	virtual void parse_begin(Object *p_object) override;
};


class UniformArtEditorPlugin final : public EditorPlugin
{
	GDCLASS(UniformArtEditorPlugin, EditorPlugin);

	struct QueueGenerateCropTexture final
	{
		String art_image_path;
		String crop_texture_path;
		Rect2i crop_region;
	};

	// HashMap<Ref<UniformArtCollection>, HashSet<Ref<UniformArtData>>> cache_unchanged_art_entry_map;
	Ref<EditorInspectorPluginUniformArtCollection> art_collection_inspector_plugin;
	Ref<EditorInspectorPluginUniformArtData> art_data_inspector_plugin;

	// void erase_art_from_cache(UniformArtCollection *p_collection, UniformArtData *p_art);
	// void clear_collection_cache(UniformArtCollection *p_collection);

public:
	void rebuild_collection(const Ref<UniformArtCollection> &p_collection);

	UniformArtEditorPlugin();
	~UniformArtEditorPlugin();
};


#endif // UNIFORM_ART_EDITOR_PLUGIN_H
