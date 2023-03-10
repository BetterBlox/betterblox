#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "shader.hpp"
#include "camera.hpp"
#include "stb_image.h"
#include <map>
#include <unordered_set>
#include <cmath>
#include "Block.hpp"
#include <algorithm>
#include <vector>
#include "inventory.hpp"
//#include <irrKlang/irrKlang.h>
#include <random>
#include "perlin.hpp"

#define SHADER_HEADER "#version 330 core\n"
#define SHADER_STR(x) #x
// checking version control



void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    (void)window;
}
void error_callback(int error, const char* msg) {
    std::string s;
    s = " [" + std::to_string(error) + "] " + msg + '\n';
    std::cerr << s << std::endl;
}
void processInput(GLFWwindow* window, int& combine, float& xOffset, float& yOffset);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mygl_GradientBackground(float top_r, float top_g, float top_b, float top_a,
    float bot_r, float bot_g, float bot_b, float bot_a);
int placeCube(glm::vec3, std::unordered_set<Block>&, int);
bool checkDuplicates(glm::vec3, glm::vec3);
void loadTexture(unsigned int& texture, std::string path, unsigned int type, unsigned int rgbType);
void updateTerrain(int startPosx, int startPosz);

const unsigned int SCR_WIDTH = 2200;
const unsigned int SCR_HEIGHT = 1200;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Setting up the player information. 

Inventory inventory(10);

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

std::unordered_set<Block> cubePositions;

// Settings
float waterLevel = 5;

int main() {
    // Opengl treats the 0,0 locations on images to be the bottom. This flips the images so the 0, 0 will be at the top.
    stbi_set_flip_vertically_on_load(true);

    // Setting up the window stuff
    glfwSetErrorCallback(error_callback);

    // Setup the music
    // irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();

    if (GL_TRUE != glfwInit())
        std::cerr << "Failed to init" << std::endl;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #if __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Deep", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return 1;
    }
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // OPENGL stuff
    glEnable(GL_DEPTH_TEST);

    float dot[] = {
        -0.005f,  0.05f,
        -0.005f, -0.05f,
         0.005f,  0.05f,
         0.005f, -0.05f,
         0.005f,  0.05f,
        -0.005f, -0.05f,
         0.05f, -0.005f,
        -0.05f, -0.005f,
         0.05f,  0.005f,
        -0.05f,  0.005f,
         0.05f,  0.005f,
        -0.05f, -0.005f
         
    };

    float inventorySquare[] = {
        -0.2f,  0.2f, 0.0f, 1.0f, // Top Left
        -0.2f, -0.2f, 0.0f, 0.0f, // Bottom Left
         0.2f,  0.2f, 1.0f, 1.0f, // Top Right
         0.2f, -0.2f, 1.0f, 0.0f, // Bottom Right
         0.2f,  0.2f, 1.0f, 1.0f, // Top Right
        -0.2f, -0.2f, 0.0f, 0.0f // Bottom Left

    };


    float cube[] = {
-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,

-0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
 0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,

-0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

-0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
-0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };

    unsigned int inventoryVbo, inventoryVao;
    glGenBuffers(1, &inventoryVbo);
    glGenVertexArrays(1, &inventoryVao);

    glBindVertexArray(inventoryVao);
    glBindBuffer(GL_ARRAY_BUFFER, inventoryVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(inventorySquare), inventorySquare, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    unsigned int vboDot, vaoDot;
    glGenBuffers(1, &vboDot);
    glGenVertexArrays(1, &vaoDot);

    glBindVertexArray(vaoDot);
    glBindBuffer(GL_ARRAY_BUFFER, vboDot);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dot), dot, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    

    const unsigned int numTriangles = 1;
    unsigned int VBO[numTriangles], VAO[numTriangles];

    glGenBuffers(numTriangles, VBO);
    glGenVertexArrays(numTriangles, VAO);

    glBindVertexArray(VAO[0]);
    // Binding the VBO to the
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);




    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    int width, height, nrChannels;
    unsigned char* data = stbi_load("c:/users/ethan/Documents/container.jpg", &width, &height, &nrChannels, 0);

    unsigned int containerTexture, dimondOreTexture, awesomeFaceTexture, bedrockTexture, grassTexture, waterTexture;

    loadTexture(containerTexture, "assets/textures/container.jpg", GL_LINEAR, GL_RGB);
    loadTexture(dimondOreTexture, "assets/textures/diamonds.png", GL_LINEAR, GL_RGBA);
    loadTexture(awesomeFaceTexture, "assets/textures/awesomeface.png", GL_LINEAR, GL_RGBA);
    loadTexture(bedrockTexture, "assets/textures/bedrock.png", GL_LINEAR, GL_RGB);
    loadTexture(grassTexture, "assets/textures/grass.jpg", GL_LINEAR, GL_RGB);
    loadTexture(waterTexture, "assets/textures/water.png", GL_LINEAR, GL_RGBA);


    //  ######################### Defining shaders for the program ############################## //

    Shader shader("assets/shaders/vertexShader1.glsl", "assets/shaders/fragmentShader1.glsl");
    Shader dotShader("assets/shaders/vertForPointer.glsl", "assets/shaders/fragForPointer.glsl");
    Shader blockShader("assets/shaders/vertexShader1.glsl", "assets/shaders/blockShader.glsl");
    Shader inventoryShader("assets/shaders/vertForInventoryMenu.glsl", "assets/shaders/fragForInventoryMenu.glsl");

    int combine = 0;
    float xOffset = 0;
    float yOffset = 0;

    // for text and menu rendering 
    glm::mat4 view2D = glm::mat4(1.0f);

    glm::mat4 projection2D = glm::mat4(1.0f);
    projection2D = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);


//    soundEngine->play2D("c:/users/ethan/Documents/softaudio.mp3");


    // Texture binding
    glActiveTexture(GL_TEXTURE0 + CONTAINER);
    glBindTexture(GL_TEXTURE_2D, containerTexture);
    glActiveTexture(GL_TEXTURE0 + DIAMOND_ORE);
    glBindTexture(GL_TEXTURE_2D, dimondOreTexture);
    glActiveTexture(GL_TEXTURE0 + HAPPY_FACE);
    glBindTexture(GL_TEXTURE_2D, awesomeFaceTexture);
    glActiveTexture(GL_TEXTURE0 + BEDROCK);
    glBindTexture(GL_TEXTURE_2D, bedrockTexture);
    glActiveTexture(GL_TEXTURE0 + GRASS);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glActiveTexture(GL_TEXTURE0 + WATER);
    glBindTexture(GL_TEXTURE_2D, waterTexture);

    
    

    while (!glfwWindowShouldClose(window))
    {
        updateTerrain((int) camera.getPosition().x, (int) camera.getPosition().z);
        
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        processInput(window, combine, xOffset, yOffset);

        // rendering commands here
        glClearColor(0.2f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mygl_GradientBackground(0.5, 0.8, 0.9, 1.0,
            0.8, 0.8, 0.9, 1.0);





        // Activate and configure shader
        blockShader.use();

        // Creating transformations
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first. That is important for some reason.
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();


        // Set transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // retrieve the matrix uniform locations
        unsigned int viewLoc = glGetUniformLocation(blockShader.getId(), "view");
        unsigned int projectionLoc = glGetUniformLocation(blockShader.getId(), "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // note: Projection matrix rarely changes so it should be set outside of the loop.
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO[0]);
        unsigned int modelLoc;
        blockShader.use();

        model = glm::mat4(1.0f);
        modelLoc = glGetUniformLocation(blockShader.getId(), "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                
        std::unordered_set<Block>::iterator itr;
        for (itr = cubePositions.begin(); itr != cubePositions.end(); itr++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, itr->getPosition());
            modelLoc = glGetUniformLocation(blockShader.getId(), "model");
            blockShader.setInt("texture2", ((Block)*itr).getBlockType());
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(((float)((float)SCR_HEIGHT / (float)SCR_WIDTH)), 1.0f, 1.0f));

        dotShader.use();
        glBindVertexArray(vaoDot);
        int mLoc = glGetUniformLocation(dotShader.getId(), "model");
        glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 12);


        // Draw the inventory here.
        // inventoryShader.use();
        model = glm::translate(model, glm::vec3(-0.8f, -0.8f, 0.0f));
        inventoryShader.use();
        for (int i = 0; i < 4; i++) {
            inventoryShader.setInt("texturein", (i));
            model = glm::translate(model, glm::vec3(0.6f, 0.0f, 0.0f));
            glBindVertexArray(inventoryVao);
            int projection2DLoc = glGetUniformLocation(inventoryShader.getId(), "projection");
            int model2dLoc = glGetUniformLocation(inventoryShader.getId(), "model");
            int view2dLoc = glGetUniformLocation(inventoryShader.getId(), "view");
            glUniformMatrix4fv(projection2DLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(model2dLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(view2dLoc, 1, GL_FALSE, glm::value_ptr(camera.GetViewMatrix()));
            
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        
        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

//    soundEngine->drop();


    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window, int& combine, float& xOffset, float& yOffset)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
        combine = HAPPY_FACE;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        combine = CONTAINER;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        combine = DIAMOND_ORE;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        combine = BEDROCK;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        combine = GRASS;
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        combine = 5;
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
        combine = 9;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        xOffset += 0.01;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        xOffset -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        yOffset += 0.01;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        yOffset -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if (glfwGetMouseButton(window, 0 == GLFW_PRESS)) {
        placeCube(camera.getPosition() + (camera.getFront() * 5.0f), cubePositions, combine);
        // Place a cube at the location based on the camera position. 
        std::cout << "num of cubes if " << cubePositions.size() << std::endl;
    }
        
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// got this from github: https://gist.github.com/mhalber/0a9b8a78182eb62659fc18d23fe5e94e
void mygl_GradientBackground(float top_r, float top_g, float top_b, float top_a,
    float bot_r, float bot_g, float bot_b, float bot_a)
{
    glDisable(GL_DEPTH_TEST);

    static GLuint background_vao = 0;
    static GLuint background_shader = 0;

    if (background_vao == 0)
    {
        glGenVertexArrays(1, &background_vao);

        const char* vs_src = (const char*)SHADER_HEADER SHADER_STR
        (
            out vec2 v_uv;
        void main()
        {
            uint idx = uint(gl_VertexID);
            gl_Position = vec4(idx & 1U, idx >> 1U, 0.0, 0.5) * 4.0 - 1.0;
            v_uv = vec2(gl_Position.xy * 0.5 + 0.5);
        }
        );

        const char* fs_src = (const char*)SHADER_HEADER SHADER_STR
        (
            uniform vec4 top_color;
        uniform vec4 bot_color;
        in vec2 v_uv;
        out vec4 frag_color;

        void main()
        {
            frag_color = bot_color * (1 - v_uv.y) + top_color * v_uv.y;
        }
        );
        GLuint vs_id, fs_id;
        vs_id = glCreateShader(GL_VERTEX_SHADER);
        fs_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vs_id, 1, &vs_src, NULL);
        glShaderSource(fs_id, 1, &fs_src, NULL);
        glCompileShader(vs_id);
        glCompileShader(fs_id);
        background_shader = glCreateProgram();
        glAttachShader(background_shader, vs_id);
        glAttachShader(background_shader, fs_id);
        glLinkProgram(background_shader);
        glDetachShader(background_shader, fs_id);
        glDetachShader(background_shader, vs_id);
        glDeleteShader(fs_id);
        glDeleteShader(vs_id);
        glUseProgram(background_shader);
    }

    glUseProgram(background_shader);
    GLuint top_color_loc = glGetUniformLocation(background_shader, "top_color");
    GLuint bot_color_loc = glGetUniformLocation(background_shader, "bot_color");
    glUniform4f(top_color_loc, top_r, top_g, top_b, top_a);
    glUniform4f(bot_color_loc, bot_r, bot_g, bot_b, bot_a);

    glBindVertexArray(background_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
}

int placeCube(glm::vec3 position, std::unordered_set<Block> &positions, int blockType) {
    std::unordered_set<Block>::iterator ip;
    position.x = (float)round(position.x);
    position.y = (float)round(position.y);
    position.z = (float)round(position.z);
    
    positions.insert(Block(position, blockType));
    
    return 0;
}

bool checkDuplicates(glm::vec3 i, glm::vec3 j) {

    return i == j;
}

void loadTexture(unsigned int& texture, std::string path, unsigned int type, unsigned int rgbType = GL_RGB) {

    glGenTextures(1, &texture);
    int width, height, nrChannels;
    unsigned char * data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (!data) {
        std::cout << "Failed to load data" << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, type);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, rgbType, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void updateTerrain(int startPosx, int startPosz) {
    for (int i = startPosx; i < startPosx + 20; i++) {
        for (int j = startPosz; j < startPosz + 20; j++) {
            float h = perlin((float)i * 0.15f, (float)j * 0.15f);
                if (h > waterLevel)
                    placeCube(glm::vec3(i, h, j), cubePositions, BEDROCK);
                else
                    placeCube(glm::vec3(i, waterLevel, j), cubePositions, WATER);
        }
    }
}