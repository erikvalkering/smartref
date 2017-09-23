#include <reflection/reflection.h>

REFLECTABLE(auto, operator[], member_function);
REFLECTABLE(auto, operator=,  member_function);
REFLECTABLE(auto, operator==, member_function);
REFLECTABLE(auto, operator!=, member_function);
REFLECTABLE(auto, operator<,  member_function);
REFLECTABLE(auto, operator<=, member_function);
REFLECTABLE(auto, operator>,  member_function);
REFLECTABLE(auto, operator>=, member_function);
REFLECTABLE(auto, operator>>, member_function);
REFLECTABLE(auto, operator<<, member_function);
