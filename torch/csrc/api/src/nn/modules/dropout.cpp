#include <torch/nn/modules/dropout.h>

namespace torch {
namespace detail {

template <typename T>
DropoutBase<T>::DropoutBase(double rate) : rate_(rate) {
  AT_CHECK(rate >= 0, "Dropout rate must not be less than zero");
  AT_CHECK(rate < 1, "Dropout rate must be less than one");
}

template <typename T>
void DropoutBase<T>::reset() {}

template <typename T>
std::vector<Variable> DropoutBase<T>::forward(std::vector<Variable> input) {
  if (rate_ == 0 || !is_training()) {
    return input;
  }
  std::vector<Variable> output;
  for (const auto& value : input) {
    const auto noise = (noise_mask(value).uniform_(0, 1) > rate_)
                           .toType(value.type().scalarType())
                           .mul_(1.0f / (1.0f - rate_));
    output.push_back(value * noise);
  }
  return output;
}

template class detail::DropoutBase<nn::Dropout>;
template class detail::DropoutBase<nn::Dropout2d>;
} // namespace detail

namespace nn {
Variable Dropout::noise_mask(Variable input) const {
  return at::empty_like(input);
}

Variable Dropout2d::noise_mask(Variable input) const {
  return input.type().empty({input.size(0), input.size(1), 1, 1});
}
} // namespace nn
} // namespace torch
