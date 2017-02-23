# Collection of test script helpers. Meant to be sourced, not run directly.

# Establish color directives if the terminal supports it.
if [ $(tput colors) -ge 8 ]; then
    RED="$(tput setaf 1)"
    GREEN="$(tput setaf 2)"
    DEFAULT="$(tput sgr0)"
fi

# try() runs the full argument list as a command, capturing stdout/stderr.
# If the command fails (does not return 0) the output is printed along with
# the command line and the return code.
try() {
    output=$("$@" 2>&1)
    local rc=$?
    if [ $rc -ne 0 ]; then
        echo "($@) returned $rc, output:"
        echo "$output"
    fi
    return $rc
}
