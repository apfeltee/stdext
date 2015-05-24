
#pragma once
#include <exception>
#include <stdexcept>

namespace stdext
{
    template<typename ReturnT, typename InputT>
    constexpr ReturnT force_cast(InputT what)
    {
        return *((ReturnT*)&what);
    }
}
