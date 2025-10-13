#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>
#include <zephyr/display/cfb.h>

#include "subsys/device_tree/dt_display.h"
#include "cfb_fonts.h"

#include "cfb.h"

LOG_MODULE_REGISTER(cfb_logger);

namespace eerie_leap::subsys::cfb {

using namespace eerie_leap::subsys::device_tree;

Cfb::Cfb() {
    k_sem_init(&processing_semaphore_, 1, 1);
}

bool Cfb::Initialize() {
    if(initialized_) {
        LOG_INF("Cfb already initialized.");
        return true;
    }

    LOG_INF("Cfb initialization started.");

    if(!DtDisplay::Get()) {
        LOG_ERR("Display device not found.");
        return false;
    }

    if (display_set_pixel_format(DtDisplay::Get(), PIXEL_FORMAT_MONO10) != 0) {
        LOG_ERR("Failed to set pixel format");
        return false;
	}

    if (cfb_framebuffer_init(DtDisplay::Get())) {
		LOG_ERR("Framebuffer initialization failed!");
		return false;
	}

	cfb_framebuffer_clear(DtDisplay::Get(), true);
    cfb_framebuffer_invert(DtDisplay::Get());

	display_blanking_off(DtDisplay::Get());
    cfb_framebuffer_finalize(DtDisplay::Get());

    SetFont(0);
    cfb_set_kerning(DtDisplay::Get(), 0);

    LOG_INF("Cfb initialized successfully.");

    x_res_ = cfb_get_display_parameter(DtDisplay::Get(), CFB_DISPLAY_WIDTH);
    y_res_ = cfb_get_display_parameter(DtDisplay::Get(), CFB_DISPLAY_HEIGHT);

    PrintScreenInfo();

    InitializeThread();

    initialized_ = true;

    return true;
}

void Cfb::InitializeThread() {
    stack_area_ = k_thread_stack_alloc(k_stack_size_, 0);
    k_work_queue_init(&work_q);
    k_work_queue_start(&work_q, stack_area_, k_stack_size_, k_priority_, nullptr);

    k_thread_name_set(&work_q.thread, "display_service");

    k_work_init(&task_.work, CfbTaskWorkTask);
    task_.work_q = &work_q;
    task_.processing_semaphore = &processing_semaphore_;
    atomic_set(&task_.is_animation_running_, 0);

    LOG_INF("Display service initialized.");

    k_work_submit_to_queue(&work_q, &task_.work);
}

void Cfb::CfbTaskWorkTask(k_work* work) {
    CfbTask* task = CONTAINER_OF(work, CfbTask, work);

    if(k_sem_take(task->processing_semaphore, PROCESSING_TIMEOUT) != 0) {
        LOG_ERR("Display service lock timed out");

        return;
    }

    bool is_animation_running = atomic_get(&task->is_animation_running_)
        && task->frame_rate > 0
        && task->animation_handler;

    if(is_animation_running)
        task->animation_handler();

    cfb_framebuffer_finalize(DtDisplay::Get());

    if(is_animation_running) {
        k_sleep(K_MSEC(1000 / task->frame_rate));
        k_work_submit_to_queue(task->work_q, &task->work);
    }

    k_sem_give(task->processing_semaphore);
}

bool Cfb::Flush() {
    if(k_sem_count_get(&processing_semaphore_) == 0)
        return false;

    k_work_submit_to_queue(&work_q, &task_.work);
    k_work_flush(&task_.work, &work_sync_);

    return true;
}

bool Cfb::SetFont(uint8_t font_idx) {
    if(cfb_framebuffer_set_font(DtDisplay::Get(), font_idx) != 0) {
        LOG_ERR("Failed to set font");
        return false;
    }

    if(cfb_get_font_size(DtDisplay::Get(), font_idx, &font_width_, &font_height_) != 0) {
        LOG_ERR("Failed to get font size");
        return false;
    }

    return true;
}

int Cfb::GetFontHeight() const {
    return font_height_;
}

int Cfb::GetFontWidth() const {
    return font_width_;
}

bool Cfb::PrintString(const char* str, const Coordinate& coordinate) {
    if(cfb_print(DtDisplay::Get(), str, coordinate.x, coordinate.y) != 0) {
        LOG_ERR("Failed to print a string");
        return false;
    }

    return true;
}

bool Cfb::PrintStringLine(const char* str, const Coordinate& coordinate) {
    if(cfb_draw_text(DtDisplay::Get(), str, coordinate.x, coordinate.y) != 0) {
        LOG_ERR("Failed to print a string");
        return false;
    }

    return true;
}

bool Cfb::DrawPoint(const Coordinate& coordinate) {
    cfb_position cbf_coordinate = {coordinate.x, coordinate.y};

    if(cfb_draw_point(DtDisplay::Get(), &cbf_coordinate) != 0) {
        LOG_ERR("Failed to draw a point");
        return false;
    }

    return true;
}

bool Cfb::DrawLine(const Coordinate& start, const Coordinate& end) {
    cfb_position cbf_start = {start.x, start.y};
    cfb_position cbf_end = {end.x, end.y};

    if(cfb_draw_line(DtDisplay::Get(), &cbf_start, &cbf_end) != 0) {
        LOG_ERR("Failed to draw a line");
        return false;
    }

    return true;
}

bool Cfb::DrawRectangle(const Coordinate& start, const Coordinate& end) {
    cfb_position cbf_start = {start.x, start.y};
    cfb_position cbf_end = {end.x, end.y};

    if(cfb_draw_rect(DtDisplay::Get(), &cbf_start, &cbf_end) != 0) {
        LOG_ERR("Failed to draw a rectangle");
        return false;
    }

    return true;
}

bool Cfb::DrawCircle(const Coordinate& center, uint16_t radius) {
    cfb_position cbf_center = {center.x, center.y};

    if(cfb_draw_circle(DtDisplay::Get(), &cbf_center, radius) != 0) {
        LOG_ERR("Failed to draw a circle");
        return false;
    }

    return true;
}

bool Cfb::InvertArea(const Coordinate& start, const Coordinate& end) {
    uint16_t height = std::abs(end.y - start.y);
    uint16_t width = std::abs(end.x - start.x);

    if(cfb_invert_area(
        DtDisplay::Get(),
        start.x >= end.x ? end.x : start.x,
        start.y >= end.y ? end.y : start.y,
        width,
        height) != 0) {

        LOG_ERR("Failed to invert an area");
        return false;
    }

    return true;
}

bool Cfb::Clear(bool clear_display) {
    if(cfb_framebuffer_clear(DtDisplay::Get(), clear_display) != 0) {
        LOG_ERR("Failed to clear the screen");
        return false;
    }

    return true;
}

void Cfb::SetAnimationHandler(std::function<void()> handler, uint32_t frame_rate) {
    task_.animation_handler = handler;
    task_.frame_rate = frame_rate;
}

void Cfb::StartAnimation() {
    atomic_set(&task_.is_animation_running_, 1);
    k_work_submit_to_queue(&work_q, &task_.work);
}

void Cfb::StopAnimation() {
    k_work_cancel_sync(&task_.work, &work_sync_);
    atomic_set(&task_.is_animation_running_, 0);
}

void Cfb::PrintScreenInfo() {
	uint16_t rows = cfb_get_display_parameter(DtDisplay::Get(), CFB_DISPLAY_ROWS);
	uint8_t ppt = cfb_get_display_parameter(DtDisplay::Get(), CFB_DISPLAY_PPT);
    uint16_t cols = cfb_get_display_parameter(DtDisplay::Get(), CFB_DISPLAY_COLS);

    LOG_INF("x_res %d, y_res %d, ppt %d, rows %d, cols %d",
        x_res_, y_res_, ppt, rows, cols);

    uint8_t font_width;
	uint8_t font_height;

	for(int idx = 0; idx < 42; idx++) {
		if(cfb_get_font_size(DtDisplay::Get(), idx, &font_width, &font_height))
			break;

		LOG_INF("font %d: width %d, height %d", idx, font_width, font_height);
	}
}

}  // namespace eerie_leap::subsys::cfb
