#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
using namespace std;

class mymesh
{
	double max_x, min_x,max_y, min_y,max_z, min_z;
public:
	vector<double> x,y,z;
	vector<int> tri[3];
	mymesh(string filename);
	void normalize();
};

