# SPDX-License-Identifier: Apache-2.0

# board_runner_args(dfu-util "--pid=0483:df11" "--alt=0" "--dfuse")
# board_runner_args(jlink "--device=STM32H743VI" "--speed=4000")

# include(${ZEPHYR_BASE}/boards/common/dfu-util.board.cmake)
# include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)

board_runner_args(stm32cubeprogrammer "--port=swd" "--reset-mode=hw")
include(${ZEPHYR_BASE}/boards/common/stm32cubeprogrammer.board.cmake)

# Doesn't work as expected
# board_runner_args(stlink_gdbserver "--apid=1")
# include(${ZEPHYR_BASE}/boards/common/stlink_gdbserver.board.cmake)

board_runner_args(openocd "--config=${BOARD_DIR}/support/openocd.cfg")
include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)
