#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <tiny_obj_loader.h>

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
        dump_framebuffer_to_ppm("/Users/yisiliu/development/3GC3/assignment2/assignment2/Assignment2-ss", buffer_width, buffer_height);
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
    "out vec3 Normal;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   Normal = aNormal;\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec3 FragColor;\n"
    "in vec3 Normal;\n"
    "void main()\n"
    "{\n"
    "   float c = dot(Normal, vec3(0.0,0.0,1.0));\n"
    "   FragColor = vec3(c,c,c);\n"
    "}\n\0";

void loadObjModel(const std::string& obj_path, std::vector<tinyobj::shape_t>& shapes, tinyobj::attrib_t& attrib)
{
    std::vector<tinyobj::material_t> materials;
    // tinyobj load obj
    std::string warn, err;
    
    bool bTriangulate = true;
    bool bSuc = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, obj_path.c_str(), nullptr, bTriangulate);
    if (!bSuc){
        std::cout << "tinyobj error: " << err.c_str() << std::endl;
        return;
    }
}

void pushBuffers(const std::vector<tinyobj::shape_t>& shapes, const tinyobj::attrib_t& attrib, std::vector<float>& vertices, std::vector<float>& normals){
    for (auto face : shapes[0].mesh.indices)
    {
        int vid = face.vertex_index;
        int nid = face.normal_index;
        
        //fill in vertex buffer with vertex positions
        vertices.push_back(attrib.vertices[vid * 3]);//vertex vid’s x
        vertices.push_back(attrib.vertices[vid * 3 + 1]);//vertex vid’s y
        vertices.push_back(attrib.vertices[vid * 3 + 2]);//vertex vid’s z
        
        //fill in normal buffer with normal directions
        normals.push_back(attrib.normals[nid * 3]);
        normals.push_back(attrib.normals[nid * 3 + 1]);
        normals.push_back(attrib.normals[nid * 3 + 2]);
    }
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
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Assignment2", NULL, NULL);

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
    
    int modelLoc = glGetUniformLocation(shaderProgram, "model");
    int viewLoc = glGetUniformLocation(shaderProgram, "view");
    int projLoc = glGetUniformLocation(shaderProgram, "projection");
    
    //load shapes
    //base
    std::string obj_path = "/Users/yisiliu/development/3GC3/assignment2/assignment2/data/faces/base.obj";
    std::vector<tinyobj::shape_t> shapes;
    tinyobj::attrib_t attrib;
    loadObjModel(obj_path, shapes, attrib);
    
    //0 to 34
    std::vector<tinyobj::shape_t> blendshapes[35];
    tinyobj::attrib_t blendattribs[35];
    for (int i = 0; i <= 34; i++) {
        std::string bs_obj_path = "/Users/yisiliu/development/3GC3/assignment2/assignment2/data/faces/";
        std::string filename = std::to_string(i);
        std::string filetype = ".obj";
        bs_obj_path += filename;
        bs_obj_path += filetype;
        
        loadObjModel(bs_obj_path, blendshapes[i], blendattribs[i]);
    }
    
    //blend shapes
    std::ifstream inFile;
    inFile.open("/Users/yisiliu/development/3GC3/assignment2/assignment2/data/weights/5.weights");
    std::string input;
    //read data from .weights and transfer string to float
    std::vector<float> weights;
    while (getline(inFile, input)) {
        weights.push_back(std::stof(input));
    }
    inFile.close();
    
    unsigned long vertices_size = attrib.vertices.size();
    std::vector<float> blendVertices = attrib.vertices;
    for (unsigned i = 0; i <= 34; i++)
    {
        assert(vertices_size == blendattribs[i].vertices.size());
        for (unsigned j = 0; j < vertices_size; j++)
        {
            blendVertices[j] += weights[i] * (blendattribs[i].vertices[j] - attrib.vertices[j]);
        }
    }
    attrib.vertices = blendVertices;
    
    //buffers
    std::vector<float> vertices, normals;
    pushBuffers(shapes, attrib, vertices, normals);
    GLuint VBO[2], VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
 
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//GL_FILL or GL_LINE
    
    //transformations
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(glm::vec3(0, 0.1, 0.5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(60.0f),4.0f/3.0f, 0.1f, 10.0f);
    
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        
        //background color
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUseProgram(shaderProgram);
        
        // Draw
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }
    //release resource
    
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    
    return 0;
}
