#include <array>
#include <memory>
#include <string>
#include <cstdint>
#include <unordered_set>
#include <zephyr/ztest.h>

#include "subsys/device_tree/dt_fs.h"
#include "subsys/fs/services/i_fs_service.h"
#include "subsys/fs/services/fs_service.h"

using namespace eerie_leap::subsys::device_tree;
using namespace eerie_leap::subsys::fs::services;

void CleanUpFs(void *f);

ZTEST_SUITE(fs_service, NULL, NULL, CleanUpFs, CleanUpFs, NULL);

struct TestData {
    std::string str;
    int number;
};

void CleanUpFs(void *f) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    fs_service->Format();
}

void SetupFiles(std::shared_ptr<FsService> fs_service, std::string& name_prefix) {
    TestData test_data = {
        .str = "Some test string test_" + name_prefix,
        .number = 1234
    };

    std::string dir_path = "test_" + name_prefix + "_dir";
    bool result = fs_service->CreateDirectory(dir_path);
    zassert_true(result);

    std::string file_path_1 = "test_" + name_prefix + "_dir/test_" + name_prefix + "_file_1.txt";
    result = fs_service->WriteFile(file_path_1, &test_data, sizeof(test_data));
    zassert_true(result);

    std::string file_path_2 = "test_" + name_prefix + "_dir/test_" + name_prefix + "_file_2.txt";
    result = fs_service->WriteFile(file_path_2, &test_data, sizeof(test_data));
    zassert_true(result);

    std::string file_path_3 = "test_" + name_prefix + "_dir/test_" + name_prefix + "_file_3.txt";
    result = fs_service->WriteFile(file_path_3, &test_data, sizeof(test_data));
    zassert_true(result);

    std::string file_path_4 = "test_" + name_prefix + "_file_4.txt";
    result = fs_service->WriteFile(file_path_4, &test_data, sizeof(test_data));
    zassert_true(result);
}

ZTEST(fs_service, test_WriteFile) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    TestData test_data = {
        .str = "Some test string test_WriteFile",
        .number = 1234
    };

    std::string file_path = "test_WriteFile.txt";
    bool result = fs_service->WriteFile(file_path, &test_data, sizeof(test_data));

    zassert_true(result);
}

ZTEST(fs_service, test_ReadFile) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    TestData test_data = {
        .str = "Some test string test_ReadFile",
        .number = 2345
    };

    std::string file_path = "test_ReadFile.txt";
    bool result = fs_service->WriteFile(file_path, &test_data, sizeof(test_data));
    zassert_true(result);

    std::array<uint8_t, sizeof(TestData)> read_buffer = {};
    size_t read_size = 0;

    bool read_result = fs_service->ReadFile(file_path, read_buffer.data(), read_buffer.size(), read_size);
    zassert_true(read_result);

    auto test_data_read = *reinterpret_cast<TestData*>(read_buffer.data());

    zassert_str_equal(test_data_read.str.c_str(), test_data.str.c_str());
    zassert_equal(test_data_read.number, test_data.number);
}

ZTEST(fs_service, test_Exists) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    TestData test_data = {
        .str = "Some test string test_Exists",
        .number = 3456
    };

    std::string file_path = "test_Exists.txt";
    bool result = fs_service->WriteFile(file_path, &test_data, sizeof(test_data));
    zassert_true(result);

    bool result_exists = fs_service->Exists(file_path);
    zassert_true(result_exists);

    std::string fake_file_path = "fake_test_Exists.txt";
    bool result_not_exists = fs_service->Exists(fake_file_path);
    zassert_false(result_not_exists);
}

ZTEST(fs_service, test_CreateDirectory) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    std::string dir_path = "test_CreateDirectory";
    bool result = fs_service->CreateDirectory(dir_path);
    zassert_true(result);

    bool result_exists = fs_service->Exists(dir_path);
    zassert_true(result_exists);

    std::string fake_dir_path= "fake_test_CreateDirectory";
    bool result_not_exists = fs_service->Exists(fake_dir_path);
    zassert_false(result_not_exists);
}

ZTEST(fs_service, test_ListFiles) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    std::string name_prefix = "ListFiles";
    std::string dir_path = "test_" + name_prefix + "_dir";
    SetupFiles(fs_service, name_prefix);

    auto core_files = fs_service->ListFiles();
    zassert_equal(core_files.size(), 2);

    std::unordered_set<std::string> core_files_set;
    for(auto name : core_files)
        core_files_set.insert(name);
    zassert_equal(core_files_set.count(dir_path), 1);
    zassert_equal(core_files_set.count("test_ListFiles_file_4.txt"), 1);


    auto dir_files = fs_service->ListFiles("test_" + name_prefix + "_dir");
    zassert_equal(dir_files.size(), 3);

    std::unordered_set<std::string> dir_files_set;
    for(auto name : dir_files)
        dir_files_set.insert(name);
    zassert_equal(dir_files_set.count("test_ListFiles_file_1.txt"), 1);
    zassert_equal(dir_files_set.count("test_ListFiles_file_2.txt"), 1);
    zassert_equal(dir_files_set.count("test_ListFiles_file_3.txt"), 1);
}

ZTEST(fs_service, test_Delete) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    std::string name_prefix = "Delete";
    std::string dir_path = "test_" + name_prefix + "_dir";
    SetupFiles(fs_service, name_prefix);

    auto core_files = fs_service->ListFiles();
    zassert_equal(core_files.size(), 2);

    std::unordered_set<std::string> core_files_set;
    for(auto name : core_files)
        core_files_set.insert(name);
    zassert_equal(core_files_set.count(dir_path), 1);
    zassert_equal(core_files_set.count("test_Delete_file_4.txt"), 1);

    auto dir_files = fs_service->ListFiles(dir_path);
    zassert_equal(dir_files.size(), 3);

    std::unordered_set<std::string> dir_files_set;
    for(auto name : dir_files)
        dir_files_set.insert(name);
    zassert_equal(dir_files_set.count("test_Delete_file_1.txt"), 1);
    zassert_equal(dir_files_set.count("test_Delete_file_2.txt"), 1);
    zassert_equal(dir_files_set.count("test_Delete_file_3.txt"), 1);

    fs_service->DeleteFile(dir_path + "/test_Delete_file_2.txt");
    dir_files = fs_service->ListFiles(dir_path);
    zassert_equal(dir_files.size(), 2);

    dir_files_set.clear();
    for(auto name : dir_files)
        dir_files_set.insert(name);
    zassert_equal(dir_files_set.count("test_Delete_file_1.txt"), 1);
    zassert_equal(dir_files_set.count("test_Delete_file_2.txt"), 0);
    zassert_equal(dir_files_set.count("test_Delete_file_3.txt"), 1);

    fs_service->DeleteFile("test_Delete_file_4.txt");
    core_files = fs_service->ListFiles();
    zassert_equal(core_files.size(), 1);

    core_files_set.clear();
    for(auto name : core_files)
        core_files_set.insert(name);
    zassert_equal(core_files_set.count(dir_path), 1);
    zassert_equal(core_files_set.count("test_Delete_file_4.txt"), 0);
}

ZTEST(fs_service, test_DeleteRecursive) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    auto core_files = fs_service->ListFiles();
    zassert_equal(core_files.size(), 0);

    std::string name_prefix = "DeleteRecursive";
    std::string dir_path = "test_" + name_prefix + "_dir";
    SetupFiles(fs_service, name_prefix);

    core_files = fs_service->ListFiles();
    zassert_equal(core_files.size(), 2);

    auto dir_files = fs_service->ListFiles(dir_path);
    zassert_equal(dir_files.size(), 3);

    fs_service->DeleteRecursive();
    core_files = fs_service->ListFiles();
    zassert_equal(core_files.size(), 0);
}

ZTEST(fs_service, test_Format) {
    DtFs::InitInternalFs();
    auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());

    auto core_files = fs_service->ListFiles();
    zassert_equal(core_files.size(), 0);

    std::string name_prefix = "Format";
    std::string dir_path = "test_" + name_prefix + "_dir";
    SetupFiles(fs_service, name_prefix);

    core_files = fs_service->ListFiles();
    zassert_equal(core_files.size(), 2);

    auto dir_files = fs_service->ListFiles(dir_path);
    zassert_equal(dir_files.size(), 3);

    fs_service->Format();
    core_files = fs_service->ListFiles();
    zassert_equal(core_files.size(), 0);
}
