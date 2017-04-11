#pragma once

#include <memory>

namespace happyntrain {

template <typename Object>
using Ref = std::shared_ptr<Object>;

template <typename Object>
using Ptr = std::unique_ptr<Object>;
}
