#!/bin/bash
# Runs a series of reference CCH files through the CCH parser and diffs the
# resulting .cc and .h files against their known good versions.
#
# Note that the diff is done in a mostly whitespace-agnostic way.
#
# Returns the number of failure cases (0 on success).

debug=""

# Set up temporary directory and a hook to remove it on exit.
tmp=$(mktemp -d 2>/dev/null || mktemp -d -t cch)
function cleanup {
    if [ -z "$debug" ]; then
        rm -rf "$tmp"
    else
        echo "Debug mode. Not cleaning up temp dir: $tmp"
    fi
}
trap cleanup EXIT

# If '-d' is specified, set debug mode.
if [ "$1" == "-d" ]; then
    debug="-d"
    shift
fi

# try() runs the full argument list as a command, capturing stdout/stderr.
# If the command fails (does not return 0) the output is printed along with
# the command line and the return code.
function try {
    output=$("$@" 2>&1)
    local rc=$?
    if [ $rc -ne 0 ]; then
        echo "($@) returned $rc, output:"
        echo "$output"
    fi
    return $rc
}

# Establish color directives if the terminal supports it.
if [ $(tput colors) -ge 8 ]; then
    red="$(tput setaf 1)"
    green="$(tput setaf 2)"
    default="$(tput sgr0)"
fi

CCH="build/cch --nobanner"
failure_count=0
tests="$@"

if [ -z "$tests" ]; then
    tests=tests/*.cch
fi

for test_case in $tests; do
    test_name=${test_case##*/}
    successful_diff_count=0
    try $CCH --input "$test_case" --output "$tmp"
    if [ $? -eq 0 ]; then
        for ext in .h .cc; do
            # for resetting the truth: cp  ${tmp}/${test_name}${ext} ${test_case}${ext}
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
        echo -n "[${green}OK${default}]     "
    else
        ((failure_count++))
        echo -n "[${red}FAILED${default}] "
    fi
    echo "$test_case"
done


exit $failure_count
