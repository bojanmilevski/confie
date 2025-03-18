#!/bin/sh

set -e

PRESET="debug"

[ ! -L "./compile_commands.json" ] && ln -s "./build/$PRESET/compile_commands.json" "./compile_commands.json"
cmake --preset "$PRESET"
cmake --build --preset "$PRESET"
./build/$PRESET/src/confie $@
