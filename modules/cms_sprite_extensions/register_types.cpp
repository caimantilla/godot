#include "register_types.h"
#include "scene/3d/directional_animated_sprite_controller_3d.h"

#ifdef TOOLS_ENABLED
#include "editor/plugins/directional_animated_sprite_editor_plugin.h"
#endif // TOOLS_ENABLED


void initialize_cms_sprite_extensions_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		GDREGISTER_CLASS(DirectionalAnimatedSpriteController3D);
	}
#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR)
	{
		EditorPlugins::add_by_type<DirectionalAnimatedSpriteEditorPlugin>();
	}
#endif // TOOLS_ENABLED
}


void uninitialize_cms_sprite_extensions_module(ModuleInitializationLevel p_level)
{
}
