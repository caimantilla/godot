#include "register_types.h"
#include "data_table.hpp"
#include "data_table_custom.hpp"
#include "data_table_manager.hpp"
#include "core/config/engine.h"

#ifdef TOOLS_ENABLED
#include "editor/data_table_editor_plugin.hpp"
#endif // TOOLS_ENABLED


void initialize_data_table_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
		GDREGISTER_ABSTRACT_CLASS(DataTableManager);
		GDREGISTER_ABSTRACT_CLASS(DataTable);
		GDREGISTER_CLASS(DataTableCustom);

		DataTableManager::create_singleton();
	}
#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR)
	{
		if (Engine::get_singleton()->is_editor_hint())
		{
			EditorPlugins::add_by_type<DataTableEditorPlugin>();
		}
	}
#endif // TOOLS_ENABLED
}


void uninitialize_data_table_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SERVERS)
	{
#ifdef TOOLS_ENABLED
		if (Engine::get_singleton()->is_editor_hint())
		{
			return;
		}
#endif
		DataTableManager::delete_singleton();
	}
}
