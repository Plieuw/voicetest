#!/bin/sh
# Build the pinned, unmodified upstream Felucca baseline. This does not flash a device.
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
BUILD_ROOT="${ROOT}/build/upstream"
FELUCCA_DIR="${BUILD_ROOT}/Felucca"
FELUCCA_COMMIT="a4f007c9211ea5b37298c01ff6d73430fe2cb96a"

mkdir -p "${BUILD_ROOT}"

if [ ! -d "${FELUCCA_DIR}/.git" ]; then
    git clone https://github.com/hugelton/Felucca.git "${FELUCCA_DIR}"
fi

git -C "${FELUCCA_DIR}" fetch --tags origin
git -C "${FELUCCA_DIR}" checkout --detach "${FELUCCA_COMMIT}"
git -C "${FELUCCA_DIR}" submodule sync --recursive
git -C "${FELUCCA_DIR}" submodule update --init --recursive

EXPECTED_BSP="5942e68b89ab9ccd23f2c541d712ca38fe635f11"
EXPECTED_ENGINES="d0260074ce100a47675814b7f38d61f11176c926"
ACTUAL_BSP=$(git -C "${FELUCCA_DIR}/firmware/bsp" rev-parse HEAD)
ACTUAL_ENGINES=$(git -C "${FELUCCA_DIR}/firmware/engines" rev-parse HEAD)

if [ "${ACTUAL_BSP}" != "${EXPECTED_BSP}" ]; then
    echo "Unexpected felucca-bsp revision: ${ACTUAL_BSP}" >&2
    exit 1
fi
if [ "${ACTUAL_ENGINES}" != "${EXPECTED_ENGINES}" ]; then
    echo "Unexpected Felucca-Engines revision: ${ACTUAL_ENGINES}" >&2
    exit 1
fi

cd "${FELUCCA_DIR}"
exec ./build.sh "$@"
