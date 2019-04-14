#include <action.h>

template<>
void std::swap(Action& lhs, Action& rhs) noexcept {
 lhs.swap(rhs);
}