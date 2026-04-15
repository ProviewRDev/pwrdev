#!/bin/bash
#
# format-all.sh — Run clang-format on all C/C++ source files in the repo.
# Preserves ISO 8859-1 encoding for files that use it.
#
# Usage:
#   ./format-all.sh          Format all files
#   ./format-all.sh --check  Dry-run: report files that would change (exit 1 if any)
#
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$REPO_ROOT"

CHECK_MODE=0
if [[ "${1:-}" == "--check" ]]; then
    CHECK_MODE=1
fi

DIRTY=0
FORMATTED=0
SKIPPED=0

# Third-party / generated files to skip (glob patterns relative to repo root)
EXCLUDE_PATTERNS=(
    "*/stdsoap2.*"                       # gSOAP runtime (multiple copies)
    "opc/lib/opc/src/opc_soap_*"         # gSOAP generated stubs
)

should_skip() {
    local file="$1"
    for pat in "${EXCLUDE_PATTERNS[@]}"; do
        # shellcheck disable=SC2254
        case "$file" in $pat) return 0 ;; esac
    done
    return 1
}

# Collect all C/C++ source files tracked by git
while IFS= read -r -d '' f; do
    if should_skip "$f"; then
        (( SKIPPED++ )) || true
        continue
    fi
    # Detect encoding (look for "ISO-8859" in file(1) output)
    enc=$(LANG=C LC_ALL=C file "$f")
    is_latin1=0
    if [[ "$enc" == *"ISO-8859"* ]]; then
        is_latin1=1
    fi

    if (( is_latin1 )); then
        # ISO 8859-1 file: convert to UTF-8, format, convert back
        tmp_utf8=$(mktemp)
        tmp_formatted=$(mktemp)
        trap 'rm -f "$tmp_utf8" "$tmp_formatted"' EXIT

        iconv -f ISO-8859-1 -t UTF-8 "$f" > "$tmp_utf8"

        if (( CHECK_MODE )); then
            clang-format --style=file "$tmp_utf8" > "$tmp_formatted"
            if ! cmp -s "$tmp_utf8" "$tmp_formatted"; then
                echo "[needs format] $f  (ISO-8859-1)"
                DIRTY=1
            fi
        else
            clang-format --style=file -i "$tmp_utf8"
            iconv -f UTF-8 -t ISO-8859-1 "$tmp_utf8" > "$tmp_formatted"
            # Only write back if content actually changed
            if ! cmp -s "$f" "$tmp_formatted"; then
                cp "$tmp_formatted" "$f"
                echo "[formatted] $f  (ISO-8859-1)"
            fi
        fi

        rm -f "$tmp_utf8" "$tmp_formatted"
        trap - EXIT
    else
        # UTF-8 / ASCII file: format in place directly
        if (( CHECK_MODE )); then
            if ! clang-format --style=file --dry-run --Werror "$f" >/dev/null 2>&1; then
                echo "[needs format] $f"
                DIRTY=1
            fi
        else
            before=$(md5sum "$f" | cut -d' ' -f1)
            clang-format --style=file -i "$f"
            after=$(md5sum "$f" | cut -d' ' -f1)
            if [[ "$before" != "$after" ]]; then
                echo "[formatted] $f"
            fi
        fi
    fi

    (( FORMATTED++ )) || true
done < <(git ls-files -z -- '*.c' '*.cpp' '*.h')

echo ""
echo "Processed $FORMATTED files, skipped $SKIPPED third-party files."
if (( CHECK_MODE )); then
    if (( DIRTY )); then
        echo "Some files need formatting."
        exit 1
    else
        echo "All files are properly formatted."
    fi
fi
