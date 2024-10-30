#include "register_types.h"

#ifdef TOOLS_ENABLED
#include "editor/uniform_art_data.h"
#include "editor/uniform_art_collection.h"
#include "editor/plugins/uniform_art_editor_plugin.h"
#endif // TOOLS_ENABLED


void initialize_uniform_art_module(ModuleInitializationLevel p_level)
{
#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR)
	{
		EditorPlugins::add_by_type<UniformArtEditorPlugin>();
	}
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		GDREGISTER_CLASS(UniformArtData);
		GDREGISTER_CLASS(UniformArtCollection);
	}
#endif // TOOLS_ENABLED
}


void uninitialize_uniform_art_module(ModuleInitializationLevel p_level)
{
}
