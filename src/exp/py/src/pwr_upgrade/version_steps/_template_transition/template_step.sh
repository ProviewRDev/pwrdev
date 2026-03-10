#!/bin/sh
#
# Template shell runner for a version transition.
#
# If scope is `per_volume`, the current volume name is passed as $1.
# For `once`, no volume argument is supplied.

set -eu

volume="${1:-}"

if [ -n "$volume" ]; then
  echo "Template shell step for volume: $volume"
else
  echo "Template shell step running once"
fi

# Example:
# some_tool --project "$pwrp_root" --volume "$volume"
