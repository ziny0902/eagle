#include <iostream>
#include <vector>
#include <Math/multivcalculus.h> 

using namespace MathHelper;

double f_xy(double *v)
{
  return v[0]*std::exp(v[1]);
}

double f_xyz(double *v)
{
  return 80/(1+ std::pow(v[0], 2) + 2*std::pow(v[1],2) + 3*std::pow(v[2], 2));
}

int main()
{
  double v[] = {2, 0};
  double d[] = {-1.5, 2};
  double u[2];
  v_2_u<2>(d, u);
  double value = directional_derivative(f_xy, v, u, 2);
  std::vector<double> grad_xy = gradiant(f_xy, v, 2);
  std::cout << value << std::endl;
  std::cout << "[" << grad_xy[0] << " " << grad_xy[1] << "]" << std::endl;

  double v_xyz[] = {1, 1, -2};
  std::vector<double> grad = gradiant(f_xyz, v_xyz, 3);
  std::cout << "[" << grad[0] << " " << grad[1] << " "  << grad[2] << "]" << std::endl;
  return 0;

  double d_xyz[] = {1, 2, -1};
  double u_xyz[3];
  v_2_u<3>(d_xyz, u_xyz);
  std::cout << directional_derivative(f_xyz, v_xyz, u_xyz, 3);
  return 0;
}
