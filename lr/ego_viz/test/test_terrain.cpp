#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>

struct TerrainData {
    float min_x;
    float min_z;
    int grid_width;
    int grid_height;
    float delta;
    std::vector<std::vector<float>> heightmap;
};

// Simple JSON parser for our specific format
bool parse_json_metadata(const std::string& json_str, TerrainData& data) {
    // Remove whitespace and braces
    std::string clean = json_str;
    clean.erase(std::remove(clean.begin(), clean.end(), '{'), clean.end());
    clean.erase(std::remove(clean.begin(), clean.end(), '}'), clean.end());
    clean.erase(std::remove(clean.begin(), clean.end(), '"'), clean.end());
    
    std::istringstream iss(clean);
    std::string line;
    
    while (std::getline(iss, line, ',')) {
        std::istringstream line_stream(line);
        std::string key, value;
        
        if (std::getline(line_stream, key, ':') && std::getline(line_stream, value)) {
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            if (key == "x_min") data.min_x = std::stof(value);
            else if (key == "width") data.grid_width = std::stoi(value);
            else if (key == "y_min") data.min_z = std::stof(value);
            else if (key == "height") data.grid_height = std::stoi(value);
            else if (key == "delta") data.delta = std::stof(value);
        }
    }
    
    return true;
}

TerrainData load_terrain_data(const std::string& metadata_path, const std::string& binary_path) {
    TerrainData data;
    
    // Load metadata
    std::ifstream metadata_file(metadata_path);
    if (!metadata_file.is_open()) {
        std::cerr << "Failed to open metadata file: " << metadata_path << std::endl;
        return data;
    }
    
    std::string json_str;
    std::getline(metadata_file, json_str);
    metadata_file.close();
    
    if (!parse_json_metadata(json_str, data)) {
        std::cerr << "Failed to parse JSON metadata" << std::endl;
        return data;
    }
    
    std::cout << "Parsed terrain metadata:" << std::endl;
    std::cout << "  x_min: " << data.min_x << std::endl;
    std::cout << "  width: " << data.grid_width << std::endl;
    std::cout << "  y_min: " << data.min_z << std::endl;
    std::cout << "  height: " << data.grid_height << std::endl;
    std::cout << "  delta: " << data.delta << std::endl;
    
    // Load binary data
    std::ifstream binary_file(binary_path, std::ios::binary);
    if (!binary_file.is_open()) {
        std::cerr << "Failed to open binary file: " << binary_path << std::endl;
        return data;
    }
    
    // Check file size
    binary_file.seekg(0, std::ios::end);
    size_t file_size = binary_file.tellg();
    size_t expected_size = data.grid_width * data.grid_height * sizeof(float);
    binary_file.seekg(0, std::ios::beg);
    
    std::cout << "Binary file size: " << file_size << " bytes" << std::endl;
    std::cout << "Expected size: " << expected_size << " bytes" << std::endl;
    
    if (file_size != expected_size) {
        std::cerr << "ERROR: Binary file size mismatch! Expected " << expected_size << " but got " << file_size << std::endl;
        return data;
    }
    
    // Initialize heightmap
    data.heightmap.resize(data.grid_height);
    for (int z = 0; z < data.grid_height; z++) {
        data.heightmap[z].resize(data.grid_width);
    }
    
    // Read height values
    std::cout << "Loading heightmap data..." << std::endl;
    for (int z = 0; z < data.grid_height; z++) {
        for (int x = 0; x < data.grid_width; x++) {
            float height;
            binary_file.read(reinterpret_cast<char*>(&height), sizeof(float));
            data.heightmap[z][x] = height;
        }
        
        if (z % 50 == 0) {
            std::cout << "Loaded row " << z << "/" << data.grid_height << std::endl;
        }
    }
    
    binary_file.close();
    std::cout << "Loaded heightmap: " << data.grid_width << "x" << data.grid_height << std::endl;
    
    return data;
}

void test_mesh_generation(const TerrainData& data) {
    std::cout << "Testing mesh generation..." << std::endl;
    
    // Calculate height range
    float min_height = std::numeric_limits<float>::max();
    float max_height = std::numeric_limits<float>::lowest();
    
    std::cout << "Calculating height range..." << std::endl;
    for (int z = 0; z < data.grid_height; z++) {
        for (int x = 0; x < data.grid_width; x++) {
            float height = data.heightmap[z][x];
            if (height < min_height) min_height = height;
            if (height > max_height) max_height = height;
        }
    }
    
    float height_range = max_height - min_height;
    std::cout << "Height range: " << min_height << " to " << max_height << std::endl;
    
    // Count triangles
    std::cout << "Counting triangles..." << std::endl;
    int triangle_count = 0;
    
    for (int z = 0; z < data.grid_height - 1; z++) {
        for (int x = 0; x < data.grid_width - 1; x++) {
            // Get heights at the four corners
            float y1 = data.heightmap[z][x];      // bottom-left
            float y2 = data.heightmap[z][x + 1];  // bottom-right
            float y3 = data.heightmap[z + 1][x];  // top-left
            float y4 = data.heightmap[z + 1][x + 1]; // top-right
            
            triangle_count += 2; // Two triangles per grid cell
        }
        
        if (z % 50 == 0) {
            std::cout << "Processed row " << z << "/" << (data.grid_height - 1) << std::endl;
        }
    }
    
    std::cout << "Total triangles: " << triangle_count << std::endl;
    std::cout << "Expected triangles: " << (data.grid_width - 1) * (data.grid_height - 1) * 2 << std::endl;
}

int main() {
    std::cout << "Testing terrain data loading and mesh generation..." << std::endl;
    
    // Debug: check current directory
    std::cout << "Current working directory: ";
    system("pwd");
    
    // Debug: check if files exist
    std::cout << "Checking if files exist:" << std::endl;
    system("ls -la ../../godot/assets/mock_terrain_metadata.json");
    system("ls -la ../../godot/assets/mock_terrain.bin");
    
    TerrainData data = load_terrain_data("../../godot/assets/mock_terrain_metadata.json", "../../godot/assets/mock_terrain.bin");
    
    if (data.grid_width == 0 || data.grid_height == 0) {
        std::cerr << "Failed to load terrain data" << std::endl;
        return 1;
    }
    
    if (data.heightmap.empty() || data.heightmap[0].empty()) {
        std::cerr << "Heightmap is empty" << std::endl;
        return 1;
    }
    
    test_mesh_generation(data);
    
    std::cout << "Test completed successfully!" << std::endl;
    return 0;
} 