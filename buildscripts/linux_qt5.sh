#!/bin/bash
V_PROJECT=pendulum
V_VERS=0.39
V_BUILDDIR=../build_linux_qt5

LINK="`readlink $0`"
if [[ -z ${LINK} ]]; then
LINK="$0"
fi
DIRNAME="`dirname ${LINK}`"
cd "${DIRNAME}" || exit 1

rm -rf "${V_BUILDDIR}" 2>/dev/null >/dev/null
mkdir "${V_BUILDDIR}" || exit 1
cd "${V_BUILDDIR}"
qmake "../${V_PROJECT}.pro" || exit 1
make -j4 || exit 1
strip --strip-all "${V_PROJECT}" || exit 1
cp "${V_PROJECT}" "../${V_PROJECT}-${V_VERS}_qt5"

cd ..
exit 0

