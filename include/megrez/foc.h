#ifndef __MEGREZ_FOC_H__
#define __MEGREZ_FOC_H__

#include <cmath>

#include "megrez/utils.h"

namespace megrez {
class FOC {
 public:
  /**
   * 帕克逆变换
   *
   * @param e 电机转子位置（电角度）
   * @param Uq 电机转子磁场定子坐标系的电压
   * @param[out] Ualpha 电机转子磁场直角坐标系的a相电压
   * @param[out] Ubeta 电机转子磁场直角坐标系的b相电压
   */
  static inline void ParkRev(float e, float Uq, float& Ualpha, float& Ubeta) {
    Ualpha = -Uq * sin(e);
    Ubeta = Uq * cos(e);
  }

  /**
   * 克拉克逆变换
   *
   * @param Ualpha 电机转子磁场直角坐标系的a相电压
   * @param Ubeta 电机转子磁场直角坐标系的b相电压
   * @param[out] Ua 电机三相直角坐标系的a相电压
   * @param[out] Ub 电机三相直角坐标系的b相电压
   * @param[out] Uc 电机三相直角坐标系的c相电压
   */
  static void ClarkeRev(float Ualpha, float Ubeta, float& Ua, float& Ub,
                        float& Uc) {
    static const float SQRT_3_2 = sqrt(3.0) / 2.0;

    Ua = Ualpha;
    Ub = SQRT_3_2 * Ubeta - 0.5 * Ualpha;
    Uc = -SQRT_3_2 * Ubeta - 0.5 * Ualpha;
  }
};
}  // namespace megrez

#endif