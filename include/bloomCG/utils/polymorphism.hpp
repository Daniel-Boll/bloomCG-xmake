#pragma once

// instance of where it gets many base classes if any of them are true (||)
template <typename... Base, typename T> inline bool instanceof (const T* ptr) {
  return (... || dynamic_cast<const Base*>(ptr));
}
