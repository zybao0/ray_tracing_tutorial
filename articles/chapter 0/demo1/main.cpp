#include<iostream>
#include<Eigen/Eigen>
#include<Eigen/Dense>
using namespace Eigen;
using namespace std;  
int main()
{
	Matrix4d mat=MatrixXd::Random(4,4);
	Vector4d u(1,2,3,4),ans;
	
	cout<<"u:"<<endl<<u<<endl<<endl;
	cout<<"mat:"<<endl<<mat<<endl<<endl;
	ans.noalias()=mat*u;
	cout<<"ans:"<<endl<<ans<<endl<<endl;
}