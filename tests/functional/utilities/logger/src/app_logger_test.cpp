#include <zephyr/kernel.h>
#include "utilities/logging/logger.hpp"

void test_log_output_formats() {
    printk("--- Test Case: Log Output Formats ---\n");
    eerie_leap::utilities::logging::Logger test_logger("FormatTest");

    printk("MARKER_FORMAT_DEBUG_START\n");
    test_logger.Debug("Format: This is a debug message with number %d.", 123);
    printk("MARKER_FORMAT_DEBUG_END\n");

    printk("MARKER_FORMAT_INFO_START\n");
    test_logger.Info("Format: This is an info message with string '%s'.", "hello");
    printk("MARKER_FORMAT_INFO_END\n");

    printk("MARKER_FORMAT_WARNING_START\n");
    test_logger.Warning("Format: This is a warning message.");
    printk("MARKER_FORMAT_WARNING_END\n");

    printk("MARKER_FORMAT_ERROR_START\n");
    test_logger.Error("Format: This is an error message with float %.2f.", 3.14f);
    printk("MARKER_FORMAT_ERROR_END\n");

    printk("--- End Test Case: Log Output Formats ---\n\n");
}

void test_log_level_filtering() {
    printk("--- Test Case: Log Level Filtering (Logger level set to INFO) ---\n");
    eerie_leap::utilities::logging::Logger filter_logger("FilterTest");

    printk("MARKER_FILTER_DEBUG_START\n");
    filter_logger.Debug("Filter: This DEBUG message should NOT be visible.");
    printk("MARKER_FILTER_DEBUG_END\n");

    printk("MARKER_FILTER_INFO_START\n");
    filter_logger.Info("Filter: This INFO message SHOULD be visible.");
    printk("MARKER_FILTER_INFO_END\n");

    printk("MARKER_FILTER_WARNING_START\n");
    filter_logger.Warning("Filter: This WARNING message SHOULD be visible.");
    printk("MARKER_FILTER_WARNING_END\n");

    printk("MARKER_FILTER_ERROR_START\n");
    filter_logger.Error("Filter: This ERROR message SHOULD be visible.");
    printk("MARKER_FILTER_ERROR_END\n");
    printk("--- End Test Case: Log Level Filtering ---\n\n");
}

void test_default_log_level_output() {
    printk("--- Test Case: Output with Logger level at INFO (prj.conf) ---\n");
    eerie_leap::utilities::logging::Logger default_logger("DefaultOutputTest");

    printk("MARKER_DEFAULT_DEBUG_START\n");
    default_logger.Debug("DefaultOutput: Debug message (should NOT appear).");
    printk("MARKER_DEFAULT_DEBUG_END\n");

    printk("MARKER_DEFAULT_INFO_START\n");
    default_logger.Info("DefaultOutput: Info message (should appear).");
    printk("MARKER_DEFAULT_INFO_END\n");

    printk("MARKER_DEFAULT_WARNING_START\n");
    default_logger.Warning("DefaultOutput: Warning message (should appear).");
    printk("MARKER_DEFAULT_WARNING_END\n");

    printk("MARKER_DEFAULT_ERROR_START\n");
    default_logger.Error("DefaultOutput: Error message (should appear).");
    printk("MARKER_DEFAULT_ERROR_END\n");
    printk("--- End Test Case: Output with Logger level at INFO ---\n\n");
}


int main(void) {
    printk("===== Starting AppLogger Test Suite =====\n\n");

    test_log_output_formats();
    k_msleep(100);

    test_log_level_filtering();
    k_msleep(100);

    test_default_log_level_output();
    k_msleep(100);

    printk("===== AppLogger Test Suite Complete =====\n");
    return 0;
}
