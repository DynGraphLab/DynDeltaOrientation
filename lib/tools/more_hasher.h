//
// Created by Jannick Borowitz on 06.06.22.
//

#ifndef DELTA_ORIENTATIONS_MORE_HASHER_H
#define DELTA_ORIENTATIONS_MORE_HASHER_H

#include <functional>
#include <definitions.h>

// https://en.cppreference.com/w/cpp/utility/hash
template<>
struct std::hash<std::pair<NodeID, NodeID>>
{
    std::size_t operator()(const std::pair<NodeID, NodeID>& pair) const noexcept
    {
        std::size_t h1 = std::hash<NodeID>{}(pair.first);
        std::size_t h2 = std::hash<NodeID>{}(pair.second);
        return h1 ^ (h2 << 1);
    }
};

#endif  // DELTA_ORIENTATIONS_MORE_HASHER_H
