#pragma once

#include "cli.hpp"
#include "error.hpp"
#include "group.hpp"
#include <set>

namespace confie {
struct Config {
public:
  Config() = delete;
  Config& operator=(const Config&) = delete;
  [[nodiscard]] static auto create(Cli&&) noexcept -> const expected<Config>;
  auto print() const& noexcept -> const void;

private:
  std::set<Group> m_groups;
  [[nodiscard]] explicit Config(std::set<Group>&&) noexcept;
};
} // namespace confie
