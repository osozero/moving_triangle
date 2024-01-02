#include <cstdio>
#include <math.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>

#define GLFW_INCLUDE_GLCOREARB

#define GL_SILENCE_DEPRECATION // disable for os x


#include <GLFW/glfw3.h>

//#include <glm/vec3.hpp> // glm::vec3
//#include <glm/mat4x4.hpp>
//#include <glm/gtc/type_ptr.hpp>


void error_callback(int error, const char* message) {
    fprintf(stderr, "Error: %s\n", message);
  }

typedef struct {
    float x;
    float y;
    float z;
} vec3;

typedef struct {
    float x;
    float y;
    float z;
    float w;
}vec4;

typedef struct {
    float m[4][4];

}mat4;


int main(void)
{
    vec3 translation = {0.0f, 0.0f, 0.0f};
    vec3 scale = {1.0f, 1.0f, 1.0f};

    mat4 z_rotation_matrix = { .m = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    }};
    
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
    window = glfwCreateWindow(640, 480, "Sonic Loop :)", NULL, NULL);

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

    GLint rotation_matrix_loc = glGetUniformLocation(shader_program, "z_rotation_matrix");

/******************************************************************/

    glfwSwapInterval(1);

    int width, height;
    float  theta = 0.0f;
    // loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // rotation on Z axis
        theta += 0.01;

        float cos_theta = cos(theta);
        float sin_theta = sin(theta);

        z_rotation_matrix.m[0][0] = cos_theta;
        z_rotation_matrix.m[0][1]= -1 * (sin_theta);
        z_rotation_matrix.m[1][0]= sin_theta;
        z_rotation_matrix.m[1][1] = cos_theta;
                    
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
            continue;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            if (translation.y < 1.0f) {
                translation.y += 0.005f;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            if (translation.y > -1.0f) {
                translation.y -= 0.005f;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            if (translation.x > -1.0f) {
                translation.x -= 0.005f;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            if (translation.x < 1.0f) {
                translation.x += 0.005f;
            }
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
        
        // render here
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);

        glUniform3f(translation_loc, translation.x, translation.y, translation.z);
        glUniform3f(scale_loc, scale.x, scale.y, scale.z);

        //glUniformMatrix4fv(rotation_matrix_loc, 1, GL_FALSE, glm::value_ptr(z_rotation_matrix));
        glUniformMatrix4fv(rotation_matrix_loc, 1, GL_FALSE, (GLfloat*)&(z_rotation_matrix.m));
                
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
