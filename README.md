# Eerie Leap Real-Time

EerieLeap is an open-source sensor monitoring system built with [Zephyr RTOS](https://github.com/zephyrproject-rtos/zephyr). It provides a robust platform for reading, processing, and managing sensor data, supporting both physical ADC sensors and virtual calculated sensors.

## Getting Started

Before proceeding, ensure you have a properly set up Zephyr development environment. Follow the official [Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html).

### Initialization

First, initialize the workspace folder (`ws_eerie_leap`) where `eerie_leap` and all Zephyr modules will be cloned. Run the following commands:

```shell
# Initialize ws_eerie_leap for the eerie_leap (main branch)
west init -m https://github.com/pavllick/EerieLeapRT --mr main eerie_leap
# Update Zephyr modules
cd ws_eerie_leap
west update
```

### Building

For VS Code build setup examples, refer to `.vscode_example/tasks.json`. Alternatively, you can build the application using the following command:

```shell
cd eerie_leap
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

## Development

### CBOR Schemas

The project uses [zcbor](https://github.com/NordicSemiconductor/zcbor) to generate CBOR helpers from schemas. To install zcbor:

```shell
pip install zcbor
```

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
