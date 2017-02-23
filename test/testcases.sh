#!/bin/bash
# Runs a series of reference CCH files through the CCH parser and diffs the
# resulting .cc and .h files against their known good versions.
#
# Note that the diff is done in a mostly whitespace-agnostic way.
#
# Returns the number of failure cases (0 on success).

# If '-d' is specified, set debug mode.
if [ "$1" == "-d" ]; then
    debug="-d"
    shift
fi

. $(dirname $0)/common.sh

# Set up temporary directory and a hook to remove it on exit.
tmp=$(mktemp -d 2>/dev/null || mktemp -d -t cch)
cleanup() {
    if [ -z "$debug" ]; then
        rm -rf "$tmp"
    else
        echo "Debug mode - not cleaning up temp dir: $tmp"
    fi
}
trap cleanup EXIT

CCH="build/cch --noBanner --noLineNumbers"
failure_count=0
tests="$@"

if [ -z "$tests" ]; then
    tests=test/cases/*.cch
fi

echo "Running .cch file test cases"
for test_case in $tests; do
    test_name=${test_case##*/}
    successful_diff_count=0
    try $CCH --input "$test_case" --output "$tmp/%f"
    if [ $? -eq 0 ]; then
        for ext in .h .cc; do
            # for resetting the truth:
            ## cp ${tmp}/${test_name}${ext} ${test_case}${ext}
            try diff --ignore-tab-expansion \
                --ignore-space-change \
                --ignore-blank-lines \
                ${test_case}${ext} ${tmp}/${test_name}${ext}
            if [ $? -eq 0 ]; then
                ((successful_diff_count++))
            else
                break
            fi
        done
    fi
    if [ $successful_diff_count -eq 2 ]; then
        printf "[${GREEN}OK${DEFAULT}]     "
    else
        ((failure_count++))
        printf "[${RED}FAILED${DEFAULT}] "
    fi
    echo "$test_case"
done

[ $failure_count -eq 0 ] || echo "${RED}ERROR:   $failure_count failures${DEFAULT}"

exit $failure_count
