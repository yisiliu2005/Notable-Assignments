#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static unsigned int ss_id = 0;
void dump_framebuffer_to_ppm(std::string prefix, unsigned int width, unsigned int height) {
    int pixelChannel = 3;
    int totalPixelSize = pixelChannel * width * height * sizeof(GLubyte);
    GLubyte * pixels = new GLubyte [totalPixelSize];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    std::string file_name = prefix + std::to_string(ss_id) + ".ppm";
    std::ofstream fout(file_name);
    fout << "P3\n" << width << " " << height << "\n" << 255 << std::endl;
    for (size_t i = 0; i < height; i++)
    {
        for (size_t j = 0; j < width; j++)
        {
            size_t cur = pixelChannel * ((height - i - 1) * width + j);
            fout << (int)pixels[cur] << " " << (int)pixels[cur + 1] << " " << (int)pixels[cur + 2] << " ";
        }
        fout << std::endl;
    }
    ss_id ++;
    delete [] pixels;
    fout.flush();
    fout.close();
}

//key board control
void processInput(GLFWwindow *window)
{
    //press escape to exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    //press p to capture screen
    if(glfwGetKey(window, GLFW_KEY_P) ==GLFW_PRESS)
    {
        std::cout << "Capture Window " << ss_id << std::endl;
        int buffer_width, buffer_height;
        glfwGetFramebufferSize(window, &buffer_width, &buffer_height);
        dump_framebuffer_to_ppm("/Users/yisiliu/development/3GC3/assignment3/assignment3/Assignment3-ss", buffer_width, buffer_height);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//shader
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNormal;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "out vec3 Normal;\n"
    "out vec3 FragPos;\n"
    "uniform mat4 MVP;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = MVP * vec4(aPos, 1.0);\n"
    "   Normal = aNormal;\n"
    "   TexCoord = aTexCoord;\n"
    "   FragPos = aPos;\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 Normal;\n"
    "in vec2 TexCoord;\n"
    "in vec3 FragPos;\n"
    "uniform vec3 lightPos;\n"
    "uniform vec3 ambientColor;\n"
    "uniform vec3 diffuseColorR;\n"
    "uniform vec3 diffuseColorG;\n"
    "uniform vec3 diffuseColorB;\n"
    "uniform vec3 spotDirR;\n"
    "uniform vec3 spotDirG;\n"
    "uniform vec3 spotDirB;\n"
    "uniform sampler2D myTexture;\n"
    "uniform float cutOffCosine;\n"
    "vec3 calculateSpotlight(vec3 spotDir, vec3 diffuseColor) {\n"
    "   vec3 lightDir = normalize(lightPos - FragPos);\n"
    "   float costheta = dot(lightDir, -normalize(spotDir));\n"
    "   if (costheta > cutOffCosine) {\n"
    "       vec3 norm = normalize(Normal);\n"
    "       float diff = max(dot(norm, lightDir), 0.0);\n"
    "       return diffuseColor * diff;}\n"
    "   return vec3(0.0);"
    "}\n"
    "void main()\n"
    "{\n"
    "   float kc = 1.0;\n"
    "   float kl = 0.35*1e-4;\n"
    "   float kq = 0.44*1e-4;\n"
    "   float distance = length(lightPos - FragPos);\n"
    "   float attenuation = 1.0 / (kc + kl * distance + kq * (distance * distance));\n"
    "   vec3 diffuseR = calculateSpotlight(spotDirR, diffuseColorR) * attenuation;\n"
    "   vec3 resultR = ambientColor + diffuseR;\n"
    "   vec3 diffuseG = calculateSpotlight(spotDirG, diffuseColorG) * attenuation;\n"
    "   vec3 resultG = ambientColor + diffuseG;\n"
    "   vec3 diffuseB = calculateSpotlight(spotDirB, diffuseColorB) * attenuation;\n"
    "   vec3 resultB = ambientColor + diffuseB;\n"
    "   vec3 result = resultR + resultG + resultB;\n"
    "   FragColor = texture(myTexture, TexCoord) * vec4(result, 1.0);\n"
    "}\n\0";

// Load model
void loadModel(const std::string& path, std::vector<float>& vbuffer, std::vector<float>& nbuffer, std::vector<float>& tbuffer) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
    for (const auto& id : shapes[0].mesh.indices) {
        int vid = id.vertex_index;
        int nid = id.normal_index;
        int tid = id.texcoord_index;
        
        //fill in vertex positions
        vbuffer.push_back(attrib.vertices[vid * 3]);
        vbuffer.push_back(attrib.vertices[vid * 3 + 1]);
        vbuffer.push_back(attrib.vertices[vid * 3 + 2]);
        
        //normal
        nbuffer.push_back(attrib.normals[nid * 3]);
        nbuffer.push_back(attrib.normals[nid * 3 + 1]);
        nbuffer.push_back(attrib.normals[nid * 3 + 2]);
        
        //tex coord
        tbuffer.push_back(attrib.texcoords[tid * 2]);
        tbuffer.push_back(attrib.texcoords[tid * 2 + 1]);
    }
}

//buffers, vertex at 0, normal at 1, texture at 2
void setupVAO(std::vector<float>& vbuffer, std::vector<float>& nbuffer, std::vector<float>& tbuffer, unsigned int& VAO, unsigned int* VBO) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(3, VBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vbuffer.size() * sizeof(float), vbuffer.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, nbuffer.size() * sizeof(float), nbuffer.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, tbuffer.size() * sizeof(float), tbuffer.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
};

// Load texture
unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    return textureID;
}

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    int windowWidth = 1024;
    int windowHeight = 768;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Assignment3", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "GLFW Window Failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "GLAD Initialization Failed" << std::endl;
        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    //shaders
    int success;
    char error_msg[512];
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vs, 512, NULL, error_msg);
        std::cout << "Vertex Shader Failed: " << error_msg << std::endl;
    }
    
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fs, 512, NULL, error_msg);
        std::cout << "Fragment Shader Failed: " << error_msg << std::endl;
    }
    
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, error_msg);
        std::cout << "Program Link Error: " << error_msg << std::endl;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    int MVPLoc = glGetUniformLocation(shaderProgram, "MVP");
    
    
    //load obj
    std::vector<float> timmyVBuffer, timmyNBuffer, timmyTBuffer;
    loadModel("/Users/yisiliu/development/3GC3/assignment3/assignment3/asset/timmy.obj", timmyVBuffer, timmyNBuffer, timmyTBuffer);
    
    std::vector<float> bucketVBuffer, bucketNBuffer, bucketTBuffer;
    loadModel("/Users/yisiliu/development/3GC3/assignment3/assignment3/asset/bucket.obj", bucketVBuffer, bucketNBuffer, bucketTBuffer);
    
    std::vector<float> floorVBuffer, floorNBuffer, floorTBuffer;
    loadModel("/Users/yisiliu/development/3GC3/assignment3/assignment3/asset/floor.obj", floorVBuffer, floorNBuffer, floorTBuffer);
    
    //texture
    unsigned int timmyTexture = loadTexture("/Users/yisiliu/development/3GC3/assignment3/assignment3/asset/timmy.png");
    
    unsigned int bucketTexture = loadTexture("/Users/yisiliu/development/3GC3/assignment3/assignment3/asset/bucket.jpg");
    
    unsigned int floorTexture = loadTexture("/Users/yisiliu/development/3GC3/assignment3/assignment3/asset/floor.jpeg");
    
    //buffers
    unsigned int timmyVAO, timmyVBO[3];
    setupVAO(timmyVBuffer, timmyNBuffer, timmyTBuffer, timmyVAO, timmyVBO);
    
    unsigned int bucketVAO, bucketVBO[3];
    setupVAO(bucketVBuffer, bucketNBuffer, bucketTBuffer, bucketVAO, bucketVBO);
    
    unsigned int floorVAO, floorVBO[3];
    setupVAO(floorVBuffer, floorNBuffer, floorTBuffer, floorVAO, floorVBO);
    
    //transformations
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(glm::vec3(50,100,200),glm::vec3(0,80,0), glm::vec3(0,1,0));
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(60.0f),4.0f / 3.0f, 0.1f, 1000.0f);
    glm::mat4 MVP = projection * view * model;
    
    //light settings
    glm::vec3 lightPos(0.0f, 200.0f, 0.0f); //all three spotlights are at the same position
    float theta = 0.0f;
    glm::vec3 ambientColor(0.2f, 0.2f, 0.2f);
    glm::vec3 diffuseColorR(1.0f, 0.0f, 0.0f);
    glm::vec3 diffuseColorG(0.0f, 1.0f, 0.0f);
    glm::vec3 diffuseColorB(0.0f, 0.0f, 1.0f);

    float cutOffCosine = cos(M_PI/6.0f);
    
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        
        //background color
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //shader
        glm::mat4 spinSpotLight = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(0.0f,1.0f,0.0f));
        glm::mat3 spin = glm::mat3(spinSpotLight);
        glm::vec3 spotDirR(50.0f, -200.0f, -50.0f);
        glm::vec3 spotDirG(-50.0f, -200.0f, -50.0f);
        glm::vec3 spotDirB(0.0f, -200.0f, 50.0f);
        spotDirR = spin * spotDirR;
        spotDirG = spin * spotDirG;
        spotDirB = spin * spotDirB;
        theta += 0.05f;
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, glm::value_ptr(MVP));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "ambientColor"), 1, glm::value_ptr(ambientColor));
        glUniform3fv(glGetUniformLocation(shaderProgram, "diffuseColorR"), 1, glm::value_ptr(diffuseColorR));
        glUniform3fv(glGetUniformLocation(shaderProgram, "diffuseColorG"), 1, glm::value_ptr(diffuseColorG));
        glUniform3fv(glGetUniformLocation(shaderProgram, "diffuseColorB"), 1, glm::value_ptr(diffuseColorB));
        glUniform3fv(glGetUniformLocation(shaderProgram, "spotDirR"), 1, glm::value_ptr(spotDirR));
        glUniform3fv(glGetUniformLocation(shaderProgram, "spotDirG"), 1, glm::value_ptr(spotDirG));
        glUniform3fv(glGetUniformLocation(shaderProgram, "spotDirB"), 1, glm::value_ptr(spotDirB));
        glUniform1f(glGetUniformLocation(shaderProgram, "cutOffCosine"), cutOffCosine);
        glUniform1i(glGetUniformLocation(shaderProgram, "myTexture"), 0);
        glActiveTexture(GL_TEXTURE0);
        
        // Draw
        glBindTexture(GL_TEXTURE_2D, timmyTexture);
        glBindVertexArray(timmyVAO);
        glDrawArrays(GL_TRIANGLES, 0, timmyVBuffer.size() / 3);
        
        glBindTexture(GL_TEXTURE_2D, bucketTexture);
        glBindVertexArray(bucketVAO);
        glDrawArrays(GL_TRIANGLES, 0, bucketVBuffer.size() / 3);
        
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, floorVBuffer.size() / 3);
        
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //release resource
    
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    
    return 0;
}
