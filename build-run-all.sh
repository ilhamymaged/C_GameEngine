#!/usr/bin/env bash

# Build And Run Everything

set -e

./build-all.sh

pushd bin > /dev/null

./skyBoxGame

popd > /dev/null

echo "Build and run completed successfully."