#include "formatTrans.h"

void save_voxel(const char* filename, vx_mesh_t* result)
{
	ofstream file(filename);
	for (int j = 0; j < result->nvertices; ++j) 
	{
		file << "v "<< result->vertices[j].x << " "
					<< result->vertices[j].y << " "
					<< result->vertices[j].z << "\n";
	}
	for (int j = 0; j < result->nindices; j += 4) 
	{
        file << "f "<< result->indices[j]+1 << " "
					<< result->indices[j+1]+1 << " "
					<< result->indices[j+2]+1 << " "
					<< result->indices[j+3]+1 << "\n";
    }
	file.close();
}

my_int3_t cal_point(vx_vertex_t p, float voxel_size)
{
	my_int3_t res;
	res.x = floor(p.x/voxel_size);
	res.y = floor(p.y/voxel_size);
	res.z = floor(p.z/voxel_size);
	return res;
}

void save_voxel_as_format(const char* filename1, const char* filename2, const char* filename3, 
						  vx_mesh_t* result, float voxel_size)
{
	vector<my_int3_t> int3_vec;
	my_int3_t max_int3(-1000),min_int3(1000),size_int3;
	for(int i=0;i<result->nvertices;i+=8)
	{
		my_int3_t tmp_int3 = cal_point(result->vertices[i], voxel_size);
		int3_vec.push_back(tmp_int3);
		for(int i=0;i<3;i++)
		{
			max_int3.v[i] = max(tmp_int3.v[i], max_int3.v[i]);
			min_int3.v[i] = min(tmp_int3.v[i], min_int3.v[i]);
		}
	}

	const int EDGE_SIZE = 1;
	for(int i=0;i<3;i++)
		size_int3.v[i] = max_int3.v[i] - min_int3.v[i] + 1 + 2*EDGE_SIZE;
	for(int i=0;i<3;i++)
		cout<<max_int3.v[i]<<" "<<min_int3.v[i]<<endl;
	vector<int> res_vec(size_int3.v[0]*size_int3.v[1]*size_int3.v[2], -1);

	for(int i=0;i<int3_vec.size();i++)
	{
		int tmp_v[3];
		for(int j=0;j<3;j++)
			tmp_v[j] = int3_vec[i].v[j] - min_int3.v[j] + EDGE_SIZE;
		int tmp_rank = size_int3.v[2]*(size_int3.v[1]*tmp_v[0] + tmp_v[1]) + tmp_v[2];
		res_vec[tmp_rank] = 1;
	}

	ofstream fout;
	fout.open(filename1);
	for(int i=0;i<size_int3.v[0];i++)
	{
		fout<<"level "<<i<<endl;
		for(int j=0;j<size_int3.v[1];j++)
		{
			for(int k=0;k<size_int3.v[2];k++)
			{
				fout<<res_vec[size_int3.v[2]*(size_int3.v[1]*i+j)+k]<<'\t';
			}
			fout<<endl;
		}
	}
	fout.close();

	fout.open(filename2);
	for(int j=0;j<size_int3.v[1];j++)
	{
		fout<<"level "<<j<<endl;
		for(int k=0;k<size_int3.v[2];k++)
		{
			for(int i=0;i<size_int3.v[0];i++)
			{
				fout<<res_vec[size_int3.v[2]*(size_int3.v[1]*i+j)+k]<<'\t';
			}
			fout<<endl;
		}
	}
	fout.close();

	fout.open(filename3);
	for(int k=0;k<size_int3.v[2];k++)
	{
		fout<<"level "<<k<<endl;
		for(int i=0;i<size_int3.v[0];i++)
		{
			for(int j=0;j<size_int3.v[1];j++)
			{
				fout<<res_vec[size_int3.v[2]*(size_int3.v[1]*i+j)+k]<<'\t';
			}
			fout<<endl;
		}
	}
	fout.close();
}