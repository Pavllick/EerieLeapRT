#include "dt_fs.h"
#include "dt_gpio.h"
#include "dt_adc.h"
#include "dt_display.h"
#include "dt_modbus.h"
#include "dt_canbus.h"

#include "dt_configurator.h"

namespace eerie_leap::subsys::device_tree {

void DtConfigurator::Initialize() {
    DtFs::InitInternalFs();
    DtFs::InitSdFs();
    DtGpio::Initialize();
    DtAdc::Initialize();
    DtDisplay::Initialize();
    DtModbus::Initialize();
    DtCanbus::Initialize();
}

} // namespace eerie_leap::subsys::device_tree
