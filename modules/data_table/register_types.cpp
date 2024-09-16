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
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		memnew(DataTableManager);
		GDREGISTER_ABSTRACT_CLASS(DataTableManager);
		Engine::get_singleton()->add_singleton(Engine::Singleton("DataTableManager", DataTableManager::get_singleton(), "DataTableManager"));
		GDREGISTER_ABSTRACT_CLASS(DataTable);
		GDREGISTER_CLASS(DataTableCustom);
	}
#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR)
	{
		EditorPlugins::add_by_type<DataTableEditorPlugin>();
	}
#endif // TOOLS_ENABLED
}


void uninitialize_data_table_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		Engine::get_singleton()->remove_singleton("DataTableManager");
		memdelete(DataTableManager::get_singleton());
	}
}
