from datetime import datetime

Import("env")

def get_firmware_specifier_build_flag():
    current_time = datetime.now()
    year = str(current_time.year)
    month = str(current_time.month)
    mday = str(current_time.day)
    wday = str(current_time.weekday())

    hour_str = current_time.strftime("%H")
    minute_str = current_time.strftime("%M")
    second_str = current_time.strftime("%S")

    build_flag = "-D AUTO_YEAR=\\\"" + year + "\\\" -D AUTO_MONTH=\\\"" + month + "\\\" -D AUTO_MDAY=\\\"" + mday + "\\\" " \
        "-D AUTO_WDAY=\\\"" + wday + "\\\" -D AUTO_HOUR=\\\"" + hour_str + "\\\" -D AUTO_MIN=\\\"" + minute_str + "\\\" " \
            "-D AUTO_SEC=\\\"" + second_str + "\\\""

    print(build_flag)

    return (build_flag)

env.Append(
    BUILD_FLAGS=[get_firmware_specifier_build_flag()]
)
