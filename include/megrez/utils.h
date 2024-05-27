#ifndef __MEGREZ_UTILS_H__
#define __MEGREZ_UTILS_H__

#include <cmath>

namespace megrez {

namespace utils {

inline float Constrain(float amt, float low, float high) {
  return ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)));
}

inline float NormalizeAngle(float angle) {
  static const float _2PI = 2 * M_PI;

  float a = fmod(angle, _2PI);
  return a >= 0 ? a : (a + _2PI);
}

}  // namespace utils
}  // namespace megrez

#endif