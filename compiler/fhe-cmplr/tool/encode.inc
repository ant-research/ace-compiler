#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>   // std::rename, std::remove, perror
#include <cstdlib>  // mkstemp
#include <unistd.h> // close()
#include <ctime>    // std::time, std::strftime
#include <sys/stat.h>

// External headers
#include "common/rtlib.h"
#include "fhe/core/rt_data_util.h"
#include "fhe/core/rt_data_writer.h"
#include "fhe/core/rt_encode_api.h"

bool file_exists(const std::string &path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

// Utility: generate timestamp suffix
std::string current_timestamp() {
    std::time_t now = std::time(nullptr);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y%m%d%H%M%S", std::localtime(&now));
    return std::string(buf);
}

bool Encode_file(const std::string &input_file, const std::string &output_file) {
    std::ifstream ifs(input_file, std::ios::binary);
    if (!ifs) {
        std::cerr << "Failed to open input file: " << input_file << std::endl;
        return false;
    }

    fhe::core::RT_DATA_READER reader(ifs);
    fhe::core::DATA_FILE_HDR hdr;
    std::vector<fhe::core::DATA_LUT_ENTRY> lut;

    if (!reader.Read_hdr(hdr) || !reader.Read_lut(lut)) {
        std::cerr << "Failed to read file header or LUT." << std::endl;
        return false;
    }

    fhe::core::RT_DATA_WRITER w_ofs(output_file.c_str(),
                                    fhe::core::DE_PLAINTEXT,
                                    hdr._model,
                                    hdr._uuid);

    for (uint64_t i = 0; i < lut.size(); ++i) {
        std::vector<float> data(lut[i]._size / sizeof(float));
        reader.Read_entry(lut[i], reinterpret_cast<char*>(data.data()));

        PLAINTEXT_BUFFER* buf = Encode_plain_buffer(data.data(),
                                                    lut[i]._size / sizeof(float),
                                                    lut[i]._scale,
                                                    lut[i]._level);

        w_ofs.Append_pt(lut[i]._name,
                        reinterpret_cast<const char*>(buf),
                        Plain_buffer_length(buf),
                        lut[i]._scale,
                        lut[i]._level);

        Free_plain_buffer(buf);
    }

    return true;
}

int main() {
    // Get input file name from RT_DATA_INFO
    RT_DATA_INFO* data_info = Get_rt_data_info();
    if (!data_info || !(data_info->_file_name)) {
        std::cerr << "RT_DATA_INFO missing or _file_name is empty" << std::endl;
        return 1;
    }

    std::string input_file = data_info->_file_name;

    // Extract directory path
    std::string dir = ".";
    size_t pos = input_file.find_last_of("/\\");
    if (pos != std::string::npos) {
        dir = input_file.substr(0, pos);
    }

    // Create temp file in same dir
    std::string temp_file = dir + "/tmpXXXXXX";
    std::vector<char> tmp_path(temp_file.begin(), temp_file.end());
    tmp_path.push_back('\0');

    int fd = mkstemp(tmp_path.data());
    if (fd == -1) {
        perror("mkstemp failed");
        return 1;
    }
    close(fd);
    temp_file.assign(tmp_path.data());


    Prepare_context();

    // Encode into temp file
    if (!Encode_file(input_file, temp_file)) {
        std::remove(temp_file.c_str());
        return 1;
    }

    // Make a unique backup filename
    std::string backup_file = input_file + ".raw_data";
    if (file_exists(backup_file)) {
        backup_file += "_" + current_timestamp();
    }

    // Rename original file to backup
    if (std::rename(input_file.c_str(), backup_file.c_str()) != 0) {
        perror("Failed to rename original file to backup");
        std::remove(temp_file.c_str());
        return 1;
    }

    // Rename temp file to original file name
    if (std::rename(temp_file.c_str(), input_file.c_str()) != 0) {
        perror("Failed to rename temp file to original name");
        // Try to restore original file
        std::rename(backup_file.c_str(), input_file.c_str());
        std::remove(temp_file.c_str());
        return 1;
    }

    std::cout << "Encoding complete. Backup saved as: " << backup_file << std::endl;
    return 0;
}
