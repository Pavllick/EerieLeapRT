# Eerie Leap Real-Time

EerieLeap is an open-source sensor monitoring system built with [Zephyr RTOS](https://github.com/zephyrproject-rtos/zephyr). It provides a robust platform for reading, processing, and managing sensor data, supporting both physical ADC sensors and virtual calculated sensors.

## Getting Started

Development environment is based on [Docker](https://www.docker.com/). Use `example.docker-compose.yml` as an example. Rename to `docker-compose.yml` to use it.

### Hardware Access From Container

By default, the container will be run with `privileged` mode enabled and `pid` mode set to `host`, to allow access to devices connected to the host. Device have to be connected to the host before running the container.

If using Docker for Windows with Docker using WSL2 engine, make sure to attach device to WSL container. It can be done with help of [usbipd-win](https://github.com/dorssel/usbipd-win) tool. Example:

```shell
usbipd attach --wsl --busid <busid>
```

Where `<busid>` is the bus id of the device that can be found with `usbipd list` command.

Connected device should be visible in the container as `/dev/ttyACM0`. Test presence of the device in the container with `ls /dev/ttyACM0` command.

### Building

For VS Code build setup examples, refer to `.vscode_example/tasks.json`. Alternatively, you can build the application using the following command:

```shell
west build -b $BOARD app
```

where `$BOARD` is the target board.

Currently, the following boards are supported:

#### Simulated boards:
- `native_sim`
- `qemu_cortex_m3`

#### SoC boards:
- `esp32s3_devkitc_procpu`

### Running and Debugging in a Simulator

Running and debugging in a simulator can be more convenient and time-efficient. To run the compiled application in a simulator, use:

```shell
west build -t run
```

For VS Code debugging setup examples, refer to `.vscode_example/launch.json`.

### Flashing

Once the application is built, flash it to the target board with:

```shell
west flash
```

### Board-Specific Commands

<details>
<summary>
    Native Simulator
    <a href="https://docs.zephyrproject.org/latest/boards/native/native_sim/doc/index.html">(Reference Docs)</a>
</summary>
<br>

**Build:**  
```shell
west build -p auto -b native_sim ./app
```

</details>


<details>
<summary>
    QEMU Cortex M3
    <a href="https://docs.zephyrproject.org/latest/boards/qemu/cortex_m3/doc/index.html">(Reference Docs)</a>
</summary>
<br>

**Build:**  
```shell
west build -p auto -b qemu_cortex_m3 ./app
```

</details>


<details>
<summary>
    ESP32S3 DevKitC v1.3
    <a href="https://docs.zephyrproject.org/latest/boards/espressif/esp32s3_devkitc/doc/index.html">(Reference Docs)</a>
</summary>
<br>

**Build with Bootloader:**  
```shell
west build -p auto -b esp32s3_devkitc/esp32s3/procpu --sysbuild ./app
```

**Simple Build:**  
```shell
west build -p auto -b esp32s3_devkitc/esp32s3/procpu ./app
```

**Serial Monitor:**  
```shell
west espressif monitor
```

</details>

<details>
<summary>
    STM32H7
</summary>
<br>

**Required tools**

STM32CubeProgrammer needs to be installed on the host machine. If using Docker, you can take an advantate of build script expecting to find STM32CubeCLT in `tools/st-stm32cubeclt_1.19.0.sh`, source the tool from [STM Website](https://www.st.com/en/development-tools/stm32cubeclt.html) and unpack it to `tools/st-stm32cubeclt_1.19.0.sh`. Dockerfile will take care of installing it during build process.

**Build**  
```shell
west build -p auto -b mcudev_devebox_stm32h743_hw_20 ./app
```

**Debug**

<a href="https://github.com/Marus/cortex-debug">Cortex-Debug</a> extension for <a href="https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug">VS Code</a> can be used for debugging, example configuration valid for Docker container in combination with ST-Link connected over SWD can be found in `.vscode_example/launch.json`.


</details>

## Development

### Required tools

#### CBOR Schemas

The project uses [zcbor](https://github.com/NordicSemiconductor/zcbor) to generate CBOR helpers from schemas. To install zcbor:

```shell
pip install zcbor
```

#### HTML Minifier

The project uses [html-minifier-terser](https://github.com/terser/html-minifier-terser) to minify HTML files. To install html-minifier-terser:

```shell
npm install -g html-minifier-terser
```

### Generating CBOR helpers

#### Example command to generate SystemConfig helpers:

```shell
zcbor code -sed --default-max-qty 24 -c ./app/src/configuration/cbor_schemas/system_config.cddl -t SystemConfig --output-h ./app/src/configuration/system_config/system_config_cbor.h --output-c ./app/src/configuration/system_config/system_config_cbor.c --output-h-types ./app/src/configuration/system_config/system_config.h
```

#### Example command to generate AdcConfig helpers:

```shell
zcbor code -sed --default-max-qty 24 -c ./app/src/configuration/cbor_schemas/adc_config.cddl -t AdcConfig --output-h ./app/src/configuration/adc_config/adc_config_cbor.h --output-c ./app/src/configuration/adc_config/adc_config_cbor.c --output-h-types ./app/src/configuration/adc_config/adc_config.h
```

#### Example command to generate SensorConfig helpers:

```shell
zcbor code -sed --default-max-qty 24 -c ./app/src/configuration/cbor_schemas/sensor_config.cddl -t SensorsConfig --output-h ./app/src/configuration/sensor_config/sensor_config_cbor.h --output-c ./app/src/configuration/sensor_config/sensor_config_cbor.c --output-h-types ./app/src/configuration/sensor_config/sensor_config.h
```

### Network

#### ESP32S3 DevKitC v1.3

To access web interface, connect to the WiFi network with SSID `EerieLeap` and use address `192.168.4.1:8080` in the browser.

#### NativeSim

In order to access web interface while running in the simulator, follow the instructions in the [Networking with native_sim board](https://docs.zephyrproject.org/latest/connectivity/networking/native_sim_setup.html) documentation. Run `./net-setup.sh up` script first, then start the simulator, in Docker container `net-tools` can be found in `~/zephyr/net-tools` directory. In order to gain access to the web interface, port needs to be forwarded from the loopback interface to the TAP IP, `socat` tool can be used for this purpose. Run the following command:

```shell
socat TCP-LISTEN:8080,fork TCP:192.0.2.1:8080
```

where `192.0.2.1` is the TAP IP address.
Once the port is forwarded, the web interface can be accessed at `http://localhost:8080`.
