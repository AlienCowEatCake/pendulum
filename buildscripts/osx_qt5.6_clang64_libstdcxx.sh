#!/bin/bash -e
PROJECT=pendulum
BUILDDIR=build_osx_qt5.6_clang64_libstdcxx
APPNAME="Pendulum"
DMGNAME="${PROJECT}_qt5.6_clang64_libstdcxx"
OUT_PATH="."
ENTITLEMENTS_PATH="src/${PROJECT}/resources/platform/macosx/${PROJECT}.entitlements"
APP_CERT="Developer ID Application: Petr Zhigalov (48535TNTA7)"
NOTARIZE_USERNAME="peter.zhigalov@gmail.com"
NOTARIZE_PASSWORD="@keychain:Notarize: ${NOTARIZE_USERNAME}"
NOTARIZE_ASC_PROVIDER="${APP_CERT: -11:10}"
ALL_SDK_VERSIONS="$(xcodebuild -showsdks | grep '\-sdk macosx' | sed 's|.*-sdk macosx||')"
for SDK_VERSION in ${ALL_SDK_VERSIONS} ; do
    SDK_PATH="$(xcode-select -p)/Platforms/MacOSX.platform/Developer/SDKs/MacOSX${SDK_VERSION}.sdk"
    if [[ $(find "${SDK_PATH}/usr/lib" -name 'libstdc++*' -maxdepth 1 | wc -l | xargs) > 0 ]] ; then
        MAC_SDK="macosx${SDK_VERSION}"
    fi
done

QT_PATH="/opt/Qt/5.6.3/clang_64_libstdc++_sdk10.10"
QTPLUGINS_PATH="${QT_PATH}/plugins"
CMD_QMAKE="${QT_PATH}/bin/qmake"
CMD_DEPLOY="${QT_PATH}/bin/macdeployqt"

echo "Using MAC_SDK=${MAC_SDK}"

cd "$(dirname $0)"/..
SOURCE_PATH="${PWD}"
rm -rf "${BUILDDIR}"
mkdir -p "${BUILDDIR}"
cd "${BUILDDIR}"
BUILD_PATH="${PWD}"
${CMD_QMAKE} -r CONFIG+="release" LIBS+=-dead_strip QMAKE_MAC_SDK=${MAC_SDK} "../${PROJECT}.pro"
make -j3
cd "${OUT_PATH}"
plutil -replace LSMinimumSystemVersion -string "10.6" "${APPNAME}.app/Contents/Info.plist"
RES_PATH="${APPNAME}.app/Contents/Resources"
rm -f "${RES_PATH}/empty.lproj"
mkdir -p "${RES_PATH}/en.lproj" "${RES_PATH}/ru.lproj"
PLUGINS_PATH="${APPNAME}.app/Contents/PlugIns"
mkdir -p "${PLUGINS_PATH}/iconengines"
for iconengines_plugin in libqsvgicon.dylib ; do
    cp -a "${QTPLUGINS_PATH}/iconengines/${iconengines_plugin}" "${PLUGINS_PATH}/iconengines/"
done
${CMD_DEPLOY} "${APPNAME}.app" -verbose=2
cd "${BUILD_PATH}"

INSTALL_PATH="${PWD}/install"
ARTIFACTS_PATH="${PWD}/artifacts"
rm -rf "${INSTALL_PATH}" "${ARTIFACTS_PATH}"
mkdir -p "${INSTALL_PATH}" "${ARTIFACTS_PATH}"
mv "${OUT_PATH}/${APPNAME}.app" "${INSTALL_PATH}/"
cd "${INSTALL_PATH}"
ln -s /Applications ./Applications
find "${APPNAME}.app/Contents/PlugIns" -name "*_debug.dylib" -delete
cd "${BUILD_PATH}"

function fix_sdk() {
    if [ -z "${NO_SIGN+x}" ] ; then
        local binary="${1}"
        local sdk="${MAC_SDK:6}"
        for SDK_VERSION in ${ALL_SDK_VERSIONS} ; do
            sdk="${SDK_VERSION}"
        done
        echo "Override LC_VERSION_MIN_MACOSX sdk to ${sdk} in ${binary}"
        vtool -set-version-min "macos" "10.6" "${sdk}" -replace -output "${binary}" "${binary}"
    fi
}
function sign() {
    if [ -z "${NO_SIGN+x}" ] ; then
        local max_retry=10
        local last_retry=$((${max_retry}-1))
        for ((i=0; i<${max_retry}; i++)) ; do
            if /usr/bin/codesign \
                    --sign "${APP_CERT}" \
                    --deep \
                    --force \
                    --timestamp \
                    --options runtime \
                    --entitlements "${SOURCE_PATH}/${ENTITLEMENTS_PATH}" \
                    --verbose \
                    --strict \
                    "${1}" ; then
                if [ ${i} != 0 ] ; then
                    echo "Sign completed at ${i} retry"
                fi
                break
            else
                if [ ${i} != ${last_retry} ] ; then
                    echo "Signing failed, retry ..."
                    sleep 5
                else
                    exit 2
                fi
            fi
        done
    fi
}
function notarize() {
    if [ -z "${NO_SIGN+x}" ] ; then
        /usr/bin/python3 "${SOURCE_PATH}/buildscripts/helpers/MacNotarizer.py" \
            --application "${1}" \
            --primary-bundle-id "${2}" \
            --username "${NOTARIZE_USERNAME}" \
            --password "${NOTARIZE_PASSWORD}" \
            --asc-provider "${NOTARIZE_ASC_PROVIDER}"
    fi
}
find "${INSTALL_PATH}/${APPNAME}.app/Contents/MacOS" -type f -print0 | while IFS= read -r -d '' item ; do fix_sdk "${item}" ; done
find "${INSTALL_PATH}/${APPNAME}.app/Contents/Frameworks" \( -name '*.framework' -or -name '*.dylib' \) -print0 | while IFS= read -r -d '' item ; do sign "${item}" ; done
find "${INSTALL_PATH}/${APPNAME}.app/Contents/PlugIns"       -name '*.dylib'                            -print0 | while IFS= read -r -d '' item ; do sign "${item}" ; done
sign "${INSTALL_PATH}/${APPNAME}.app"
notarize "${INSTALL_PATH}/${APPNAME}.app" "$(plutil -extract CFBundleIdentifier xml1 -o - "${INSTALL_PATH}/${APPNAME}.app/Contents/Info.plist" | sed -n 's|.*<string>\(.*\)<\/string>.*|\1|p')"

hdiutil create -format UDBZ -fs HFS+ -srcfolder "${INSTALL_PATH}" -volname "${APPNAME}" "${ARTIFACTS_PATH}/${DMGNAME}.dmg"
sign "${ARTIFACTS_PATH}/${DMGNAME}.dmg"
