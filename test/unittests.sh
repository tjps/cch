#!/bin/bash
# Run all of the unit test binaries.
#
# Returns the number of failure cases (0 on success).

. $(dirname $0)/common.sh

failure_count=0

# Cross-platform way to find executables under a given directory.
find_executables() {
    for i in $1/*; do
        [ -x "$i" ] && echo "$i"
    done
}

echo "Running unit tests"
for test_binary in $(find_executables build/test); do
    try $test_binary
    if [ $? -eq 0 ]; then
        printf "[${GREEN}OK${DEFAULT}]     "
    else
        ((failure_count++))
        printf "[${RED}FAILED${DEFAULT}] "
    fi
    echo "$test_binary"
done

[ $failure_count -eq 0 ] || echo "${RED}ERROR:   $failure_count failures${DEFAULT}"

exit $failure_count
