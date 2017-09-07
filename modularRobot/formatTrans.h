#pragma once
#include "voxelizer.h"
#include<iostream>
#include<fstream>
#include<vector>
using namespace std;

//save voxel
void save_voxel(const char* filename, vx_mesh_t* result);


//save voxel format like layer bit
typedef struct my_int3{
	union{
		int v[3];
        struct {
            int x;
            int y;
            int z;
        };
	};
	my_int3(){}
	my_int3(int xyz):x(xyz),y(xyz),z(xyz){}
}my_int3_t;

my_int3_t cal_point(vx_vertex_t p, float voxel_size);

void save_voxel_as_format(const char* filename1, const char* filename2, const char* filename3,
						  vx_mesh_t* result, float voxel_size);


void trans_xyz_to_format(string name);
