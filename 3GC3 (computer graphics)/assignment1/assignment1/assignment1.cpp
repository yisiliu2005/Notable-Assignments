#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>

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
        dump_framebuffer_to_ppm("/Users/yisiliu/development/3GC3/assignment1/assignment1/Assignment1-ss", buffer_width, buffer_height);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//rotation angle functions
float get_sun_rotate_angle_around_itself(float day) {
    float degrees_per_day = 360.0f/27;
    return day * degrees_per_day;
}

float get_earth_rotate_angle_around_sun(float day) {
    float degrees_per_day = 360.0f/365;
    return day * degrees_per_day;
}

float get_earth_rotate_angle_around_itself(float day) {
    float degrees_per_day = 360.0f; //divided by 1 day
    return day * degrees_per_day;
}

float get_moon_rotate_angle_around_earth(float day) {
    float degrees_per_day = 360.0f/28;
    return day * degrees_per_day;
}

float get_moon_rotate_angle_around_itself(float day) {
    float degrees_per_day = 360.0f/28; //actually the same as moon orbit
    return day * degrees_per_day;
}

//shader
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0f);\n"
    "}\n\0";


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    int windowWidth = 1024;
    int windowHeight = 576;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Assignment1", NULL, NULL);

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
    
    float vertices[] = {
        // positions         // colors
        -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // A red
        0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  // B red
        0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,  // E red
        
        0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  // B green
        1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // C green
        0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  // E green
        
        1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // C blue
        0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f,  // D blue
        0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  // E blue
        
        0.0f, 0.0f, -1.0f,  0.5f, 0.5f, 0.5f,  // D grey
        -1.0f, 0.0f, 0.0f,  0.5f, 0.5f, 0.5f,  // A grey
        0.0f, 1.0f, 0.0f,  0.5f, 0.5f, 0.5f,  // E grey
        
        0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f,  // B fushia
        -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 1.0f,  // A fushia
        0.0f, -1.0f, 0.0f,  1.0f, 0.0f, 1.0f,  // F fushia
        
        1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,  // C yellow
        0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 0.0f,  // B yellow
        0.0f, -1.0f, 0.0f,  1.0f, 1.0f, 0.0f,  // F yellow

        0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f,  // D aqua
        1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 1.0f,  // C aqua
        0.0f, -1.0f, 0.0f,  0.0f, 1.0f, 1.0f,  // F aqua
        
        -1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,  // A white
        0.0f, 0.0f, -1.0f,  1.0f, 1.0f, 1.0f,  // D white
        0.0f, -1.0f, 0.0f,  1.0f, 1.0f, 1.0f  // F white
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);

    float day = 0;
    float inc = 1.0f/24;
    
    //glm::vec3 earth_pos = glm::vec3(30,0,0);
    glm::vec3 moon_pos = glm::vec3(45,0,0);
    
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        //background color
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //shader
        glUseProgram(shaderProgram);
        
        //transformations
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        
        view = glm::lookAt(glm::vec3(30.0f, 20.0f, 90.0f), //cam pos
                     moon_pos, //looking at
                     glm::vec3(0.0f, 1.0f, 0.0f)); //the upwards dir
        projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 1000.0f);
        
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        
        glBindVertexArray(VAO);
        
        //model:
        //sun
        model = glm::rotate(model, glm::radians(get_sun_rotate_angle_around_itself(day)), glm::vec3(0,1,0));
        model = glm::scale(model, glm::vec3(9.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 24);
        
        //earth
        model = glm::mat4(1.0f);
        //4. spin radius 30
        //glm::rotate 0,1,0
        model = glm::rotate(model, glm::radians(get_earth_rotate_angle_around_sun(day)), glm::vec3(0,1,0));
        model = glm::translate(model, glm::vec3(30,0,0));
        model = glm::rotate(model, glm::radians(-get_earth_rotate_angle_around_sun(day)), glm::vec3(0,1,0));

        //3. self revolve
        glm::vec3 earth_revolve_axis = glm::normalize(glm::vec3(glm::cos(glm::radians(66.6f)), glm::sin(glm::radians(66.6f)), 0));  //finding tilted rotation axis
        model = glm::rotate(model, glm::radians(get_earth_rotate_angle_around_itself(day)), earth_revolve_axis);
        
        //2. tilt
        model = glm::rotate(model, glm::radians(-23.4f), glm::vec3(0,0,1));
        
        //1. scale 5
        model = glm::scale(model, glm::vec3(5.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 24);
        
        //earth_pos = glm::vec3(model[3]);
        
        //moon
        model = glm::mat4(1.0f);
        //4. spin radius 30 (copy earth)
        //glm::rotate 0,1,0 angle same as where earth is
        model = glm::rotate(model, glm::radians(get_earth_rotate_angle_around_sun(day)), glm::vec3(0,1,0));
        model = glm::translate(model, glm::vec3(30,0,0));
        model = glm::rotate(model, glm::radians(-get_earth_rotate_angle_around_sun(day)), glm::vec3(0,1,0));
        
        //3. spin radius 15
        model = glm::rotate(model, glm::radians(get_moon_rotate_angle_around_earth(day)), glm::vec3(0,1,0));
        model = glm::translate(model, glm::vec3(15,0,0));
        model = glm::rotate(model, glm::radians(-get_moon_rotate_angle_around_earth(day)), glm::vec3(0,1,0));
        
        //2. self revolve
        model = glm::rotate(model, glm::radians(get_moon_rotate_angle_around_itself(day)), glm::vec3(0,1,0));
        
        //1. scale 3
        model = glm::scale(model, glm::vec3(3.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 24);

        moon_pos = glm::vec3(model[3]);
        
        day += inc;
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //release resource
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    
    return 0;
}
