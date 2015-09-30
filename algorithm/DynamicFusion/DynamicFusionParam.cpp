#include "DynamicFusionParam.h"
#include <math.h>
namespace dfusion
{
	Param::Param()
	{
		/** *****************************************************
		* tsdf volume related
		* ******************************************************/
		volume_resolution[0] = 256;
		volume_resolution[1] = 256;
		volume_resolution[2] = 256;
		voxels_per_meter = 256;

		/** *****************************************************
		* marching cube related
		* ******************************************************/
		marching_cube_level = 0;
		marching_cube_tile_size = 256;
		marching_cube_max_activeVoxel_ratio = 0.2;
		marching_cube_isoValue = 0.f;

		/** *****************************************************
		* warp field related
		* ******************************************************/
		warp_radius_search_epsilon = 0.05;
		warp_param_dw = warp_radius_search_epsilon * 1.5;
		warp_radius_search_beta = 4;
		warp_point_step_before_update_node = 1;
		warp_valid_point_num_each_node = 0.1 * (pow(warp_radius_search_epsilon*voxels_per_meter, 3) 
			/ warp_point_step_before_update_node);

		/** *****************************************************
		* dynamic fusion related
		* ******************************************************/
		fusion_max_weight = 128;
		fusion_lambda = 200;
		fusion_psi_data = 0.01;
		fusion_psi_reg = 0.0001;

		/** *****************************************************
		* visualization related
		* ******************************************************/
		view_show_mesh = true;
		view_show_nodes = true;
		view_show_graph = true;
		view_show_graph_level = 0;
	}
}