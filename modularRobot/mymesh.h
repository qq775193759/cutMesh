#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include<vector>
using namespace std;

class mymesh
{
public:
	vector<double> x,y,z;
	vector<int> tri[3];
	mymesh(string filename);
};

