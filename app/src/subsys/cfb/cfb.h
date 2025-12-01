#pragma once

#include <memory>
#include <cstdint>

#include "utilities/gui/coordinate.h"
#include "subsys/threading/work_queue_thread.h"

#include "cbf_task.h"

namespace eerie_leap::subsys::cfb {

using namespace eerie_leap::utilities::gui;
using namespace eerie_leap::subsys::threading;

class Cfb {
private:
    static constexpr int thread_stack_size_ = 2048;
    static constexpr int thread_priority_ = 6;
    std::unique_ptr<WorkQueueThread> work_queue_thread_;
    std::unique_ptr<WorkQueueTask<CfbTask>> work_queue_task_;

    uint16_t x_res_;
    uint16_t y_res_;
    uint8_t font_height_;
    uint8_t font_width_;
    bool initialized_ = false;

    void PrintScreenInfo();
    void InitializeThread();
    static WorkQueueTaskResult ProcessWorkTask(CfbTask* task);

public:
    Cfb();
    ~Cfb() = default;

    bool Initialize();
    bool SetFont(uint8_t font_idx);
    bool PrintString(const char* str, const Coordinate& coordinate);
    bool PrintStringLine(const char* str, const Coordinate& coordinate);
    bool DrawPoint(const Coordinate& coordinate);
    bool DrawLine(const Coordinate& start, const Coordinate& end);
    bool DrawRectangle(const Coordinate& start, const Coordinate& end);
    bool DrawCircle(const Coordinate& center, uint16_t radius);
    bool InvertArea(const Coordinate& start, const Coordinate& end);

    int GetFontHeight() const;
    int GetFontWidth() const;

    bool Flush();
    bool Clear(bool clear_display = false);

    [[nodiscard]] uint16_t GetXRes() const { return x_res_; }
    [[nodiscard]] uint16_t GetYRes() const { return y_res_; }

    void SetAnimationHandler(std::function<void()> handler, uint32_t frame_rate);
    void StartAnimation();
    void StopAnimation();
};

}  // namespace eerie_leap::subsys::cfb
