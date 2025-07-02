add_executable(confie)
target_sources(confie PRIVATE
  cli.hpp cli.cpp
  config.hpp config.cpp
  error.hpp error.cpp
  group.hpp group.cpp
  toml.hpp toml.cpp
  yaml.hpp yaml.cpp
  main.cpp
)

# find_package(PkgConfig CONFIG REQUIRED)
# pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)

find_package(spdlog CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

target_link_libraries(confie PRIVATE
  tomlplusplus
  spdlog::spdlog_header_only
  CLI11::CLI11
)

target_compile_definitions(confie PRIVATE TOML_EXCEPTIONS=0 TOML_IMPLEMENTATION=1)
