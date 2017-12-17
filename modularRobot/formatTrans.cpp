#include "formatTrans.h"
#include <string>

void save_voxel(const char* filename, vx_mesh_t* result)
	//not use it to save
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

void save_voxel(const char* filename, vector<int> hollow_vec, my_int3_t size_int3)
{
	const int dx[8] = {0, 1, 1, 0, 0, 1, 1, 0};
	const int dy[8] = {0, 0, 1, 1, 0, 0, 1, 1};
	const int dz[8] = {0, 0, 0, 0, 1, 1, 1, 1};
	const int face[6][4] = {1, 2, 3, 4, 
							5, 6, 7, 8, 
							1, 5, 8, 4, 
							2, 6, 7, 3, 
							1, 2, 6, 5, 
							4, 3, 7, 8};
	ofstream file(filename);
	int cube_co = 0;
	for(int i=0;i<size_int3.v[0];i++)
		for(int j=0;j<size_int3.v[1];j++)
			for(int k=0;k<size_int3.v[2];k++)
				if(hollow_vec[size_int3.v[2]*(size_int3.v[1]*i+j)+k] == 1)
				{
					for(int vco=0;vco<8;vco++)
						file << "v "<< i + dx[vco] << " "<< j + dy[vco] << " "<< k + dz[vco] << "\n";
					for(int fco=0;fco<6;fco++)
					{
						file << "f ";
						for(int vco=0;vco<4;vco++)
							file<< cube_co + face[fco][vco] << " ";
						file<< "\n";
					}
					cube_co+=8;
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

vector<int> fix_surround(vector<int> hollow_vec, my_int3_t size_int3)
{
	for(int i=0;i<size_int3.v[0];i++)
		for(int j=0;j<size_int3.v[1];j++)
			for(int k=0;k<size_int3.v[2];k++)
				if((i*(size_int3.v[0]-i-1)*j*(size_int3.v[1]-j-1)*k*(size_int3.v[2]-k-1)) == 0)
					hollow_vec[size_int3.v[2]*(size_int3.v[1]*i+j)+k] = 2;
	return hollow_vec;
}

vector<int> fix_over(vector<int> hollow_vec, my_int3_t size_int3)
{
	for(int i=0;i<size_int3.v[0];i++)
		for(int j=0;j<size_int3.v[1];j++)
			for(int k=0;k<size_int3.v[2];k++)
				if(hollow_vec[size_int3.v[2]*(size_int3.v[1]*i+j)+k] == -1)
					hollow_vec[size_int3.v[2]*(size_int3.v[1]*i+j)+k] = 1;
				else if(hollow_vec[size_int3.v[2]*(size_int3.v[1]*i+j)+k] == 2)
					hollow_vec[size_int3.v[2]*(size_int3.v[1]*i+j)+k] = -1;
	return hollow_vec;
}

vector<int> fix_3d(vector<int> hollow_vec, my_int3_t size_int3, int type)
{
	hollow_vec = fix_surround(hollow_vec, size_int3);
	const int dx[12] = {1, -1, 0, 0, 0, 0, 1, -1, 0, 0, 0, 0};
	const int dy[12] = {0, 0, 1, -1, 0, 0, 0, 0, 1, -1, 0, 0};
	const int dz[12] = {0, 0, 0, 0, 1, -1, 0, 0, 0, 0, 1, -1};
	int dr_st[4] = {0, 2, 4, 0};
	int dr_en[4] = {4, 6, 8, 6};
	int solid_co;
	do{
		solid_co = 0;
		for(int i=1;i<(size_int3.v[0]-1);i++)
			for(int j=1;j<(size_int3.v[1]-1);j++)
				for(int k=1;k<(size_int3.v[2]-1);k++)
					if(hollow_vec[size_int3.v[2]*(size_int3.v[1]*i+j)+k] == -1)
						for(int dr=dr_st[type];dr<dr_en[type];dr++)
							if(hollow_vec[size_int3.v[2]*(size_int3.v[1]*(i+dx[dr])+(j+dy[dr]))+(k+dz[dr])] == 2)
							{
								hollow_vec[size_int3.v[2]*(size_int3.v[1]*i+j)+k] = 2;
								solid_co++;
								break;
							}
		cout<<solid_co<<" ";
	}while(solid_co != 0);
	hollow_vec = fix_over(hollow_vec, size_int3);
	return hollow_vec;
}


vector<int> get_solid(vector<int> hollow_vec, my_int3_t size_int3)
{
	for(int i=0;i<4;i++)
		hollow_vec = fix_3d(hollow_vec, size_int3, i);
	int solid_co = 0;
	for(int i=0;i<size_int3.v[0];i++)
		for(int j=0;j<size_int3.v[1];j++)
			for(int k=0;k<size_int3.v[2];k++)
			{
				if(hollow_vec[size_int3.v[2]*(size_int3.v[1]*i+j)+k] == 1)
					solid_co++;
			}
	cout<<"solid voxels: "<<solid_co<<endl;
	return hollow_vec;
}

void save_voxel_as_format(const char* filename1, const char* filename2, const char* filename3, const char* filename, 
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

	res_vec = get_solid(res_vec, size_int3);
	save_voxel(filename, res_vec, size_int3);

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

my_int3_t cal_point(float px, float py, float pz)
{
	my_int3_t res;
	float voxel_size = 20.5;
	res.x = floor(px/voxel_size + 0.1);
	res.y = floor(py/voxel_size + 0.1);
	res.z = floor(pz/voxel_size + 0.1);
	return res;
}

void trans_xyz_to_format(const string filename1, const string filename2, const string filename3,
						  const string xyzname)
{
	vector<my_int3_t> int3_vec;
	my_int3_t max_int3(-1000),min_int3(1000),size_int3;
	ifstream fin(xyzname);
	float px,py,pz;
	while(fin>>px)
	{
		fin>>py>>pz;
		my_int3_t tmp_int3 = cal_point(px,py,pz);
		int3_vec.push_back(tmp_int3);
		for(int i=0;i<3;i++)
		{
			max_int3.v[i] = max(tmp_int3.v[i], max_int3.v[i]);
			min_int3.v[i] = min(tmp_int3.v[i], min_int3.v[i]);
		}
	}
	fin.close();

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

void trans_xyz_to_format(string name)
{
	trans_xyz_to_format("xyz/"+name+"_x.txt", "xyz/"+name+"_y.txt", "xyz/"+name+"_z.txt", "xyz/"+name+".xyz");
}