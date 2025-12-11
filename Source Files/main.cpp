#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>
#include <vector>
#include "stb_image.h"

namespace fs = std::filesystem;

int main() {
    // Step 1: collect all images in current folder
    std::vector<std::string> images;
    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp") {
                images.push_back(entry.path().string());
            }
        }
    }

    if (images.empty()) {
        std::cerr << "No images found in folder!" << std::endl;
        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    // Step 2: show menu
    std::cout << "Found " << images.size() << " image(s):" << std::endl;
    for (size_t i = 0; i < images.size(); i++) {
        std::cout << "  " << (i + 1) << ") " << images[i] << std::endl;
    }
    std::cout << "Enter a number to inspect one image, or 'all' to inspect all: ";

    std::string choice;
    std::getline(std::cin, choice);

    // Step 3: decide what to process
    std::vector<std::string> selected;
    if (choice == "all") {
        selected = images;
    }
    else {
        try {
            int index = std::stoi(choice);
            if (index >= 1 && index <= (int)images.size()) {
                selected.push_back(images[index - 1]);
            }
            else {
                std::cerr << "Invalid selection." << std::endl;
                return 1;
            }
        }
        catch (...) {
            std::cerr << "Invalid input." << std::endl;
            return 1;
        }
    }

    // Step 4: load and show info
    for (const auto& input : selected) {
        std::cout << "\nLoading: " << input << std::endl;

        int width = 0, height = 0, channels = 0;
        unsigned char* data = stbi_load(input.c_str(), &width, &height, &channels, 0);
        if (!data) {
            std::cerr << "Failed to load image: " << input << std::endl;
            continue;
        }

        std::cout << "Dimensions: " << width << " x " << height
            << " | Channels: " << channels << std::endl;

        stbi_image_free(data);
    }

    std::cout << "\nDone. Press Enter to exit...";
    std::cin.get();
    return 0;
}
