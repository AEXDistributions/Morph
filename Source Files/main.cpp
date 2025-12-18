#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <vector>
#include "filters.h"

namespace fs = std::filesystem;


std::vector<std::string> parseCommand(const std::string& input) {
    std::vector<std::string> tokens;
    std::string current_token;
    bool inside_quotes = false;

    for (char character : input) {
        if (character == '"') {
            inside_quotes = !inside_quotes;
        }
        else if (character == ' ' && !inside_quotes) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        }
        else {
            current_token += character;
        }
    }

    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }

    return tokens;
}


void displayHelp() {
    std::cout << "\n=== Morph - Image Processing Engine ===\n" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  -i @\"path\"              Load image(s) from file or folder" << std::endl;
    std::cout << "  @i                      List all images in input" << std::endl;
    std::cout << "  @i grayscale <percent>  Apply grayscale filter" << std::endl;
    std::cout << "  @i grayscale <percent> <filename>  Apply grayscale to specific image" << std::endl;
    std::cout << "  preview                 Save images to Morph/output" << std::endl;
    std::cout << "  preview <filename>      Save specific image to Morph/output" << std::endl;
    std::cout << "  -o @\"path\"              Export images and clear" << std::endl;
    std::cout << "  -o keep @\"path\"         Export images but keep in input" << std::endl;
    std::cout << "  help                    Show this help message" << std::endl;
    std::cout << "  exit                    Exit program\n" << std::endl;
}


void handleInputCommand(Pipeline& pipeline, const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        std::cerr << "Use -i @\"path\" to load images" << std::endl;
        return;
    }

    std::string path = tokens[1];
    if (path[0] == '@') {
        pipeline.addInput(path.substr(1));
    }
    else {
        std::cerr << "Use -i @\"path\" to load images" << std::endl;
    }
}


void handleFilterCommand(Pipeline& pipeline, const std::vector<std::string>& tokens) {
    if (tokens.size() == 1) {
        pipeline.listInput();
        return;
    }

    std::string filter_name = tokens[1];
    std::transform(filter_name.begin(), filter_name.end(), filter_name.begin(), ::tolower);

    if (filter_name == "grayscale") {
        std::string percent_str = (tokens.size() >= 3) ? tokens[2] : "100";
        std::string target_file = (tokens.size() >= 4) ? tokens[3] : "";

        size_t percent_pos = percent_str.find('%');
        if (percent_pos != std::string::npos) {
            percent_str.erase(percent_str.begin() + percent_pos);
        }

        double intensity = 100.0;
        try {
            intensity = std::stod(percent_str);
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Invalid percentage value: " << percent_str << std::endl;
            return;
        }

        pipeline.applyGrayscale(target_file, intensity);
    }
    else {
        std::cerr << "Unknown filter: " << filter_name << std::endl;
    }
}


void handlePreviewCommand(Pipeline& pipeline, const std::vector<std::string>& tokens) {
    std::string target_file = (tokens.size() >= 2) ? tokens[1] : "";
    pipeline.savePreview(target_file);
}


void handleOutputCommand(Pipeline& pipeline, const std::vector<std::string>& tokens) {
    if (tokens.size() < 2) {
        std::cerr << "Use -o @\"path\" [keep/clear] [filename] to export" << std::endl;
        return;
    }

    std::string output_path;
    bool clear_after_export = true;
    std::string target_file;

    for (size_t i = 1; i < tokens.size(); i++) {
        std::string token = tokens[i];
        std::string token_lower = token;
        std::transform(token_lower.begin(), token_lower.end(), token_lower.begin(), ::tolower);

        if (token[0] == '@') {
            output_path = token.substr(1);
        }
        else if (token_lower == "keep") {
            clear_after_export = false;
        }
        else if (token_lower == "clear") {
            clear_after_export = true;
        }
        else {
            target_file = token;
        }
    }

    if (output_path.empty()) {
        std::cerr << "Use -o @\"path\" [keep/clear] [filename] to export" << std::endl;
        return;
    }

    pipeline.exportOutput(output_path, clear_after_export, target_file);
}


int main() {
    Pipeline pipeline;
    bool running = true;

    std::cout << "\n=== Morph - Image Processing Engine ===\n" << std::endl;
    std::cout << "Type 'help' for commands\n" << std::endl;

    while (running) {
        std::cout << "> ";
        std::string user_input;
        std::getline(std::cin, user_input);

        if (user_input.empty()) continue;

        std::vector<std::string> tokens = parseCommand(user_input);
        if (tokens.empty()) continue;

        std::string command = tokens[0];
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);

        if (command == "exit" || command == "quit") {
            size_t memory_bytes = pipeline.getMemoryUsage();
            if (memory_bytes > 0) {
                double memory_mb = memory_bytes / (1024.0 * 1024.0);
                std::cout << "\nClearing " << memory_mb << " MB from input..." << std::endl;
            }
            running = false;
        }
        else if (command == "help") {
            displayHelp();
        }
        else if (command == "-i") {
            handleInputCommand(pipeline, tokens);
        }
        else if (command == "@i") {
            handleFilterCommand(pipeline, tokens);
        }
        else if (command == "preview") {
            handlePreviewCommand(pipeline, tokens);
        }
        else if (command == "-o") {
            handleOutputCommand(pipeline, tokens);
        }
        else if (command == "list") {
            pipeline.listInput();
        }
        else {
            std::cerr << "Unknown command. Type 'help' for available commands." << std::endl;
        }
    }

    std::cout << "\nGoodbye!" << std::endl;
    return 0;
}
