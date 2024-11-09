// Necessary includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#define N 25  // Number of rows in nation.tbl (e.g., for TPC-H)
#define D 2   // Number of columns for simplification (Primary Key, Value)

int nation_data[N][D];  // Array to hold data from nation.tbl

// Function to load nation.tbl.csv data
void loadNationTable(int data[N][D]) {
    std::cout << "fload nation table called" << std::endl;
    std::ifstream file("tpch_1GB/temp.tbl.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open nation.tbl.csv" << std::endl;
        return;
    }

    std::string line;
    int row = 0;
    while (std::getline(file, line) && row < N) {
        std::stringstream ss(line);
        std::string cell;
        std::cout << line << std::endl;
        // Assuming the CSV format has two columns: PrimaryKey, Value
        for (int col = 0; col < D; ++col) {
            if (!std::getline(ss, cell, '|')) break;
            data[row][col] = std::stoi(cell);
        }
        row++;
    }
    std::cout << "nation table loaded" << std::endl;
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < D; j++) {
            std::cout << data[i][j] << " ";
        }
        std::cout << std::endl;
    }
    file.close();
}

// Function to load shader code from a file
std::string loadShaderCode(const char* filePath) {
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    return shaderStream.str();
}


// Utility function to compile shader program (simplified)
GLuint compileShaderProgram(const char* vertexSource, const char* fragmentSource) {
    // Compile shaders and link to a program (implementation needed)
    // Returning a dummy program ID here for the example
    GLuint programID = glCreateProgram();
    // (Compile and link steps omitted for brevity)
    return programID;
}

int main() {
    // Initialize OpenGL context (using GLFW)
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed" << std::endl;
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Texture Query", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewInit();

    // Load data from the nation table
    loadNationTable(nation_data);

    // Generate and bind a 2D texture to store the data
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters (nearest neighbor filtering for discrete lookups)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Transfer data to the 2D texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, N, D, 0, GL_RED_INTEGER, GL_INT, nation_data);

     // Load shaders from separate files
    std::string vertexShaderCode = loadShaderCode("shader.vs");
    std::string fragmentShaderCode = loadShaderCode("shader.fs");

    // Compile shader program using the loaded shader code
    GLuint shaderProgram = compileShaderProgram(vertexShaderCode.c_str(), fragmentShaderCode.c_str());
    glUseProgram(shaderProgram);

    // Set the target primary key to query
    int targetKey = 4;  // Example primary key to look up
    int columnIndex = 1;  // Example column index to query
    glUniform1i(glGetUniformLocation(shaderProgram, "targetKey"), targetKey);
    glUniform1i(glGetUniformLocation(shaderProgram, "columnIndex"), columnIndex);
    
    // Bind texture and activate it for use in the shader
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "nationTexture"), 0);  // texture unit 0

    // Render call (dummy, to invoke the shader program)
    glDrawArrays(GL_POINTS, 0, 1);

    // Read back query result from GPU if needed
    int queryResult;
    glGetUniformiv(shaderProgram, glGetUniformLocation(shaderProgram, "queryResult"), &queryResult);

    // Output or use queryResult for further operations
    printf("Query Result: %d\n", queryResult);

    // Clean up and exit
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
