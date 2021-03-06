#!/bin/bash
PROJECT="pendulum"
BUILDDIR="build_linux_qt4.8_gcc"
SUFFIX="_qt4.8_$(gcc -dumpmachine)"

QTDIR="/opt/qt-4.8.7-static"
CMD_QMAKE="${QTDIR}/bin/qmake"

cd "$(dirname $0)"/..
rm -rf "${BUILDDIR}"
mkdir -p "${BUILDDIR}"
cd "${BUILDDIR}"
${CMD_QMAKE} CONFIG+="release" CONFIG+="use_static_qico" "../${PROJECT}.pro"
make
strip --strip-all "${PROJECT}"
cp -a "${PROJECT}" ../"${PROJECT}${SUFFIX}.elf"

