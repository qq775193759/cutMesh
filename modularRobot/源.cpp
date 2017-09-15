#include <iostream> 
#include <fstream>
#include <vector>
using namespace std;
#include "vgl.h" 
#include "LoadShaders.h"
#include "rotate.h"
#include<GL/glut.h>

#include"mymesh.h"

#define VOXELIZER_IMPLEMENTATION
#include "formatTrans.h"


enum VAO_IDs { Triangles, Voxels, NumVAOs };
enum Buffer_IDs { ArrayBuffer, VoxelBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 , vPosition2 = 1};
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

GLint paintModeLoc;
GLint rotateMatrixLoc, windowMatrixLoc;
GLfloat rotateMatrix[16]={  1.0, 0.0, 0.0, 0.0,
							0.0, 1.0, 0.0, 0.0, 
							0.0, 0.0, 1.0, 0.0, 
							0.0, 0.0, 0.0, 1.0};
GLfloat repeatMatrix[25]={};
GLfloat inverseMatrix[9]={  1.0, 0.0, 0.0, 
							0.0, 1.0, 0.0, 
							0.0, 0.0, 1.0};
float scale_size = 1;
int projectSize = 2;
GLfloat windowMatrix[16]={0.0};

GLfloat press_x,press_y;

int WINDOW_RANGE_X = 640, WINDOW_RANGE_Y = 640;

const int VERT_DIM = 3;
const int EDGE_DIM = 2;
vector<GLfloat> edges_v2;
vector<GLfloat> voxel_triangle_v3;
bool edges_v2_show=1,voxel_triangle_v3_show=1,voxel_triangle_v3_blanking=1;
const float DEPTHEPS = 0.001;

RotateController *rotCtl;

vx_mesh_t* origin_mesh;
vx_mesh_t* rotate_mesh;
vx_mesh_t* voxel_mesh;
float voxel_size = 0.1;
float voxel_precision = 0.01;

/*void MyMesh2vx_mesh_t(MyMesh mesh, vx_mesh_t* &x)
{
	x = vx_mesh_alloc(mesh.n_vertices(), 3*mesh.n_faces());
	int f_no = 0;
	for(auto v_it = mesh.vertices_begin();v_it != mesh.vertices_end();v_it++)
	{
		x->vertices[v_it->idx()].x = mesh.point(*v_it).data()[0];
		x->vertices[v_it->idx()].y = mesh.point(*v_it).data()[1];
		x->vertices[v_it->idx()].z = mesh.point(*v_it).data()[2];
	}
	for(auto f_it = mesh.faces_begin();f_it != mesh.faces_end();f_it++)
	{
		for (auto fv_it=mesh.fv_iter( *f_it ); fv_it.is_valid(); fv_it++)
		{
			x->indices[f_no] = fv_it->idx();
			f_no++;
		}
	}
}*/

void MyMesh2vx_mesh_t(mymesh mesh, vx_mesh_t* &x)
{
	int v_size = mesh.x.size(), f_size = mesh.tri[0].size();
	x = vx_mesh_alloc(v_size, 3*f_size);
	int f_no = 0;
	for(int i=0;i<v_size;i++)
	{
		x->vertices[i].x=mesh.x[i];
		x->vertices[i].y=mesh.y[i];
		x->vertices[i].z=mesh.z[i];
	}
	for(int i=0;i<f_size;i++)
		for(int j=0;j<3;j++)
		{
			x->indices[f_no] = mesh.tri[j][i];
			f_no++;
		}
}

void cal_inverseMatrix()
{
	scale_size = 0;
	for(int j=0;j<3;j++)
	{
		scale_size += rotateMatrix[j%3]*rotateMatrix[j%3];
	}
	//scale_size = sqrt(scale_size);
	for(int i=0;i<3;i++)
		for(int j=0;j<3;j++)
			inverseMatrix[j*3+i] = rotateMatrix[(i%3)*4+(j%3)]/scale_size;
}

void cal_rotate_vertex_before(vx_vertex_t after)
{
	float x,y,z,rx,ry,rz;
	x = after.x - rotateMatrix[12];
	y = after.y - rotateMatrix[13];
	z = after.z - rotateMatrix[14];
	rx = inverseMatrix[0]*x + inverseMatrix[3]*y + inverseMatrix[6]*z;
	ry = inverseMatrix[1]*x + inverseMatrix[4]*y + inverseMatrix[7]*z;
	rz = inverseMatrix[2]*x + inverseMatrix[5]*y + inverseMatrix[8]*z;
	voxel_triangle_v3.push_back(rx);
	voxel_triangle_v3.push_back(ry);
	voxel_triangle_v3.push_back(rz);
}

void voxelize_for_show()
{
	cout<<"Voxelize begin:"<<endl;
	if(voxel_mesh) vx_mesh_free(voxel_mesh);//if it have ever been voxelized
	voxel_mesh = vx_voxelize(rotate_mesh, voxel_size, voxel_size, voxel_size, voxel_precision);
	cout<<"Voxelize end:"<<endl;
	voxel_triangle_v3.clear();
	cal_inverseMatrix();
	cout<< voxel_mesh->nindices/24 <<endl;
	for(int i=0;i<voxel_mesh->nindices;i++)
		cal_rotate_vertex_before(voxel_mesh->vertices[voxel_mesh->indices[i]]);
}

void read_point()
{
	string meshname;
	ifstream fin("config.txt");
	fin>>meshname;
	cout<<meshname<<endl;
	mymesh testmesh(meshname);
	fin.close();
	
	for(int i=0;i<testmesh.tri[0].size();i++)
		for(int j=0;j<3;j++)
		{
			int tmp_rank = testmesh.tri[j][i];
			edges_v2.push_back(testmesh.x[tmp_rank]);
			edges_v2.push_back(testmesh.y[tmp_rank]);
			edges_v2.push_back(testmesh.z[tmp_rank]);
		}
	cout<<"In total, Edges: "<<edges_v2.size()/6<<endl;
	MyMesh2vx_mesh_t(testmesh, origin_mesh);
	MyMesh2vx_mesh_t(testmesh, rotate_mesh);
	voxelize_for_show();
}

vx_vertex_t cal_rotate_vertex(vx_vertex_t origin)
{
	vx_vertex_t res;
	for(int i=0;i<3;i++)
		res.v[i] = origin.x*rotateMatrix[i] + origin.y*rotateMatrix[4+i] + origin.z*rotateMatrix[8+i] + rotateMatrix[12+i];
	return res;
}

void cal_rotate_mesh()
{
	for(int i=0;i<origin_mesh->nvertices;i++)
		rotate_mesh->vertices[i] = cal_rotate_vertex(origin_mesh->vertices[i]);
}

void revoxelize()
{
	cal_rotate_mesh();
	voxelize_for_show();
	glBindVertexArray(VAOs[Voxels]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * voxel_triangle_v3.size(), &voxel_triangle_v3[0], GL_STATIC_DRAW);
}


void updateWindowMatrix()
{
	windowMatrix[0] = windowMatrix[5]  = windowMatrix[15] = 1.0;
	windowMatrix[10] = 0.5;
	windowMatrix[0] *= WINDOW_RANGE_Y;
	windowMatrix[0] /= WINDOW_RANGE_X;
}



void init(void) 
{
	glGenVertexArrays(NumVAOs, VAOs); 
	glGenBuffers(NumBuffers, Buffers);
	read_point();

	glBindVertexArray(VAOs[Triangles]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * edges_v2.size(), &edges_v2[0], GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, VERT_DIM, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); 
	glEnableVertexAttribArray(vPosition);

	glBindVertexArray(VAOs[Voxels]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[VoxelBuffer]); 
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * voxel_triangle_v3.size(), &voxel_triangle_v3[0], GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, VERT_DIM, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); 
	glEnableVertexAttribArray(vPosition);

	//load shader
	ShaderInfo shaders[] = { { GL_VERTEX_SHADER, "triangles.vert" }, { GL_FRAGMENT_SHADER, "triangles.frag" }, { GL_NONE, NULL } };
	GLuint program = LoadShaders(shaders); 
	glUseProgram(program);
	
	//for matrix
	rotateMatrixLoc = glGetUniformLocation(program, "rotateMatrix");
	//projectMatrixLoc = glGetUniformLocation(program, "projectMatrix");
	windowMatrixLoc = glGetUniformLocation(program, "windowMatrix");
	paintModeLoc = glGetUniformLocation(program, "paintMode");
	//initProjectMatrix();
	//glUniformMatrix4fv(projectMatrixLoc, 1, GL_FALSE, projectMatrix);//no use

	glPointSize(6.0f);
	glLineWidth(1.0f);
	
	rotCtl = new RotateController(rotateMatrix);

	//DEPTH TEST FOR HIDDEN
	glEnable(GL_DEPTH_TEST);
}
//--------------------------------------------------------------------// // display //
void display(void) 
{ 
	//cout<<"display"<<endl;
	glUniformMatrix4fv(rotateMatrixLoc, 1, GL_FALSE, rotateMatrix);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(edges_v2_show)
	{
		glBindVertexArray(VAOs[Triangles]); 
		glUniform1i(paintModeLoc, 0);
		glColorMask(0,0,0,0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDepthRange(DEPTHEPS,1);
		glDrawArrays(GL_TRIANGLES, 0, edges_v2.size()/3);
		glColorMask(1, 1, 1, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDepthRange(0,1 - DEPTHEPS);
		glDrawArrays(GL_TRIANGLES, 0, edges_v2.size()/3);
	}
	if(voxel_triangle_v3_show)
	{
		glBindVertexArray(VAOs[Voxels]);
		glUniform1i(paintModeLoc, 1);
		if(voxel_triangle_v3_blanking)
		{
			glColorMask(0,0,0,0);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDepthRange(DEPTHEPS,1);
			glDrawArrays(GL_QUADS, 0, voxel_triangle_v3.size()/3);
		}
		glColorMask(1, 1, 1, 1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDepthRange(0,1 - DEPTHEPS);
		glDrawArrays(GL_QUADS, 0, voxel_triangle_v3.size()/3);
	}
	glutSwapBuffers();
	glutPostRedisplay();
}


void reshapeFunc(int width, int height)
{
	//cout<<width<<height<<endl;
	glViewport(0, 0, width, height);
	WINDOW_RANGE_X = width;
	WINDOW_RANGE_Y = height;
	updateWindowMatrix();
	glUniformMatrix4fv(windowMatrixLoc, 1, GL_FALSE, windowMatrix);
	rotCtl->resize(width, height);
}

void keyboardFunc(unsigned char key, int x, int y)
{
	cout<<key<<endl;
	switch (key)
	{
	case ' ':
		revoxelize();
		break;
	case '1':
		edges_v2_show = 1 - edges_v2_show; 
		break;
	case '2':
		voxel_triangle_v3_show = 1 - voxel_triangle_v3_show;
		break;
	case '3':
		voxel_triangle_v3_blanking = 1 - voxel_triangle_v3_blanking;
		break;
	case 'r':
		rotCtl->recoverRot();
		break;
	case 's':
		save_voxel("save.obj", voxel_mesh);
		save_voxel_as_format("save_x.txt", "save_y.txt", "save_z.txt", voxel_mesh, voxel_size);
		break;
	}
	glutPostRedisplay();
}

int press_button;
float scale_rate = 1.0;

void mouseFunc(int button, int state, int x, int y)
{
	press_button = button;
	cout<<button<<" "<<state<<" "<<x<<" "<<y<<endl;
	//roll and scale
	const int GLUT_WHEEL_UP = 3;
	const int GLUT_WHEEL_DOWN = 4;
	if(button == GLUT_MIDDLE_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			//scale default
			float tmp_scale_rate = 1.0/scale_rate;
			rotCtl->scale(tmp_scale_rate, Vector2D(x,y));
			scale_rate = scale_rate * tmp_scale_rate;
		}
	}
	if(button == GLUT_WHEEL_UP && state == GLUT_UP)
	{
		//scale up
		float tmp_scale_rate = 1.1;
		rotCtl->scale(tmp_scale_rate, Vector2D(x,y));
		scale_rate = scale_rate * tmp_scale_rate;
	}
	if(button == GLUT_WHEEL_DOWN && state == GLUT_UP)
	{
		//scale down
		float tmp_scale_rate = 0.9;
		rotCtl->scale(tmp_scale_rate, Vector2D(x,y));
		scale_rate = scale_rate * tmp_scale_rate;
	}
	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		//shift
		press_x = x;
		press_y = y;
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		rotCtl->MousePress(Vector2D(x,y));
	}
	glutPostRedisplay();
}

void motionFunc(int x, int y)
{
	if(press_button == GLUT_RIGHT_BUTTON)
	{
		//shift
		float delta_x = x - press_x, delta_y = y - press_y;
		rotCtl->translate(Vector3D(2*delta_x/WINDOW_RANGE_Y, -2*delta_y/WINDOW_RANGE_Y, 0));
		press_x = x;
		press_y = y;
	}
	if(press_button == GLUT_LEFT_BUTTON)
	{
		rotCtl->MouseMoveRotate(Vector2D(x,y));
	}
	glutPostRedisplay();
}


int main(int argc, char** argv) 
{ 
	//trans begin
	/*trans_xyz_to_format("bunny");
    trans_xyz_to_format("table");
    trans_xyz_to_format("teddy");
    trans_xyz_to_format("eight");
    trans_xyz_to_format("chair");
    trans_xyz_to_format("cup");*/
	//trans end

	cout<<"OK"<<endl;
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_RGBA); 
	glutInitWindowSize(WINDOW_RANGE_X, WINDOW_RANGE_Y); 
	//glutInitContextVersion(4, 3); 
	glutInitContextVersion(2, 0); 
	glutInitContextProfile(GLUT_CORE_PROFILE); 
	glutCreateWindow(argv[0]);
	glewExperimental = GL_TRUE;
	if (glewInit()) 
	{ 
		cout<<"error"<<endl;
		cerr << "Unable to initialize GLEW ... exiting" << endl; 
		exit(EXIT_FAILURE); 
	}
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardFunc);
	glutReshapeFunc(reshapeFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glutMainLoop();
}
