#!/usr/bin/env bash
set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
cd "$DIR"

VFC=$1
OUTPUT_DIR="$DIR/output"

# Need to output to this dir since we want tool to create output dir.
"$VFC" -o "$OUTPUT_DIR" < "$DIR/input.json" > "$DIR/run-output.json"

# / -> \/
SED_PATH="`echo "$OUTPUT_DIR" | sed 's/\//\\\\\//g'`"
sed "s/\$OUTPUT_DIR/$SED_PATH/g" "$DIR/output.json" > "$OUTPUT_DIR/expected-output.json"
cmp "$OUTPUT_DIR/expected-output.json" "$DIR/run-output.json"
cmp "$DIR/output.vertices.dat" "$OUTPUT_DIR/vertices.dat"
cmp "$DIR/output.indices.dat" "$OUTPUT_DIR/indices.0.dat"
