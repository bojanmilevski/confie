#!/bin/sh

set -e

PRESET="debug"
EXAMPLE="${1:-config.toml}"
LOG_LEVEL="${2:-trace}"

[ ! -L "./compile_commands.json" ] && ln -s "./build/$PRESET/compile_commands.json" "./compile_commands.json"
cmake --preset "$PRESET"
cmake --build --preset "$PRESET"
./build/$PRESET/src/confie -c "./examples/$EXAMPLE" -l "$LOG_LEVEL"
