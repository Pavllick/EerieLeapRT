#pragma once

#include <cstdint>

#include "utilities/gui/coordinate.h"

#include "cbf_task.h"

namespace eerie_leap::subsys::cfb {

using namespace eerie_leap::utilities::gui;

class Cfb {
private:
    uint16_t x_res_;
    uint16_t y_res_;

    static constexpr int k_stack_size_ = 1024;
    static constexpr int k_priority_ = K_PRIO_PREEMPT(6);

    k_thread_stack_t* stack_area_;
    k_work_q work_q;
    k_work_sync work_sync_;
    CfbTask task_;

    k_sem processing_semaphore_;
    static constexpr k_timeout_t PROCESSING_TIMEOUT = K_MSEC(200);

    void PrintScreenInfo();
    void InitializeThread();
    static void CfbTaskWorkTask(k_work* work);

public:
    Cfb();

    bool Initialize();
    bool SetFont(uint8_t font_idx);
    bool PrintString(const char* str, const Coordinate& coordinate);
    bool PrintStringLine(const char* str, const Coordinate& coordinate);
    bool DrawRectangle(const Coordinate& start, const Coordinate& end);

    bool Flush();
    bool Clear(bool clear_display = false);

    [[nodiscard]] uint16_t GetXRes() const { return x_res_; }
    [[nodiscard]] uint16_t GetYRes() const { return y_res_; }
};

}  // namespace eerie_leap::subsys::cfb
