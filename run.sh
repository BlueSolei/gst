SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
pushd $SCRIPT_DIR
cd build/bin
if [[ "$1" != "tests" ]]; then
    ./gst
else
    ./gstTest
fi
popd
