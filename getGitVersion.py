import subprocess
Import("env")

def get_firmware_specifier_build_flag():
    tag = subprocess.run(
        ["git", "describe", "--tags", "--abbrev=0"],
        stdout=subprocess.PIPE, text=True
    ).stdout.strip()

    commit_count = subprocess.run(
        ["git", "rev-list", "--count", "HEAD"],
        stdout=subprocess.PIPE, text=True
    ).stdout.strip()

    commit_hash = subprocess.run(
        ["git", "rev-parse", "--short", "HEAD"],
        stdout=subprocess.PIPE, text=True
    ).stdout.strip()

    build_version = f"{tag}-{commit_count}-{commit_hash}"
    build_flag = f'-D AUTO_VERSION=\\"{build_version}\\"'

    print("Firmware Revision:", build_version)
    return build_flag

env.Append(
    BUILD_FLAGS=[get_firmware_specifier_build_flag()]
)