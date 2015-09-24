#include "GpuKdTree.h"

#include "PointTree.h"
#include "ObjMesh.h"

namespace dfusion
{
	const char* g_mesh_test_name = "E:\\MyProjects\\DynamicFusion\\code\\DynamicFusionUI\\data\\Alma.obj";
	typedef ldp::kdtree::PointTree<float> CpuKdTree;
	typedef CpuKdTree::Point CpuPoint;

	void GpuKdTree::test()
	{
		try
		{
			const int Knn = 1;
			const int maxLeafNodes = 32;
			const int nQueryX = 256;
			const int nQuery =  nQueryX * nQueryX * nQueryX;

			ObjMesh mesh;
			mesh.loadObj(g_mesh_test_name, false, false);
			mesh.vertex_list.resize(1024);

			std::vector<CpuPoint> point_h(mesh.vertex_list.size());
			std::vector<float4> point_d_host(mesh.vertex_list.size());
			for (size_t i = 0; i < mesh.vertex_list.size(); i++)
			{
				ldp::Float3 v = mesh.vertex_list[i];
				point_h[i].p = v;
				point_h[i].idx = i;
				point_d_host[i] = make_float4(v[0], v[1], v[2], 0);
			}


			std::vector<CpuPoint> point_h_query(nQuery);
			std::vector<float4> point_d_host_query(nQuery);
			for (int i = 0; i < nQuery; i++)
			{
				int z = i / (nQueryX*nQueryX);
				int y = (i - z*nQueryX*nQueryX) / nQueryX;
				int x = i % nQueryX;
				float vx = float(x) / nQueryX;
				float vy = float(y) / nQueryX;
				float vz = float(z) / nQueryX;
				point_h_query[i].p = ldp::Float3(vx, vy, vz);
				point_h_query[i].idx = i;
				point_d_host_query[i] = make_float4(vx, vy, vz, 0);
			}
			


			DeviceArray<float4> points_d;
			DeviceArray<float4> points_d_query;
			points_d.upload(point_d_host.data(), point_d_host.size());
			points_d_query.upload(point_d_host_query.data(), point_d_host_query.size());

			std::vector<int> index_h(point_h_query.size()*Knn);
			std::vector<float> dist_h(point_h_query.size()*Knn);
			DeviceArray<int> index_d(point_h_query.size()*Knn);
			std::vector<int> index_d_host(point_h_query.size()*Knn);
			DeviceArray<float> dist_d(point_h_query.size()*Knn);
			std::vector<float> dist_d_host(point_h_query.size()*Knn);

			for (int test_iter = 0; test_iter < 1; test_iter++)
			{
				printf("test iter %d------------------------\n", test_iter);
				// cpu build
				ldp::tic();
				CpuKdTree tree_h;
				tree_h.build(point_h);
				ldp::toc("cpu build time");

				ldp::tic();
				for (int i = 0; i < point_h_query.size(); i++)
				{
					CpuPoint p = point_h_query[i];
					float dist = 0.f;
					CpuPoint nbp = tree_h.nearestPoint(p, dist);
					index_h[i] = nbp.idx;
					dist_h[i] = dist;
				}
				ldp::toc("cpu search time");

				// gpu build
				ldp::tic();
				GpuKdTree tree_d(points_d.ptr(), sizeof(float4), points_d.size(), maxLeafNodes);
				tree_d.buildTree();
				cudaThreadSynchronize();
				ldp::toc("gpu build time");

				ldp::tic();
				tree_d.knnSearchGpu(points_d_query.ptr(), index_d.ptr(), dist_d.ptr(), Knn, points_d_query.size());
				cudaThreadSynchronize();
				ldp::toc("gpu search time");


				////// compare
				//index_d.download(index_d_host);
				//dist_d.download(dist_d_host);
				//for (size_t i = 0; i < index_d_host.size(); i++)
				//{
				//	int id1 = index_h[i];
				//	int id2 = index_d_host[i];
				//	float dt1 = dist_h[i];
				//	float dt2 = sqrt(dist_d_host[i]);
				//	if (id1 != id2)
				//	{
				//		if (fabs(dt1-dt2)>1e-7)
				//		printf("error, cpu, gpu not matched: %d (%d %f) (%d %f)\n", i, id1, dt1, id2, dt2);
				//	}
				//}
			}
		}// end for try
		catch (std::exception e)
		{
			std::cout << e.what() << std::endl;
		}
	}
}