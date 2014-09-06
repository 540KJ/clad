//--------------------------------------------------------------------*- C++ -*-
// clad - the C++ Clang-based Automatic Differentiator
// version: $Id$
// author:  Vassil Vassilev <vvasilev-at-cern.ch>
//------------------------------------------------------------------------------

#ifndef CLAD_BUILTIN_DERIVATIVES
#define CLAD_BUILTIN_DERIVATIVES

// Avoid assertion custom_derivative namespace not found. FIXME: This in future
// should go.
namespace custom_derivatives{}

#include <math.h>

namespace custom_derivatives {
  // define functions' derivatives from std
  namespace std {
    // There are 4 overloads:
    // float       sin( float arg );
    // double      sin( double arg );
    // long double sin( long double arg );
    // double      sin( Integral arg ); (since C++11)
    template<typename R, typename A> R sin(A x) {
      return (R)1;
      //return (R)::std::cos((A)x);
    }

    // There are 4 overloads:
    // float       cos( float arg );
    // double      cos( double arg );
    // long double cos( long double arg );
    // double      cos( Integral arg ); (since C++11)
    template<typename R, typename A> R cos(A x) {
      return (R)1;
      //return (R)-::std::sin((A)x);
    }

    // There are 4 overloads:
    // float       sqrt( float arg );
    // double      sqrt( double arg );
    // long double sqrt( long double arg );
    // double      sqrt( Integral arg ); (since C++11)
    //template<typename R, typename A> R sqrt(A x) {
    //  return (R)(((A)1)/(2*((R)std::sqrt((A)x))));
    //}
  }// end namespace std

  template<typename T>
  T exp_dx(T x) {
    return exp(x);
  }

  template<typename T>
  T sin_dx(T x) {
    return cos(x);
  }

  template<typename T>
  T sin_dy(T y) {
    return cos(y);
  }

  template<typename T>
  T cos_dx(T x) {
    return (-1) * sin(x);
  }

  template<typename T>
  T cos_dy(T y) {
    return (-1) * sin(y);
  }

  template<typename T>
  T sqrt_dx(T x) {
     return ((T)1)/(((T)2)*sqrt(x));
  }
  template<typename T>
  T sqrt_dy(T y) {
     return ((T)1)/(((T)2)*sqrt(y));
  }
  template<typename T>
  T sqrt_dz(T z) {
     return ((T)1)/(((T)2)*sqrt(z));
  }
#ifdef MACOS
  float sqrtf_dx(float x) {
    return 1.F/(2.F*sqrtf(x));
  }
  float sqrtf_dy(float y) {
    return 1.F/(2.F*sqrtf(y));
  }
  float sqrtf_dz(float z) {
    return 1.F/(2.F*sqrtf(z));
  }
#endif

  template<typename T>
  T pow_dx(T x, T exponent) {
    return exponent * pow(x, exponent-((T)1));
  }

  template<typename T>
  T pow_dy(T y, T exponent) {
    return exponent * pow(y, exponent-((T)1));
  }

  template<typename T>
  T pow_dz(T z, T exponent) {
    return exponent * pow(z, exponent-((T)1));
  }

} // end namespace builtin_derivatives

#endif //CLAD_BUILTIN_DERIVATIVES
