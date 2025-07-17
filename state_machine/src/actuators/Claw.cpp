#include <Claw.h>

Claw::Claw(Servo *servo) {
    this->servo = servo;
}

void Claw::open() {
    servo->write(CLAW_OPEN);
}

void Claw::close() {
    servo->write(CLAW_CLOSED);
}

Claw::~Claw() {}