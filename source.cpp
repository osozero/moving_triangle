#include <cstdio>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION // disable for os x

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp> // glm::vec3


void error_callback(int error, const char* message) {
    fprintf(stderr, "Error: %s\n", message);
}

int main(void)
{
    glm::vec3 translation(0.0f, 0.0f, 0.0f);
    glm::vec3 scale(1.0f, 1.0f, 1.0f);

    glfwSetErrorCallback(error_callback);
    
    GLFWwindow *window;

    //init the library

    if (!glfwInit()) {
        printf("GLFW can not be initialized!\n");
        return -1;
    }
    // osx things
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    

    // create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Hello Window", NULL, NULL);

    if (!window) {
        printf("no window...\n");
        glfwTerminate();
        return -1;
    }
    
    // make the window's context current

    glfwMakeContextCurrent(window);

    int version = gladLoadGL(glfwGetProcAddress);
                
    if (version == 0) {
        printf("Failed to initialize OpenGL context\n");
        return -1;
    } 

    const GLubyte* version2 = glGetString(GL_VERSION);
    printf("----OpenGL version: %s\n", version2);

    /*********** Preparing OpenGL buffers, shaders etc *****************/
    float points[] = {
        0.2f,  0.2f,  0.0f,
        0.2f, -0.2f,  0.0f,
        -0.2f, -0.2f,  0.0f
    };

    // vertex buffer
    GLuint vbo = 0;
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

    // vertex array
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    //shaders

    FILE *f = fopen("vertex_shader.glsl", "r");

    if (!f) {
        printf("vertex_shader.glsl file could not be read\n");
        glfwTerminate();
        exit(-1);
    }

    fseek(f, 0, SEEK_END);
    int file_len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* vertex_shader = (char*)malloc(file_len * sizeof(char));

    if (!vertex_shader) {
        printf("malloc error\n");
        glfwTerminate();
        exit(-1);
    }

    fread(vertex_shader, 1, file_len, f);

    fclose(f);
    
    const char* fragment_shader =
        "#version 410\n"
        "out vec4 frag_colour;"
        "void main() {"
        "  frag_colour = vec4(1.0, 0.5, 1.0, 0.5);"
        "}";
    

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    GLuint  fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);

    GLint translation_loc = glGetUniformLocation(shader_program, "translation");
    GLint scale_loc = glGetUniformLocation(shader_program, "scale");

/******************************************************************/

    glfwSwapInterval(1);

    int width, height;

    // loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
            continue;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            translation.y += 0.005f;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            translation.y -= 0.005f;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            translation.x -= 0.005f;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            translation.x += 0.005f;
        }

        // scale keys

        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            scale.x += 0.005f;
            scale.y += 0.005f;
            scale.z += 0.005f;
        }

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            scale.x -= 0.005f;
            scale.y -= 0.005f;
            scale.z -= 0.005f;
        }
        
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);

        glUniform3f(translation_loc, translation.x, translation.y, translation.z);

        glUniform3f(scale_loc, scale.x, scale.y, scale.z);

        
        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        // swap front and back buffers
        glfwSwapBuffers(window);

        // poll for and process events

        glfwPollEvents();
    }

    glfwTerminate();
    
    return 0;
}
