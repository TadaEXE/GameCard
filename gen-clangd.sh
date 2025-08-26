#!/usr/bin/env bash
set -euo pipefail

# Generate a .clangd that points clangd at the xPack ARM GCC C/C++ stdlib.
#
# Usage:
#   ./gen-clangd.sh [--root ~/xpack-arm-none-eabi-gcc-14.2.1-1.1] [--cpu cortex-m4] [--out .clangd]
# Environment defaults:
#   XPACK_ROOT=~/xpack-arm-none-eabi-gcc-14.2.1-1.1
# Notes:
#   - Requires the toolchain's g++ (either in PATH as arm-none-eabi-g++, or at $XPACK_ROOT/bin/arm-none-eabi-g++).
#   - We parse the compiler's own include search list to stay version-accurate.

XPACK_ROOT="${XPACK_ROOT:-$HOME/xpack-arm-none-eabi-gcc-14.2.1-1.1}"
CPU="cortex-m0plus"
OUT=".clangd"
FLOAT_ABI="soft"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --root) XPACK_ROOT="$2"; shift 2 ;;
    --cpu)  CPU="$2"; shift 2 ;;
    --out)  OUT="$2"; shift 2 ;;
    -h|--help)
      echo "Usage: $0 [--root <xpack-root>] [--cpu <cortex-mX>] [--out <path>]"
      exit 0
      ;;
    *)
      echo "Unknown arg: $1" >&2; exit 1 ;;
  esac
done

# Locate the compiler
if [[ -x "$XPACK_ROOT/bin/arm-none-eabi-g++" ]]; then
  CXX="$XPACK_ROOT/bin/arm-none-eabi-g++"
else
  CXX="$(command -v arm-none-eabi-g++ || true)"
fi
if [[ -z "${CXX}" ]]; then
  echo "arm-none-eabi-g++ not found. Install xPack or add it to PATH, or set --root." >&2
  exit 1
fi

# Ask the compiler for its include search paths for C++.
# We also force -nostdinc++ to see exactly where libstdc++ lives via sysroot,
# then add those directories explicitly to clangd.
VERBOSE_OUT="$("$CXX" -E -x c++ - -v </dev/null 2>&1 || true)"

# Extract include paths between the markers.
mapfile -t INC_PATHS < <(printf "%s\n" "$VERBOSE_OUT" \
  | awk '
      $0 ~ /^#include <\.\.\.> search starts here:/ { capture=1; next }
      $0 ~ /^End of search list\./ { capture=0 }
      capture { gsub(/^[[:space:]]+/, "", $0); print $0 }
    ' \
  | sed 's/ (framework directory)//' \
  | sed 's/ (framework directory, unused)//' \
  | sed 's/ (device)//' \
)

# De-duplicate while preserving order
uniq_paths=()
seen="|"
for p in "${INC_PATHS[@]}"; do
  [[ -d "$p" ]] || continue
  if [[ "$seen" != *"|$p|"* ]]; then
    uniq_paths+=("$p")
    seen+="$p|"
  fi
done

# Sanity check: can we find <utility> in any include dir?
found_utility="no"
for p in "${uniq_paths[@]}"; do
  if [[ -f "$p/utility" ]]; then
    found_utility="yes"
    break
  fi
done

# If not found, try common xPack libstdc++ locations (helps on some distros/editors).
if [[ "$found_utility" == "no" ]]; then
  guess1="$XPACK_ROOT/arm-none-eabi/include/c++"
  if [[ -d "$guess1" ]]; then
    # Pick the highest version dir
    ver_dir="$(ls -1 "$guess1" | sort -V | tail -n1 || true)"
    if [[ -n "$ver_dir" && -d "$guess1/$ver_dir" ]]; then
      extra_candidates=(
        "$guess1/$ver_dir"
        "$guess1/$ver_dir/arm-none-eabi"
      )
      for p in "${extra_candidates[@]}"; do
        if [[ -d "$p" ]]; then
          uniq_paths+=("$p")
        fi
      done
      # Re-check utility
      for p in "${extra_candidates[@]}"; do
        if [[ -f "$p/utility" ]]; then
          found_utility="yes"
          break
        fi
      done
    fi
  fi
fi

# Build YAML with quoted -I flags
yaml_includes=()
for p in "${uniq_paths[@]}"; do
  yaml_includes+=("\"-I${p}\"")
done

# Write the .clangd
cat > "$OUT" <<EOF
CompileFlags:
  Add: [
    "-target", "arm-none-eabi",
    "-mcpu=${CPU}",
    "-mfloat-abi=${FLOAT_ABI}",
    "-mthumb",
    "-nostdinc++",
$(printf "    %s\n" "$(IFS=,; echo "${yaml_includes[*]}")")
  ]
EOF

echo "Wrote $OUT"
# if [[ "$found_utility" == "yes" ]]; then
#   echo "✓ Passed sanity check: Found <utility> in the discovered include paths."
# else
#   echo "⚠ Failed sanity check: Did not see <utility> directly. If clangd still complains, check libstdc++ version dirs under:"
#   echo "   $XPACK_ROOT/arm-none-eabi/include/c++"
# fi

