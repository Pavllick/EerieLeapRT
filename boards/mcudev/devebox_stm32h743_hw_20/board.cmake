# SPDX-License-Identifier: Apache-2.0

board_runner_args(stm32cubeprogrammer "--port=swd" "--reset-mode=hw")
include(${ZEPHYR_BASE}/boards/common/stm32cubeprogrammer.board.cmake)

board_runner_args(dfu-util "--pid=0483:df11" "--alt=0" "--dfuse")
include(${ZEPHYR_BASE}/boards/common/dfu-util.board.cmake)

board_runner_args(openocd --target-handle=_CHIPNAME.cpu0)
include(${ZEPHYR_BASE}/boards/common/openocd.board.cmake)

board_runner_args(jlink "--device=STM32H743VI" "--speed=4000")
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
