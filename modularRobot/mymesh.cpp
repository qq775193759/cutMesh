#include "mymesh.h"


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
	cout<<"v size:"<<x.size()<<endl;
	cout<<"f size:"<<tri[0].size()<<endl;
}


