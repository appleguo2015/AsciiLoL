
/*
    By aPpLegUo
    appleguo2015@github.com
*/

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

// must define these macros before include stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class SimpleASCIIArt {
private:
    float sample_rate = 0.3f;  // Smaller sample rate for console display
    bool use_color = false;
    
    // clear(cross-platform)
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    void appendAsciiChar(std::string& line, char symbol, unsigned char r, unsigned char g,
                         unsigned char b, unsigned char a) const {
        if (a == 0) {
            line += ' ';
            return;
        }

        if (!use_color) {
            line += symbol;
            return;
        }

        int color_r = static_cast<int>(r) * static_cast<int>(a) / 255;
        int color_g = static_cast<int>(g) * static_cast<int>(a) / 255;
        int color_b = static_cast<int>(b) * static_cast<int>(a) / 255;

        line += "\033[38;2;";
        line += std::to_string(color_r);
        line += ';';
        line += std::to_string(color_g);
        line += ';';
        line += std::to_string(color_b);
        line += 'm';
        line += symbol;
    }
    
public:
    /**
     * Generates ASCII art from an image file
     * @param filename Path to the input image file
     * @param symbols Character set for ASCII art (darker to brighter)
     */
    void generate(const std::string& filename, const std::string& symbols) {
        // Load image using STB library
        int width, height, channels;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
        
        if (!data) {
            std::cerr << "Error: Failed to load image: " << filename << std::endl;
            return;
        }
        
        std::cout << "Image size: " << width << "x" << height << std::endl;
        std::cout << "Color channels: " << channels << std::endl;
        
        // Calculate ASCII art dimensions
        int ascii_width = static_cast<int>(width * sample_rate);
        int ascii_height = static_cast<int>(height * sample_rate * 0.5f);
        
        ascii_width = std::max(1, ascii_width);
        ascii_height = std::max(1, ascii_height);
        
        std::cout << "ASCII dimensions: " << ascii_width << "x" << ascii_height << std::endl;
        std::cout << "Character set: " << symbols << std::endl;
        
        // Generate ASCII art
        std::vector<std::string> ascii = generateAsciiFromData(data, width, height, channels, symbols);
        
        // Output to console
        std::cout << "\nASCII Art Output:\n";
        std::cout << std::string(ascii_width, '-') << std::endl;
        for (const auto& line : ascii) {
            std::cout << line << std::endl;
        }
        std::cout << std::string(ascii_width, '-') << std::endl;
        
        // Free image memory allocated by STB library
        stbi_image_free(data);
    }
    
    /**
     * Generates ASCII art from raw image data
     * @param data Raw image data
     * @param width Image width
     * @param height Image height
     * @param channels Number of color channels
     * @param symbols Character set for ASCII art
     * @return Vector of strings representing ASCII art
     */
    std::vector<std::string> generateAsciiFromData(unsigned char* data, int width, int height, 
                                                  int channels, const std::string& symbols) {
        // Calculate ASCII art dimensions
        int ascii_width = static_cast<int>(width * sample_rate);
        int ascii_height = static_cast<int>(height * sample_rate * 0.5f);
        
        ascii_width = std::max(1, ascii_width);
        ascii_height = std::max(1, ascii_height);
        
        std::vector<std::string> ascii;
        
        for (int y = 0; y < ascii_height; y++) {
            std::string line;
            for (int x = 0; x < ascii_width; x++) {
                // Sample original image using nearest-neighbor resizing
                int src_x = x * width / ascii_width;
                int src_y = y * height / ascii_height;
                src_x = std::min(src_x, width - 1);
                src_y = std::min(src_y, height - 1);
                
                // Calculate grayscale value
                int idx = (src_y * width + src_x) * channels;
                unsigned char r = data[idx];
                unsigned char g = (channels >= 2) ? data[idx + 1] : r;
                unsigned char b = (channels >= 3) ? data[idx + 2] : r;
                unsigned char a = (channels >= 4) ? data[idx + 3] : 255;
                
                // Luminance formula: 0.299R + 0.587G + 0.114B
                float gray = (0.299f * r + 0.587f * g + 0.114f * b) * (a / 255.0f);
                
                // Map grayscale value to ASCII character
                int char_idx = static_cast<int>(gray / 255.0f * (symbols.size() - 1));
                char_idx = std::max(0, std::min(static_cast<int>(symbols.size() - 1), char_idx));
                appendAsciiChar(line, symbols[char_idx], r, g, b, a);
            }
            if (use_color) {
                line += "\033[0m";
            }
            ascii.push_back(line);
        }
        
        return ascii;
    }
    
    /**
     * Displays animated GIF in ASCII art format
     * @param filename Path to the GIF file
     * @param symbols Character set for ASCII art
     * @param frame_delay_ms Delay between frames in milliseconds (-1 to use GIF embedded delays)
     * @param loop_count Number of times to loop (0 for infinite)
     */
    void displayGIF(const std::string& filename, const std::string& symbols = " .,:;08@", 
                    int frame_delay_ms = -1, int loop_count = 0) {
        std::cout << "\nLoading GIF: " << filename << std::endl;
        
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file) {
            std::cerr << "Error: Failed to open GIF: " << filename << std::endl;
            return;
        }

        std::streamsize file_size = file.tellg();
        if (file_size <= 0) {
            std::cerr << "Error: GIF file is empty: " << filename << std::endl;
            return;
        }

        file.seekg(0, std::ios::beg);
        std::vector<unsigned char> gif_buffer(static_cast<size_t>(file_size));
        if (!file.read(reinterpret_cast<char*>(gif_buffer.data()), file_size)) {
            std::cerr << "Error: Failed to read GIF file: " << filename << std::endl;
            return;
        }

        int width = 0;
        int height = 0;
        int channels = 0;
        int num_frames = 0;
        int* frame_delays = nullptr;
        unsigned char* data = stbi_load_gif_from_memory(
            gif_buffer.data(),
            static_cast<int>(gif_buffer.size()),
            &frame_delays,
            &width,
            &height,
            &num_frames,
            &channels,
            4
        );
        
        if (!data) {
            std::cerr << "Error: Failed to load GIF: " << filename << std::endl;
            return;
        }

        if (num_frames <= 0) {
            std::cerr << "Error: GIF has no frames." << std::endl;
            stbi_image_free(data);
            stbi_image_free(frame_delays);
            return;
        }

        channels = 4;
        
        std::cout << "GIF size: " << width << "x" << height << std::endl;
        std::cout << "Color channels: " << channels << std::endl;
        std::cout << "GIF frames: " << num_frames << std::endl;
        
        // Calculate ASCII dimensions
        int ascii_width = static_cast<int>(width * sample_rate);
        int ascii_height = static_cast<int>(height * sample_rate * 0.5f);
        
        ascii_width = std::max(1, ascii_width);
        ascii_height = std::max(1, ascii_height);
        
        std::cout << "ASCII dimensions: " << ascii_width << "x" << ascii_height << std::endl;
        if (frame_delay_ms > 0) {
            std::cout << "Frame delay override: " << frame_delay_ms << "ms" << std::endl;
        } else {
            std::cout << "Frame delay: using GIF embedded delays (fallback 100ms)" << std::endl;
        }
        std::cout << "Loop count: " << (loop_count == 0 ? "Infinite" : std::to_string(loop_count)) << std::endl;
        
        const int frame_stride = width * height * channels;
        std::vector<std::vector<std::string>> frames;
        frames.reserve(num_frames);
        
        std::cout << "\nGenerating animation frames..." << std::endl;
        
        for (int frame = 0; frame < num_frames; frame++) {
            std::cout << "Generating frame " << frame + 1 << "/" << num_frames << "...\r";
            std::cout.flush();
            const unsigned char* frame_data = data + frame * frame_stride;
            
            std::vector<std::string> ascii_frame;
            
            for (int y = 0; y < ascii_height; y++) {
                std::string line;
                for (int x = 0; x < ascii_width; x++) {
                    int src_x = x * width / ascii_width;
                    int src_y = y * height / ascii_height;
                    src_x = std::min(src_x, width - 1);
                    src_y = std::min(src_y, height - 1);
                    
                    // Calculate grayscale value
                    int idx = (src_y * width + src_x) * channels;
                    unsigned char r = frame_data[idx];
                    unsigned char g = frame_data[idx + 1];
                    unsigned char b = frame_data[idx + 2];
                    unsigned char a = frame_data[idx + 3];
                    float gray = (0.299f * r + 0.587f * g + 0.114f * b) * (a / 255.0f);
                    
                    // Map to ASCII character
                    int char_idx = static_cast<int>(gray / 255.0f * (symbols.size() - 1));
                    char_idx = std::max(0, std::min(static_cast<int>(symbols.size() - 1), char_idx));
                    appendAsciiChar(line, symbols[char_idx], r, g, b, a);
                }
                if (use_color) {
                    line += "\033[0m";
                }
                ascii_frame.push_back(line);
            }
            
            frames.push_back(ascii_frame);
        }
        
        std::cout << "\nAnimation ready! Press Ctrl+C to stop." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // Play the animation
        int loop_counter = 0;
        int frame_counter = 0;
        
        try {
            while (loop_count == 0 || loop_counter < loop_count) {
                clearScreen();
                
                // Display frame counter info
                std::cout << "GIF Animation - Frame: " << (frame_counter % num_frames) + 1 
                          << "/" << num_frames;
                if (loop_count > 0) {
                    std::cout << " | Loop: " << loop_counter + 1 << "/" << loop_count;
                }
                std::cout << "\n";
                std::cout << std::string(ascii_width, '=') << "\n";
                
                // Display current frame
                const auto& frame = frames[frame_counter % num_frames];
                for (const auto& line : frame) {
                    std::cout << line << std::endl;
                }
                
                std::cout << std::string(ascii_width, '=') << "\n";
                int current_frame = frame_counter % num_frames;
                int current_delay_ms = frame_delay_ms;
                if (current_delay_ms <= 0) {
                    current_delay_ms = 100;
                    if (frame_delays && frame_delays[current_frame] > 0) {
                        current_delay_ms = frame_delays[current_frame];
                    }
                }

                std::cout << "Delay: " << current_delay_ms << "ms | Press Ctrl+C to exit\n";
                
                // Wait for next frame
                std::this_thread::sleep_for(std::chrono::milliseconds(current_delay_ms));
                
                frame_counter++;
                if (frame_counter % num_frames == 0) {
                    loop_counter++;
                }
            }
        } catch (...) {
            // Catch Ctrl+C or other interrupts
            std::cout << "\nAnimation stopped by user.\n";
        }
        
        // Free memory
        stbi_image_free(data);
        stbi_image_free(frame_delays);
    }
    
    /**
     * Set the sample rate for downscaling
     * @param rate Sample rate (0.1 to 1.0)
     */
    void setSampleRate(float rate) {
        sample_rate = std::max(0.1f, std::min(1.0f, rate));
    }

    void setUseColor(bool enabled) {
        use_color = enabled;
    }
};

namespace {
void printUsage(const char* program) {
    std::cout << "ASCII Art Generator with GIF Support" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "Usage: " << program << " [options] <input file>" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  -f            Force static image mode" << std::endl;
    std::cout << "  -gif, -g      Force GIF mode" << std::endl;
    std::cout << "  -s <symbols>  Character set for ASCII art" << std::endl;
    std::cout << "  -c, --color   Enable ANSI truecolor output" << std::endl;
    std::cout << "  -r <rate>     Sample rate 0.1~1.0 (default: 0.3)" << std::endl;
    std::cout << "  -d <delay>    GIF frame delay in ms (<=0 means use GIF delay)" << std::endl;
    std::cout << "  -l <loops>    GIF loop count (0 for infinite, default: 0)" << std::endl;
    std::cout << "  -APPLEGUO     Use preset symbols: \" app:lEGUO\"" << std::endl;
    std::cout << "  -h, --help    Show this help" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  " << program << " image.jpg" << std::endl;
    std::cout << "  " << program << " -s \" .:-=+*#%@\" image.jpg" << std::endl;
    std::cout << "  " << program << " -c -s \" .:-=+*#%@\" image.jpg" << std::endl;
    std::cout << "  " << program << " -gif -d 50 -l 3 -s \" .,:;08@\" animation.gif" << std::endl;
}

bool isGifFile(const std::string& filename) {
    if (filename.size() < 4) {
        return false;
    }
    std::string ext = filename.substr(filename.size() - 4);
    std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return ext == ".gif";
}

bool looksLikeOption(const std::string& token) {
    return !token.empty() && token[0] == '-';
}
} // namespace

int main(int argc, char* argv[]) {
    SimpleASCIIArt art;

    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    enum class Mode { Auto, Static, Gif };
    Mode mode = Mode::Auto;

    std::string symbols = " .,:;08@";
    std::string filename;
    float sample_rate = 0.3f;
    int frame_delay = -1;
    int loop_count = 0;
    bool use_color = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }

        if (arg == "-f") {
            mode = Mode::Static;
            if (i + 1 < argc && !looksLikeOption(argv[i + 1]) && filename.empty()) {
                filename = argv[++i];
            }
            continue;
        }

        if (arg == "-gif" || arg == "-g") {
            mode = Mode::Gif;
            if (i + 1 < argc && !looksLikeOption(argv[i + 1]) && filename.empty()) {
                filename = argv[++i];
            }
            continue;
        }

        if (arg == "-APPLEGUO") {
            symbols = " app:lEGUO";
            if (i + 1 < argc && !looksLikeOption(argv[i + 1]) && filename.empty()) {
                filename = argv[++i];
            }
            continue;
        }

        if (arg == "-s") {
            if (i + 1 >= argc) {
                std::cerr << "Error: -s requires a value." << std::endl;
                return 1;
            }
            symbols = argv[++i];
            continue;
        }

        if (arg == "-c" || arg == "--color") {
            use_color = true;
            continue;
        }

        if (arg == "-r") {
            if (i + 1 >= argc) {
                std::cerr << "Error: -r requires a value." << std::endl;
                return 1;
            }
            try {
                sample_rate = std::stof(argv[++i]);
            } catch (...) {
                std::cerr << "Error: Invalid sample rate: " << argv[i] << std::endl;
                return 1;
            }
            continue;
        }

        if (arg == "-d") {
            if (i + 1 >= argc) {
                std::cerr << "Error: -d requires a value." << std::endl;
                return 1;
            }
            int parsed_delay = 0;
            try {
                parsed_delay = std::stoi(argv[++i]);
            } catch (...) {
                std::cerr << "Error: Invalid frame delay: " << argv[i] << std::endl;
                return 1;
            }
            frame_delay = (parsed_delay <= 0) ? -1 : std::max(10, std::min(1000, parsed_delay));
            continue;
        }

        if (arg == "-l") {
            if (i + 1 >= argc) {
                std::cerr << "Error: -l requires a value." << std::endl;
                return 1;
            }
            try {
                loop_count = std::stoi(argv[++i]);
            } catch (...) {
                std::cerr << "Error: Invalid loop count: " << argv[i] << std::endl;
                return 1;
            }
            loop_count = std::max(0, loop_count);
            continue;
        }

        if (looksLikeOption(arg)) {
            std::cerr << "Error: Unknown option: " << arg << std::endl;
            return 1;
        }

        if (!filename.empty()) {
            std::cerr << "Error: Multiple input files are not supported: '" << filename
                      << "' and '" << arg << "'" << std::endl;
            return 1;
        }

        filename = arg;
    }

    if (filename.empty()) {
        std::cerr << "Error: Missing input file." << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    art.setSampleRate(sample_rate);
    art.setUseColor(use_color);

    if (mode == Mode::Auto) {
        mode = isGifFile(filename) ? Mode::Gif : Mode::Static;
    }

    if (mode == Mode::Gif) {
        art.displayGIF(filename, symbols, frame_delay, loop_count);
    } else {
        art.generate(filename, symbols);
    }

    return 0;
}
