struct CArm : public Component {
    Vec3 shoulderPos_m = { 0, 0, 0 };
    Vec3 elbowPos_m = { 0, 0, 0 };
    // The wrist position is effectively the Racket position

    float upperArmLength = 0.30f; // 30cm
    float lowerArmLength = 0.35f; // 35cm (includes hand/grip)
    float maxReach_m = 0.65f;     // Total length

    bool isLeftHanded = true;
};