#include "error.hpp"
#include <expected>
#include <string_view>

namespace confie {
Error::Error(const ErrorType&& error_type, const std::string_view&& message) noexcept
    : m_error_type{std::move(error_type)}, m_message{std::move(message)} {}

auto Error::create(const ErrorType&& error_type, const std::string_view&& message) noexcept
    -> const std::unexpected<Error> {
  return std::unexpected(Error(std::move(error_type), std::move(message)));
}

auto Error::get_message() const& noexcept -> const std::string_view& { return m_message; }

auto Error::get_error() const& noexcept -> const ErrorType& { return m_error_type; }
} // namespace confie
