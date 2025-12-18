#ifndef FILTERS_H
#define FILTERS_H

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <memory>

extern "C" {
    unsigned char* stbi_load(char const* filename, int* x, int* y, int* channels_in_file, int desired_channels);
    void stbi_image_free(void* retval_from_stbi_load);
    int stbi_write_png(char const* filename, int w, int h, int comp, const void* data, int stride_in_bytes);
    int stbi_write_jpg(char const* filename, int w, int h, int comp, const void* data, int quality);
    int stbi_write_bmp(char const* filename, int w, int h, int comp, const void* data);
}

namespace fs = std::filesystem;


struct ImageData {
    std::string original_path;
    std::string filename;
    int width;
    int height;
    int channels;
    std::unique_ptr<unsigned char[]> pixels;
    bool modified;

    ImageData() : width(0), height(0), channels(0), modified(false) {}

    ImageData(ImageData&& other) noexcept
        : original_path(std::move(other.original_path)),
          filename(std::move(other.filename)),
          width(other.width),
          height(other.height),
          channels(other.channels),
          pixels(std::move(other.pixels)),
          modified(other.modified) {
        other.width = 0;
        other.height = 0;
        other.channels = 0;
        other.modified = false;
    }

    ImageData& operator=(ImageData&& other) noexcept {
        if (this != &other) {
            original_path = std::move(other.original_path);
            filename = std::move(other.filename);
            width = other.width;
            height = other.height;
            channels = other.channels;
            pixels = std::move(other.pixels);
            modified = other.modified;

            other.width = 0;
            other.height = 0;
            other.channels = 0;
            other.modified = false;
        }
        return *this;
    }

    ImageData(const ImageData&) = delete;
    ImageData& operator=(const ImageData&) = delete;
};


class Pipeline {
private:
    std::string base_folder;
    std::string input_folder;
    std::string output_folder;
    std::vector<ImageData> loaded_images;


    void createFolderStructure() {
        if (!fs::exists(base_folder)) {
            fs::create_directory(base_folder);
            std::cout << "Created Morph folder: " << base_folder << std::endl;
        }

        if (!fs::exists(input_folder)) {
            fs::create_directory(input_folder);
            std::cout << "Created input folder: " << input_folder << std::endl;
        }

        if (!fs::exists(output_folder)) {
            fs::create_directory(output_folder);
            std::cout << "Created output folder: " << output_folder << std::endl;
        }
    }


    bool isValidImageFormat(const std::string& extension) {
        std::string ext = extension;
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        return ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" ||
               ext == ".tga" || ext == ".gif" || ext == ".webp" || ext == ".tiff" || ext == ".tif";
    }


    bool loadSingleImage(const std::string& file_path) {
        ImageData img;

        unsigned char* data = stbi_load(file_path.c_str(), &img.width, &img.height, &img.channels, 0);
        if (!data) {
            std::cerr << "Failed to load: " << file_path << std::endl;
            return false;
        }

        size_t data_size = img.width * img.height * img.channels;
        img.pixels = std::unique_ptr<unsigned char[]>(new unsigned char[data_size]);
        std::copy(data, data + data_size, img.pixels.get());

        img.original_path = file_path;
        img.filename = fs::path(file_path).filename().string();
        std::transform(img.filename.begin(), img.filename.end(), img.filename.begin(), ::tolower);
        img.modified = false;

        stbi_image_free(data);
        loaded_images.push_back(std::move(img));
        
        return true;
    }


    ImageData* findImageByName(const std::string& target_name) {
        std::string target_lower = target_name;
        std::transform(target_lower.begin(), target_lower.end(), target_lower.begin(), ::tolower);

        for (auto& img : loaded_images) {
            if (img.filename == target_lower) {
                return &img;
            }
        }
        return nullptr;
    }


    bool writeImageToFile(const ImageData& img, const std::string& output_path) {
        fs::path original_path(img.original_path);
        std::string ext = original_path.extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".png") {
            return stbi_write_png(output_path.c_str(), img.width, img.height,
                                img.channels, img.pixels.get(), img.width * img.channels);
        }
        else if (ext == ".jpg" || ext == ".jpeg") {
            return stbi_write_jpg(output_path.c_str(), img.width, img.height,
                                img.channels, img.pixels.get(), 95);
        }
        else if (ext == ".bmp") {
            return stbi_write_bmp(output_path.c_str(), img.width, img.height,
                                img.channels, img.pixels.get());
        }
        
        return false;
    }

public:
    Pipeline() : base_folder("Morph"),
                 input_folder("Morph/input"),
                 output_folder("Morph/output") {
        createFolderStructure();
    }


    bool addInput(const std::string& path) {
        if (!fs::exists(path)) {
            std::cerr << "Path not found: " << path << std::endl;
            return false;
        }

        if (fs::is_regular_file(path)) {
            std::string ext = fs::path(path).extension().string();
            
            if (!isValidImageFormat(ext)) {
                std::cerr << "Not a valid image file" << std::endl;
                return false;
            }

            std::string filename = fs::path(path).filename().string();
            std::cout << "Loading: " << filename << std::endl;

            if (loadSingleImage(path)) {
                std::cout << "Added: " << filename << std::endl;
                return true;
            }
            return false;
        }

        if (!fs::is_directory(path)) {
            std::cerr << "Invalid path: " << path << std::endl;
            return false;
        }

        int count = 0;
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();

                if (isValidImageFormat(ext)) {
                    std::string filename = entry.path().filename().string();
                    std::cout << "Loading: " << filename << std::endl;

                    if (loadSingleImage(entry.path().string())) {
                        count++;
                    }
                }
            }
        }

        std::cout << "Loaded " << count << " image(s)" << std::endl;
        return count > 0;
    }


    bool applyGrayscale(const std::string& target = "", double intensity = 100) {
        if (loaded_images.empty()) {
            std::cerr << "No images in input. Use: -i @\"path\"" << std::endl;
            return false;
        }

        intensity = std::max(0.0, std::min(100.0, intensity));
        double blend = intensity / 100.0;

        const double red_weight = 0.299;
        const double green_weight = 0.587;
        const double blue_weight = 0.114;

        std::cout << "Applying grayscale (" << intensity << "%)..." << std::endl;

        int processed_count = 0;
        
        for (auto& img : loaded_images) {
            if (!target.empty()) {
                std::string target_lower = target;
                std::transform(target_lower.begin(), target_lower.end(), target_lower.begin(), ::tolower);
                
                if (img.filename != target_lower) {
                    continue;
                }
            }

            for (int pixel = 0; pixel < img.width * img.height; pixel++) {
                int index = pixel * img.channels;
                
                unsigned char red = img.pixels[index];
                unsigned char green = (img.channels > 1) ? img.pixels[index + 1] : red;
                unsigned char blue = (img.channels > 2) ? img.pixels[index + 2] : red;

                unsigned char gray_value = static_cast<unsigned char>(
                    red_weight * red + green_weight * green + blue_weight * blue
                );

                unsigned char new_red = static_cast<unsigned char>((1.0 - blend) * red + blend * gray_value);
                unsigned char new_green = static_cast<unsigned char>((1.0 - blend) * green + blend * gray_value);
                unsigned char new_blue = static_cast<unsigned char>((1.0 - blend) * blue + blend * gray_value);

                img.pixels[index] = new_red;
                if (img.channels > 1) img.pixels[index + 1] = new_green;
                if (img.channels > 2) img.pixels[index + 2] = new_blue;
            }

            img.modified = true;
            std::cout << "[OK] " << img.filename << std::endl;
            processed_count++;

            if (!target.empty()) break;
        }

        if (processed_count == 0 && !target.empty()) {
            std::cerr << "Image not found in input: " << target << std::endl;
            return false;
        }

        return true;
    }


    bool savePreview(const std::string& target = "") {
        if (loaded_images.empty()) {
            std::cerr << "No images in input." << std::endl;
            return false;
        }

        std::cout << "Saving preview to: " << output_folder << std::endl;

        int saved_count = 0;
        
        for (const auto& img : loaded_images) {
            if (!target.empty()) {
                std::string target_lower = target;
                std::transform(target_lower.begin(), target_lower.end(), target_lower.begin(), ::tolower);
                
                if (img.filename != target_lower) {
                    continue;
                }
            }

            fs::path output_path = fs::path(output_folder) / img.filename;
            bool success = writeImageToFile(img, output_path.string());

            if (success) {
                std::cout << "[OK] " << img.filename << std::endl;
                saved_count++;
            }
            else {
                std::cerr << "[FAIL] " << img.filename << std::endl;
            }

            if (!target.empty()) break;
        }

        std::cout << "Saved " << saved_count << " preview(s) to disk" << std::endl;
        return saved_count > 0;
    }


    bool exportOutput(const std::string& output_path, bool clear_input = true, const std::string& target = "") {
        if (loaded_images.empty()) {
            std::cerr << "No images in input." << std::endl;
            return false;
        }

        fs::path out_dir(output_path);
        if (!fs::exists(out_dir)) {
            try {
                fs::create_directories(out_dir);
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "Failed to create output directory" << std::endl;
                return false;
            }
        }

        std::cout << "Exporting to: " << output_path << std::endl;

        std::vector<size_t> images_to_remove;
        int exported_count = 0;

        for (size_t i = 0; i < loaded_images.size(); i++) {
            const auto& img = loaded_images[i];

            if (!target.empty()) {
                std::string target_lower = target;
                std::transform(target_lower.begin(), target_lower.end(), target_lower.begin(), ::tolower);
                
                if (img.filename != target_lower) {
                    continue;
                }
            }

            fs::path output_file = out_dir / img.filename;
            bool success = writeImageToFile(img, output_file.string());

            if (success) {
                std::cout << "[OK] " << img.filename << std::endl;
                exported_count++;
                
                if (clear_input) {
                    images_to_remove.push_back(i);
                }
            }
            else {
                std::cerr << "[FAIL] " << img.filename << std::endl;
            }

            if (!target.empty()) break;
        }

        std::cout << "Export complete! (" << exported_count << " file(s))" << std::endl;

        if (clear_input && !images_to_remove.empty()) {
            std::cout << "Clearing " << images_to_remove.size() << " image(s) from input..." << std::endl;
            
            for (auto it = images_to_remove.rbegin(); it != images_to_remove.rend(); ++it) {
                loaded_images.erase(loaded_images.begin() + *it);
            }
            
            std::cout << "Input cleared!" << std::endl;
        }

        return exported_count > 0;
    }


    void listInput() const {
        if (loaded_images.empty()) {
            std::cout << "No images in input." << std::endl;
            return;
        }

        std::cout << "Images in input (" << loaded_images.size() << "):" << std::endl;
        
        for (const auto& img : loaded_images) {
            std::string status = img.modified ? " [MODIFIED]" : "";
            size_t memory_size = img.width * img.height * img.channels;
            double size_in_mb = memory_size / (1024.0 * 1024.0);
            
            std::cout << "  - " << img.filename 
                     << " (" << img.width << "x" << img.height << ", " << size_in_mb << " MB)" 
                     << status << std::endl;
        }
    }


    size_t getMemoryUsage() const {
        size_t total_bytes = 0;
        for (const auto& img : loaded_images) {
            total_bytes += img.width * img.height * img.channels;
        }
        return total_bytes;
    }
};

#endif
