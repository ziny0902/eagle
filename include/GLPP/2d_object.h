#pragma once

#include <chrono>
#include <vector>
#include <boost/units/quantity.hpp>
#include <boost/units/systems/si.hpp>
#include <boost/units/systems/si/prefixes.hpp>
#include <boost/units/systems/si/length.hpp>
#include <boost/units/systems/si/io.hpp>
#include <GLPP/shader.h>

namespace Gl {

using namespace boost::units;
using namespace boost::units::si;
using namespace std::chrono;

class Object2D 
{
public:
  Object2D(){
    m_t0 = steady_clock::now();
    m_is_moving = false; 
    m_offset = 0;
  }

  void StartMove() 
  { 
    m_t0 = steady_clock::now();
    m_is_moving = true; 
    Get_x(m_t0);
    Get_y(m_t0);
  }
  void StopMove()
  {
    m_is_moving = false;
  }

  inline bool Is_moving(void) { return m_is_moving; }
  inline void SetOffset(long offset) { m_offset = offset; }
  inline long GetOffset() { return m_offset; }

  virtual quantity<si::length> Get_y( steady_clock::time_point &t_c) = 0; 
  virtual quantity<si::length> Get_x( steady_clock::time_point &t_c) = 0; 
  virtual void Update(steady_clock::time_point &t_c, Gl::Shader &shader) = 0; 

  inline std::vector<float>& GetObject(){return m_obj;}

protected:
  steady_clock::time_point m_t0;
  quantity<si::length> m_x;
  quantity<si::length> m_y;
  long m_offset;

private:
  std::vector<float> m_obj;
  bool m_is_moving;
};

}
