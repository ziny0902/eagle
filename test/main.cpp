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

using namespace MathHelper;

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
