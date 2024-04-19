# Autor - https://community.platformio.org/u/awbmilne
# Source - https://community.platformio.org/t/how-to-build-got-revision-into-binary-for-version-output/15380/5

import subprocess

Import("env")

def get_firmware_specifier_build_flag():
    ret = subprocess.run(["git", "describe", "--tags"], stdout=subprocess.PIPE, text=True)
    build_version = ret.stdout.strip()
    build_flag = "-D AUTO_VERSION=\\\"" + build_version + "\\\""
    print ("Firmware Revision: " + build_version)
    return (build_flag)

env.Append(
    BUILD_FLAGS=[get_firmware_specifier_build_flag()]
)