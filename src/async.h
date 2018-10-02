#pragma once

#include <cstddef>
#include <unordered_map>
#include "bulk.hpp"

namespace async
{
    const size_t MAX_CONNECTIONS = 100;
    using handle_t = int;

    handle_t
    connect     ( std::size_t bulk );

    void
    receive     ( handle_t handle, const char *data, std::size_t size );

    void
    disconnect  ( handle_t handle );
}
