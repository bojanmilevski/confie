add_executable(confie)
target_sources(confie PRIVATE main.cpp error.hpp error.cpp group.hpp group.cpp config.hpp config.cpp)

# find_package(PkgConfig REQUIRED)
# pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)
target_link_libraries(confie PRIVATE tomlplusplus)
target_compile_definitions(confie PRIVATE TOML_EXCEPTIONS=0 TOML_IMPLEMENTATION=1)
