#include "register_types.h"
#include "scene/gui/dynamic_speech_balloon.h"
#include "scene/gui/grid_layout_rect.h"


void initialize_cms_gui_extensions_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		GDREGISTER_CLASS(DynamicSpeechBalloon);
		GDREGISTER_CLASS(GridLayoutRect);
	}
}


void uninitialize_cms_gui_extensions_module(ModuleInitializationLevel p_level)
{
}
