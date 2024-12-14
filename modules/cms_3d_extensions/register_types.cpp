#include "register_types.h"
#include "scene/resources/3d/corner_deformable_grid_mesh.h"
#include "scene/resources/3d/corner_deformable_plane_mesh.h"
#include "scene/resources/3d/free_deformable_plane_mesh.h"
#include "scene/resources/3d/quad_split_corner_deformable_plane_mesh.h"


void initialize_cms_3d_extensions_module(ModuleInitializationLevel p_level)
{
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE)
	{
		GDREGISTER_CLASS(CornerDeformableGridMesh);
		GDREGISTER_CLASS(CornerDeformablePlaneMesh);
		GDREGISTER_CLASS(FreeDeformablePlaneMesh);
		GDREGISTER_CLASS(QuadSplitCornerDeformablePlaneMesh);
	}
}


void uninitialize_cms_3d_extensions_module(ModuleInitializationLevel p_level)
{
}
