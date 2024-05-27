#include "megrez/motor.h"

#include "megrez/foc.h"

namespace megrez {

Motor::Motor(int pole_pairs, uint32_t period, volatile uint32_t* a,
             volatile uint32_t* b, volatile uint32_t* c)
    : pole_pairs_(pole_pairs),
      half_period_(period * 0.5),
      a_(a),
      b_(b),
      c_(c) {}

void Motor::SetVoltage(float voltage, float theta) {
  float e = utils::NormalizeAngle(theta * pole_pairs_);

  float Ualpha, Ubeta;
  FOC::ParkRev(e, voltage, Ualpha, Ubeta);

  float Ua, Ub, Uc;
  FOC::ClarkeRev(Ualpha, Ubeta, Ua, Ub, Uc);

  *a_ = static_cast<uint32_t>((Ua + 1.0) * half_period_);
  *b_ = static_cast<uint32_t>((Ub + 1.0) * half_period_);
  *c_ = static_cast<uint32_t>((Uc + 1.0) * half_period_);
}

}  // namespace megrez