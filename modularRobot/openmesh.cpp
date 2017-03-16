#include <iostream>
#include <iterator>
// -------------------- OpenMesh
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Utils/getopt.h>

// ----------------------------------------------------------------------------

using namespace OpenMesh;
using namespace std;

// ----------------------------------------------------------------------------

typedef TriMesh_ArrayKernelT<>  MyMesh;

void print_v(MyMesh::Point x)
{
	cout<<x.data()[0]<<" "<<x.data()[1]<<" "<<x.data()[2]<<endl;
}

int openmesh(int argc, char **argv)
{
	MyMesh  mesh;
	IO::Options ropt, wopt;

	// -------------------- read mesh

	if ( ! IO::read_mesh(mesh,"output.off", ropt))
	{
		std::cerr << "Error loading mesh from file " << std::endl;
		return 1;
	}

	for(auto v_it = mesh.vertices_begin();v_it != mesh.vertices_end(); v_it++)
	{
		print_v(mesh.point(*v_it));
		for (auto vv_it=mesh.vv_iter( *v_it ); vv_it.is_valid(); vv_it++)
        {
			print_v(mesh.point(*vv_it)-mesh.point(*v_it));
        }
		cout<<endl;
	}
	int a = 0;
	for(auto v_it = mesh.faces_begin();v_it != mesh.faces_end(); v_it++)
	{
		for (auto vv_it=mesh.fv_iter( *v_it ); vv_it.is_valid(); vv_it++)
		{
			a++;
		}
		cout<<a<<endl;
	}
	cout<<a<<endl;

	cin>>a;

  return 0;
}