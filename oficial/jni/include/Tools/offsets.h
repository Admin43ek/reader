// Centralized UE offsets and matrix connection chain
// Keep version notes if these change between game updates.
#ifndef ZK_OFFSETS_H
#define ZK_OFFSETS_H

#include <cstdint>

namespace zk_offsets {
    // UE4 global world pointer (libUE4 base + kUWorld)
    static constexpr uintptr_t kUWorld = 0x0EA54A70; // libUE4 + 0xea54a70
    // World -> +0xC0 -> matrix holder, then +0x9D0 -> view/projection matrix
    static constexpr uintptr_t kWorldToMat_C0 = 0xC0;
    static constexpr uintptr_t kMat_Proj      = 0x9D0;
}

#endif // ZK_OFFSETS_H

