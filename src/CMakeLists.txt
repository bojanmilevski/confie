add_executable(confie)
target_sources(confie PUBLIC main.cpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(tomlplusplus REQUIRED IMPORTED_TARGET tomlplusplus)
target_link_libraries(confie PRIVATE PkgConfig::tomlplusplus)
target_compile_definitions(confie PRIVATE TOML_EXCEPTIONS=0 TOML_IMPLEMENTATION=1)
