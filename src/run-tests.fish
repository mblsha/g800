#!/usr/bin/env fish

cmake -Bbuild-ninja -DCMAKE_BUILD_TYPE=Release -G Ninja

function build_and_run
    ninja -C build-ninja && ctest --test-dir build-ninja --output-on-failure
end

build_and_run

while fswatch -1 .
    reset
    build_and_run
    sleep 1
end

