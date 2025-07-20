#include <actuators/BaseGear.h>
#include <sensors/MagneticEncoder.h>

BaseGear::BaseGear() {}
BaseGear::~BaseGear() {}

void BaseGear::write(int angle) {
    // TODO: implement feedback loop with magnetic encoder
}