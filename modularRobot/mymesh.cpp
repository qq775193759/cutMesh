#include "mymesh.h"
#include <cmath>


mymesh::mymesh(string filename)
{
	if(filename.substr(filename.size()-3,3) == "vtk")
	{
		ifstream fin(filename);
		string tmp;
		getline(fin,tmp);getline(fin,tmp);getline(fin,tmp);getline(fin,tmp);
		fin>>tmp;
		int v_num;
		fin>>v_num;
		fin>>tmp;
		for(int i=0;i<v_num;i++)
		{
			double tmp_double;
			fin>>tmp_double;x.push_back(tmp_double);
			fin>>tmp_double;y.push_back(tmp_double);
			fin>>tmp_double;z.push_back(tmp_double);
		}
		fin>>tmp;
		int c_num;
		fin>>c_num;
		fin>>tmp;
		for(int i=0;i<c_num;i++)
		{
			int n,a[4];
			fin>>n;
			for(int j=0;j<n;j++)
				fin>>a[j];
			if(n==3)
			{
				for(int j=0;j<n;j++)
					tri[j].push_back(a[j]);
			}
			if(n==4)
			{
				for(int j=0;j<3;j++)
				{
					tri[j].push_back(a[j]);
					tri[j].push_back(a[(j+1)%4]);
					tri[j].push_back(a[(j+2)%4]);
					tri[j].push_back(a[(j+3)%4]);
				}
			}
		}
		cout<<"vtk file"<<endl;
		fin.close();
	}
	if(filename.substr(filename.size()-3,3) == "obj")
	{
		ifstream fin(filename);
		string tmp;
		while(fin>>tmp)
		{
			if(tmp=="v")
			{
				double tmp_double;
				fin>>tmp_double;x.push_back(tmp_double);
				fin>>tmp_double;y.push_back(tmp_double);
				fin>>tmp_double;z.push_back(tmp_double);
			}
			if(tmp=="f")
			{
				int tmp_int;
				for(int i=0;i<3;i++)
				{
					fin>>tmp_int;
					tri[i].push_back(tmp_int-1);
				}
			}
		}
		fin.close();
		cout<<"obj file"<<endl;
	}
	if(x[0]>1 || x[0]<-1)
		normalize();
	cout<<"v size:"<<x.size()<<endl;
	cout<<"f size:"<<tri[0].size()<<endl;
}


void mymesh::normalize()
{
	max_x = max_y = max_z = -10000;
	min_x = min_y = min_z = 10000;
	for(int i=0;i<x.size();i++)
	{
		max_x = max(x[i], max_x);
		max_y = max(y[i], max_y);
		max_z = max(z[i], max_z);
		min_x = min(x[i], min_x);
		min_y = min(y[i], min_y);
		min_z = min(z[i], min_z);
	}
	double scale = max_x-min_x;
	for(int i=0;i<x.size();i++)
	{
		x[i] = (x[i]-(min_x+max_x)/2)/scale;
		y[i] = (y[i]-(min_y+max_y)/2)/scale;
		z[i] = (z[i]-(min_z+max_z)/2)/scale;
	}
}