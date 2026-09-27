#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
SRC_ROOT="$ROOT_DIR/src"

# ------------------------------------------------------------
# Default exclusions
#
# Directory names relative to src/
# ------------------------------------------------------------

DEFAULT_EXCLUDE_DIRS=(
    unused
    legacy
    template
)

EXCLUDE_DIRS=("${DEFAULT_EXCLUDE_DIRS[@]}")

# ------------------------------------------------------------
# Usage
# ------------------------------------------------------------

usage()
{
    cat <<EOF
Usage:
  $0 [options]
  $0 [options] <target>
  $0 clean [options]
  $0 clean [options] <target>

Options:
  --exclude <name>
      Exclude src/<name> from the operation.
      This option may be specified multiple times.

  -h, --help
      Show this help.

Examples:
  $0
      Build all projects under src/

  $0 clean
      Remove build directories from all projects

  $0 ssd
      Build only src/ssd

  $0 clean ssd
      Remove src/ssd/build

  $0 --exclude ssd
      Build everything except src/ssd

  $0 --exclude ssd --exclude tutorial
      Build everything except src/ssd and src/tutorial

  $0 clean --exclude ssd
      Clean everything except src/ssd
EOF
}

# ------------------------------------------------------------
# Check exclusion
# ------------------------------------------------------------

is_excluded()
{
    local name="$1"
    local excluded

    for excluded in "${EXCLUDE_DIRS[@]}"; do
        if [ "$name" = "$excluded" ]; then
            return 0
        fi
    done

    return 1
}

# ------------------------------------------------------------
# Build one project
# ------------------------------------------------------------

build_project()
{
    local srcdir="$1"
    local name
    local builddir

    name="$(basename "$srcdir")"
    builddir="$srcdir/build"

    echo
    echo "========================================"
    echo " Building: $name"
    echo "========================================"
    echo

    cmake \
        -S "$srcdir" \
        -B "$builddir"

    cmake \
        --build "$builddir" \
        -j
}

# ------------------------------------------------------------
# Clean one project
# ------------------------------------------------------------

clean_project()
{
    local srcdir="$1"
    local name
    local builddir

    name="$(basename "$srcdir")"
    builddir="$srcdir/build"

    if [ -d "$builddir" ]; then
        echo "[clean] $name"
        rm -rf "$builddir"
    else
        echo "[clean] $name: nothing to do"
    fi
}

# ------------------------------------------------------------
# Parse arguments
# ------------------------------------------------------------

MODE="build"
TARGET=""

while [ "$#" -gt 0 ]; do
    case "$1" in
        clean)
            if [ "$MODE" != "build" ]; then
                echo "ERROR: duplicate mode specification." >&2
                exit 1
            fi

            MODE="clean"
            shift
            ;;

        --exclude)
            if [ "$#" -lt 2 ]; then
                echo "ERROR: --exclude requires a directory name." >&2
                exit 1
            fi

            EXCLUDE_DIRS+=("$2")
            shift 2
            ;;

        --exclude=*)
            EXCLUDE_DIRS+=("${1#*=}")
            shift
            ;;

        -h|--help)
            usage
            exit 0
            ;;

        -*)
            echo "ERROR: unknown option: $1" >&2
            echo >&2
            usage >&2
            exit 1
            ;;

        *)
            if [ -n "$TARGET" ]; then
                echo "ERROR: multiple targets specified:" >&2
                echo "  $TARGET" >&2
                echo "  $1" >&2
                exit 1
            fi

            TARGET="$1"
            shift
            ;;
    esac
done

# ------------------------------------------------------------
# Check src/
# ------------------------------------------------------------

if [ ! -d "$SRC_ROOT" ]; then
    echo "ERROR: source directory does not exist:" >&2
    echo "  $SRC_ROOT" >&2
    exit 1
fi

# ------------------------------------------------------------
# Single target
# ------------------------------------------------------------

if [ -n "$TARGET" ]; then
    srcdir="$SRC_ROOT/$TARGET"

    if [ ! -d "$srcdir" ]; then
        echo "ERROR: target directory does not exist:" >&2
        echo "  $srcdir" >&2
        exit 1
    fi

    if is_excluded "$TARGET"; then
        echo "ERROR: target is excluded:" >&2
        echo "  $TARGET" >&2
        exit 1
    fi

    if [ ! -f "$srcdir/CMakeLists.txt" ]; then
        echo "ERROR: CMakeLists.txt not found:" >&2
        echo "  $srcdir/CMakeLists.txt" >&2
        exit 1
    fi

    case "$MODE" in
        build)
            build_project "$srcdir"
            ;;

        clean)
            clean_project "$srcdir"
            ;;
    esac

    exit 0
fi

# ------------------------------------------------------------
# All targets
# ------------------------------------------------------------

found=0

for srcdir in "$SRC_ROOT"/*; do
    [ -d "$srcdir" ] || continue

    name="$(basename "$srcdir")"

    if is_excluded "$name"; then
        echo "[skip] $name (excluded)"
        continue
    fi

    if [ ! -f "$srcdir/CMakeLists.txt" ]; then
        echo "[skip] $name (no CMakeLists.txt)"
        continue
    fi

    found=1

    case "$MODE" in
        build)
            build_project "$srcdir"
            ;;

        clean)
            clean_project "$srcdir"
            ;;
    esac
done

if [ "$found" -eq 0 ]; then
    echo "No buildable projects found under:"
    echo "  $SRC_ROOT"
fi
