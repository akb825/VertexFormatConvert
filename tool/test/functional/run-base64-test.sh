#!/usr/bin/env bash
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
cd "$DIR"

VFC=$1

"$VFC" -o "$OUTPUT_DIR" < "$DIR/input-base64.json" > "$DIR/run-output-base64.json"
cmp "$DIR/output-base64.json" "$DIR/run-output-base64.json"
