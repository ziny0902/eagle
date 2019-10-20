#include <iostream>
#include <vector>
#include <limits>
#include <Math/multivcalculus.h> 

typedef std::numeric_limits< double > dbl;

using namespace MathHelper;

double f_xy(double *v)
{
  return v[0]*std::exp(v[1]);
}

double f_xyz(double *v)
{
  return 80/(1+ std::pow(v[0], 2) + 2*std::pow(v[1],2) + 3*std::pow(v[2], 2));
}

double f_ex_11_6_8(double *xyz)
{
  return std::pow(xyz[0], 2)/4 + std::pow(xyz[1], 2) + std::pow(xyz[2], 2)/9;
}


glm::dvec3 v_f_parametric(double t)
{
  double x = std::cos(t);
  double y = std::sin(t);
  double z = t;
  return glm::dvec3(x,y,z);
}

double f_parametric(double t, int idx)
{
  double ret;
  switch(idx){
    case 0:
      ret = std::cos(t);
      break;
    case 1:
      ret = std::sin(t);
      break;
    case 2:
      ret = t;
      break;
  }
  if( idx < 3)
  {
#ifdef __DEBUG__
    std::cout.precision(dbl::max_digits10);
    std::cout
        << " t: " << t << " ; "
        << "idx: " << idx << " ; "
        << "val: " << ret << std::endl;
#endif
    return ret;
  }
  std::cout
      << __FILE__ << "(" << __LINE__ << ")"
      << "Error: Unknown index: " << idx << std::endl;
  return 0;
}

double f_ex_11_3_2(double* xy)
{
  return 4 - xy[0]*xy[0] - 2*xy[1]*xy[1];
}

double f_ex_11_4_1(double* xy)
{
  return 2*xy[0]*xy[0] + xy[1]*xy[1];
}

double f_ex_11_4_2(double* xy)
{
  return xy[0]*std::exp(xy[0]*xy[1]);
}

double f_ex_11_4_7(double* xy, int idx)
{
  if(idx == 0)
    return xy[0]*xy[0];
  if(idx == 1)
    return xy[1]*xy[1];
  if(idx == 2)
    return xy[0] + 2*xy[1];
  std::cout << "Critical Error: dimention error: " << idx << "\n";
  exit(-1);
  return -1;
}

glm::dvec3 f_ex_11_4_7(double u, double v)
{
  double x = u*u;
  double y = v*v;
  double z = u+2*v;
  return glm::dvec3(x, y, z);
}

double f_ex_11_5_1(double* xy, int idx)
{
  if(idx == 0)
    return std::sin(2*xy[0]);
  if(idx == 1)
    return std::cos(xy[0]);
  if(idx == 2)
    return std::sin(2*xy[0])*std::sin(2*xy[0])*std::cos(xy[0]) +
        3*std::sin(2*xy[0])*std::pow(std::cos(xy[0]), 2);
  std::cout << "Critical Error: dimention error: " << idx << "\n";
  exit(-1);
  return -1;
}



using namespace MathHelper;

int main()
{
  double xy[] = {1, 1};
  std::cout << "Dx: " << partial(f_ex_11_3_2, xy, 0) << std::endl;
  std::cout << "Dy: " << partial(f_ex_11_3_2, xy, 1) << std::endl;

  std::cout << "Dx: " << partial(f_ex_11_4_1, xy, 0) << std::endl;
  std::cout << "Dy: " << partial(f_ex_11_4_1, xy, 1) << std::endl;

  xy[0]=1; xy[1]=0;
  std::cout << "Dx: " << partial(f_ex_11_4_2, xy, 0) << std::endl;
  std::cout << "Dy: " << partial(f_ex_11_4_2, xy, 1) << std::endl;

  xy[0]=1; xy[1]=1;
  std::cout << "r_u: "
            << partial<0>(f_ex_11_4_7, xy, 0) <<" i + "
            << partial<1>(f_ex_11_4_7, xy, 0) <<" j + "
            << partial<2>(f_ex_11_4_7, xy, 0) <<" k "
            << std::endl;

  std::cout << "r_v: "
            << partial<0>(f_ex_11_4_7, xy, 1) <<" i + "
            << partial<1>(f_ex_11_4_7, xy, 1) <<" j + "
            << partial<2>(f_ex_11_4_7, xy, 1) <<" k "
            << std::endl;
  {
    glm::dvec3 norm = unit_normal(f_ex_11_4_7, 1, 1);
    std::cout << "11.4 ex7)\n";
    std::cout << "[" << norm.x
              << " " << norm.y
              << " " << norm.z
              << "]" << std::endl;
  }

  xy[0] =0;
  std::cout << "Dz: " << partial<2>(f_ex_11_5_1, xy, 0) << "\n";


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

  double v_ex_11_6_8[] = {-2, 1, -3};
  grad = gradiant(f_ex_11_6_8, v_ex_11_6_8, 3);
  std::cout << "11.6 ex 8) \n";
  std::cout << "[" << grad[0] << " " << grad[1] << " "  << grad[2] << "]" << std::endl;
  // return 0;

  double d_xyz[] = {1, 2, -1};
  double u_xyz[3];
  v_2_u<3>(d_xyz, u_xyz);
  std::cout << directional_derivative(f_xyz, v_xyz, u_xyz, 3)
  << std::endl;

  double p = M_PI/2;
  std::shared_ptr<double[]> r_prime
      = derivative(f_parametric, p, 3);
  std::cout << "[" << r_prime[0]
            << " " << r_prime[1]
            << " "  << r_prime[2]
            << "]" << std::endl;

  std::shared_ptr<double[]> r_prime_prime
      = second_derivative(f_parametric, p, 3);

  std::cout << "[" << r_prime_prime[0]
            << " " << r_prime_prime[1]
            << " "  << r_prime_prime[2]
            << "]" << std::endl;

  std::shared_ptr<float[]> T
      = get_tanget_v(v_f_parametric, p, 3);
  std::cout << "[" << T[0]
            << " " << T[1]
            << " "  << T[2]
            << "]" << std::endl;

  std::shared_ptr<float[]> N
      = get_normal_v(v_f_parametric, p, 3);
  std::cout << "[" << N[0]
            << " " << N[1]
            << " "  << N[2]
            << "]" << std::endl;

  std::shared_ptr<float[]> B
      = get_binormal_v(&T[0], &N[0]);
  std::cout << "[" << B[0]
            << " " << B[1]
            << " "  << B[2]
            << "]" << std::endl;

  return 0;
}
