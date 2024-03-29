// Definitions
#define STB_IMAGE_IMPLEMENTATION
#define SHADER_HEADER "#version 330 core\n"
#define SHADER_STR(x) #x

// Dependencies
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// STL
#include <cmath>
#include <iostream>
#include <random>
#include <stack>
#include <string>
#include <unordered_set>
#include <chrono>

// Header Files
#include "Block.hpp"
#include "Camera.hpp"
#include "ChunkLoader.hpp"
#include "Inventory.hpp"
#include "perlin.hpp"
#include "Shader.hpp"
#include "stb_image.h"

// Utilities
#include "utils/RuntimeError.hpp"

class BetterBlox {
private:
    // Constants
    const unsigned int SCR_WIDTH = 2200;
    const unsigned int SCR_HEIGHT = 1200;

    // Since VAO and VBO arrays need this before compile time, we have to use `static constexpr`
    // to initialize it at compile time.
    static constexpr unsigned int NUM_TRIANGLES = 1;

    Camera camera; // This can also be thought of as the player.

    std::unordered_set<Block> block_rendering; // Were the render blocks are stored

    std::map<std::string, std::unordered_set<Block>> local_block_data;

    float last_x = SCR_WIDTH / 2.0f;
    float last_y = SCR_HEIGHT / 2.0f;

    bool first_mouse = true;
    std::unordered_set<Block> cube_positions;
    std::stack<std::pair<int, int> > render;

    GLFWwindow *window; // Check BetterBlox::initialize() for initialization

    int combine; // Used for getting user input

    float x_offset;
    float y_offset;

    unsigned int inventory_vao;
    unsigned int inventory_vbo;
    unsigned int vao_dot;
    unsigned int vbo_dot;
    unsigned int VAO[NUM_TRIANGLES];
    unsigned int VBO[NUM_TRIANGLES];

    // Player Information
    Inventory inventory;

    // Timing
    float delta_time = 0.0f;
    float last_frame = 0.0f;
    std::chrono::system_clock::time_point last_call_time = std::chrono::system_clock::from_time_t(0);


    // Shaders
    // These need to be pointers as they do not have a default constructor.
    // Pointers MUST be set to nullptr, else exceptions will not work!
    Shader *shader = nullptr;
    Shader *dot_shader = nullptr;
    Shader *block_shader = nullptr;
    Shader *inventory_shader = nullptr;

    // MultiThreading
    // std::thread chunk_thread;
    // std::thread read_thread;

    // Settings
    int render_distance = 3;
    int buffer = 1;
    bool show_inventory_menu = false;

    // Function Prototypes
    /**
     * This sets up GLFW to display a window that will work for OpenGL. Then it creates the block geometry
     * It adds textures and configures all OpenGL settings for startup.
     */
    void initialize();

    /**
     * This is stuff that happens every frame. Updating locations of objects and getting user input.
     */
    void updateFrame();

    // These functions need to be static to be able to pass them to GLFW.
    static void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
    static void errorCallback(int error, const char *msg);

    /**
     * This is for getting the user input from keyboard and mouse and modifying certain values in the program.
     * @param window
     * @param combine
     * @param x_offset
     * @param y_offset
     * @param chunk_rendering
     */
    void processInput(GLFWwindow *window, int &combine, float &x_offset, float &y_offset, std::map<std::string, std::unordered_set<Block> >& chunk_rendering, std::chrono::system_clock::time_point&);

    // Static wrapper functions are needed to pass these member functions to GLFW since they access other members.
    /**
     * for mouse actions such as panning
     */
    void mouseCallback(double x_pos_in, double y_pos_in);
    static void mouseCallbackStatic(GLFWwindow *window, double x_pos_in, double y_pos_in);

    /**
     * This was used to start with but is not very relevant in the final release
     * It allows the mouse wheel to zoom in
     * @param x_offset
     * @param y_offset
     */
    void scrollCallback(double x_offset, double y_offset);
    static void scrollCallbackStatic(GLFWwindow *window, double x_offset, double y_offset);

    /**
     * This is a way of creating a gradient background on the window. It takes rgba values for the top and bottom of the screen
     * @param top_r
     * @param top_g
     * @param top_b
     * @param top_a
     * @param bot_r
     * @param bot_g
     * @param bot_b
     * @param bot_a
     */
    void myglGradientBackground(float top_r, float top_g, float top_b, float top_a, float bot_r, float bot_g, float bot_b,
                                 float bot_a);

    /**
     * This is used to make texture loading easier. It takes a bunch of lines and breaks them down into several parameters.
     * @param texture The id of the texture that the image is being imported for.
     * @param path The path to the image file
     * @param type How the image pixels will be used. Linear will take the actual pixel value and gradient will take the average of the pixels
     * @param rgb_type RGB or RGBA
     */
    void loadTexture(unsigned int &texture, std::string path, unsigned int type, unsigned int rgb_type);

public:
    ~BetterBlox();
    void run();
};

// This deconstructor can be removed if Shader gets a default constructor.
BetterBlox::~BetterBlox() {
    delete shader;
    delete dot_shader;
    delete block_shader;
    delete inventory_shader;
}

void BetterBlox::run() {
    initialize();
    while(!glfwWindowShouldClose(window)) {
        updateFrame();
    }

    glfwTerminate(); // We could probably have a terminate function.
}

void BetterBlox::initialize() {
    // CoolDown
    const int cooldown_duration = 5;

    camera = Camera(glm::vec3(0.0f, 11.0f, 3.0f));
    inventory = Inventory(10);

    // Opengl treats the 0,0 locations on images to be the bottom. This flips the images so the 0, 0 will be at the top.
    stbi_set_flip_vertically_on_load(true);

    // Setting up the window stuff
    glfwSetErrorCallback(errorCallback);

    if (GL_TRUE != glfwInit()) {
        throw RuntimeError("Failed to initialize GLFW.", __FILE__, __LINE__);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif


    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "BetterBlox", glfwGetPrimaryMonitor(), NULL);
    if (window == NULL) {
        glfwTerminate();
        throw RuntimeError("Failed to create GLFW window.", __FILE__, __LINE__);
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallbackStatic);
    glfwSetScrollCallback(window, scrollCallbackStatic);

    glfwSetWindowUserPointer(window, this);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw RuntimeError("Failed to initialize GLAD.", __FILE__, __LINE__);
    }
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    // OPENGL stuff
    glEnable(GL_DEPTH_TEST);

    // This line prevents the CLion formatter from formatting this. Keeps it nicely aligned.
    // @formatter:off
    float dot[] = {
            -0.005f,  0.05f,
            -0.005f, -0.05f,
             0.005f,  0.05f,
             0.005f, -0.05f,
             0.005f,  0.05f,
            -0.005f, -0.05f,
             0.05f,  -0.005f,
            -0.05f,  -0.005f,
             0.05f,   0.005f,
            -0.05f,   0.005f,
             0.05f,   0.005f,
            -0.05f,  -0.005f
    };

    float inventory_square[] = {
            -0.2f,  0.2f, 0.0f, 1.0f, // Top Left
            -0.2f, -0.2f, 0.0f, 0.0f, // Bottom Left
             0.2f,  0.2f, 1.0f, 1.0f, // Top Right
             0.2f, -0.2f, 1.0f, 0.0f, // Bottom Right
             0.2f,  0.2f, 1.0f, 1.0f, // Top Right
            -0.2f, -0.2f, 0.0f, 0.0f  // Bottom Left
    };

    float inventoryMenuBackground[] = {
            -1, 1, 0, 1,
            -1, -1, 0, 0,
            1, 1, 1, 1,
            1, -1, 1, 0,
            1, 1, 1, 1,
            -1, -1, 0, 0
    };

    float cube[] = {
            -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
             0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
             0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,

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

            -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
             0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
             0.5f, 0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
             0.5f, 0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    // @formatter:on

    glGenBuffers(1, &inventory_vbo);
    glGenVertexArrays(1, &inventory_vao);

    glBindVertexArray(inventory_vao);
    glBindBuffer(GL_ARRAY_BUFFER, inventory_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(inventory_square), inventory_square, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vbo_dot);
    glGenVertexArrays(1, &vao_dot);

    glBindVertexArray(vao_dot);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_dot);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dot), dot, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    //VAO AND VBO STUFF HERE

    glGenBuffers(NUM_TRIANGLES, VBO);
    glGenVertexArrays(NUM_TRIANGLES, VAO);

    glBindVertexArray(VAO[0]);
    // Binding the VBO to the
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);




    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // TODO: Texture loading should be reworked, we shouldn't need 3 lines of code for each texture
    unsigned int container_texture, diamond_ore_texture, awesome_face_texture, bedrock_texture, grass_texture, water_texture;

    // Texture loading
    loadTexture(container_texture, "assets/textures/container.jpg", GL_LINEAR, GL_RGB);
    loadTexture(diamond_ore_texture, "assets/textures/diamonds.png", GL_LINEAR, GL_RGBA);
    loadTexture(awesome_face_texture, "assets/textures/awesomeface.png", GL_LINEAR, GL_RGBA);
    loadTexture(bedrock_texture, "assets/textures/bedrock.png", GL_LINEAR, GL_RGB);
    loadTexture(grass_texture, "assets/textures/grass.jpg", GL_LINEAR, GL_RGB);
    loadTexture(water_texture, "assets/textures/water.png", GL_LINEAR, GL_RGBA);

    // Texture binding
    glActiveTexture(GL_TEXTURE0 + CONTAINER);
    glBindTexture(GL_TEXTURE_2D, container_texture);
    glActiveTexture(GL_TEXTURE0 + DIAMOND_ORE);
    glBindTexture(GL_TEXTURE_2D, diamond_ore_texture);
    glActiveTexture(GL_TEXTURE0 + HAPPY_FACE);
    glBindTexture(GL_TEXTURE_2D, awesome_face_texture);
    glActiveTexture(GL_TEXTURE0 + BEDROCK);
    glBindTexture(GL_TEXTURE_2D, bedrock_texture);
    glActiveTexture(GL_TEXTURE0 + GRASS);
    glBindTexture(GL_TEXTURE_2D, grass_texture);
    glActiveTexture(GL_TEXTURE0 + WATER);
    glBindTexture(GL_TEXTURE_2D, water_texture);

    // Shader loading
    shader = new Shader("assets/shaders/vertexShader1.glsl", "assets/shaders/fragmentShader1.glsl");
    dot_shader = new Shader("assets/shaders/vertForPointer.glsl", "assets/shaders/fragForPointer.glsl");
    block_shader = new Shader("assets/shaders/vertexShader1.glsl", "assets/shaders/blockShader.glsl");
    inventory_shader = new Shader("assets/shaders/vertForInventoryMenu.glsl", "assets/shaders/fragForInventoryMenu.glsl");

    combine = 0;
    x_offset = 0;
    y_offset = 0;
}

void BetterBlox::updateFrame() {
    // Finds the chunks that need to be written
    std::stack<std::pair<int,int>> chunk_buffer;
    int relative_x, relative_z;
    if(camera.getPosition().x < 0) relative_x = ((camera.getPosition().x - 16)/16);
    else relative_x = (camera.getPosition().x/16);
    if(camera.getPosition().z < 0) relative_z = ((camera.getPosition().z - 16)/16);
    else relative_z = (camera.getPosition().z/16);
    for(int i = -render_distance - buffer; i <= render_distance + buffer; i++){
        for(int j = -render_distance - buffer; j <= render_distance + buffer; j++){
            if(!ChunkLoader::checkFile(ChunkLoader::findFile(relative_x + i, relative_z + j, true))) {
                chunk_buffer.emplace(relative_x + i, relative_z + j);
                std::cerr << "writing file: Chunk(" << relative_x + i  << ',' << relative_z + j << ").txt" << std::endl;
            }
        }
    }
    // Writes the chunks one frame at a time
    if (!chunk_buffer.empty()) {
        if (!ChunkLoader::checkFile(ChunkLoader::findFile(chunk_buffer.top().first, chunk_buffer.top().second, true)))
            ChunkLoader::updateChunk(chunk_buffer.top().first, chunk_buffer.top().second);
        chunk_buffer.pop();
    }

    float current_frame = static_cast<float>(glfwGetTime());
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    // rendering commands here
    glClearColor(0.2f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    myglGradientBackground(0.5, 0.8, 0.9, 1.0,
                            0.8, 0.8, 0.9, 1.0);





    // Activate and configure shader
    block_shader->use();

    // Creating transformations
    glm::mat4 model = glm::mat4(
            1.0f); // make sure to initialize matrix to identity matrix first. That is important for some reason.
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view = camera.getViewMatrix();


    // Set transformations
    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // retrieve the matrix uniform locations
    unsigned int view_loc = glGetUniformLocation(block_shader->getId(), "view");
    unsigned int projection_loc = glGetUniformLocation(block_shader->getId(), "projection");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    // note: Projection matrix rarely changes so it should be set outside of the loop.
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO[0]);
    unsigned int model_loc;
    block_shader->use();

    model = glm::mat4(1.0f);
    model_loc = glGetUniformLocation(block_shader->getId(), "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

    // Finds the chunks that need to be rendered
    for(int i = -render_distance; i <= render_distance; i++){
        for(int j = -render_distance; j <= render_distance; j++) {
            if(local_block_data.find(ChunkLoader::findFile(relative_x + i, relative_z + j, true)) == local_block_data.end())
                render.push(std::make_pair(relative_x + i, relative_z + j));
        }
    }

    std::unordered_set<Block> temp;
    std::string file = ChunkLoader::findFile(render.top().first, render.top().second, true);
    ChunkLoader::readFile(file, temp);
    if(local_block_data.find(file) == local_block_data.end() && !temp.empty()) {
        local_block_data.insert(std::make_pair(file, *&temp));
        render.pop();
    }
    temp.clear();

    // rendering of blocks
    for(auto itk : local_block_data){
        for(auto itj : itk.second) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, itj.getPosition());
            model_loc = glGetUniformLocation(block_shader->getId(), "model");
            block_shader->setInt("texture2", ((Block)itj).getBlockType());
            glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    // User input function call
    processInput(window, combine, x_offset, y_offset, local_block_data, last_call_time);
    // local_block_data.clear();

    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(((float)((float)SCR_HEIGHT / (float)SCR_WIDTH)), 1.0f, 1.0f));

    dot_shader->use();
    glBindVertexArray(vao_dot);
    int m_loc = glGetUniformLocation(dot_shader->getId(), "model");
    glUniformMatrix4fv(m_loc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_TRIANGLES, 0, 12);
    // Draw the inventory here.
    // inventoryShader.use();
    model = glm::translate(model, glm::vec3(-1.8f, -0.8f, 0.0f));
    inventory_shader->use();
    int selection = 0;
    for (int i = 0; i < inventory.size(); i++) {
        inventory_shader->setInt("texturein", (i));
        model = glm::translate(model, glm::vec3(0.55f, 0.0f, 0.0f));
        glBindVertexArray(inventory_vao);
        int projection_2d_loc = glGetUniformLocation(inventory_shader->getId(), "projection");
        int model_2d_loc = glGetUniformLocation(inventory_shader->getId(), "model");
        int view_2d_loc = glGetUniformLocation(inventory_shader->getId(), "view");
        glUniformMatrix4fv(projection_2d_loc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(model_2d_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_2d_loc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
        if (combine == i) {
            inventory_shader->setInt("combine", 1);
        }
        else {
            inventory_shader->setInt("combine", 0);
        }
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // draw inventory menu
    if (show_inventory_menu) {

    }


    // check and call events and swap the buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

}

void BetterBlox::frameBufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    (void)window;
}

void BetterBlox::errorCallback(int error, const char *msg) {
    std::string s;
    s = " [" + std::to_string(error) + "] " + msg + '\n';
    std::cerr << s << std::endl;
}
/**
 * @brief User Inputted Keystrokes for game functions
 * @param window Game Window
 * @param combine Block Identity
 * @param x_offset X - Player position
 * @param y_offset Y - Player position
 * @param chunk_rendering Local Cache
 * @param last_call_time Cooldown since last called
 */
void BetterBlox::processInput(GLFWwindow *window, int &combine, float &x_offset, float &y_offset, std::map<std::string, std::unordered_set<Block> >& chunk_rendering, std::chrono::system_clock::time_point& last_call_time) {
    // initializing variables for cooldown
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = now - last_call_time;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        show_inventory_menu ? show_inventory_menu = false : show_inventory_menu = true;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        combine = HAPPY_FACE;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        combine = CONTAINER;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        combine = DIAMOND_ORE;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        combine = BEDROCK;
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        combine = GRASS;
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
        combine = WATER;
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
        combine = 9;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        x_offset += 0.01;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        x_offset -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        y_offset += 0.01;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        y_offset -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.processKeyboard(UP, delta_time);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.processKeyboard(DOWN, delta_time);
    if (glfwGetMouseButton(window, 0 == GLFW_PRESS) || glfwGetMouseButton(window, 1 == GLFW_PRESS)) {
        if (elapsed_seconds < std::chrono::milliseconds (350)) {
            return;
        }
        // break or insert blocks into the save files and local data storage.
        for (float distance = 1; distance < 15; distance++) {
            glm::vec3 camera_position = camera.getPosition() + (camera.getFront() * (distance));
            camera_position.x = (float)std::round(camera_position.x);
            camera_position.y = (float)std::round(camera_position.y);
            camera_position.z = (float)std::round(camera_position.z);
            for (int i = 0; i < 10; i++) {   // Check for a valid block id
                glm::vec3 cursor = camera.getPosition() + (camera.getFront() * (distance - 1));
                cursor.x = (float)std::round(cursor.x);
                cursor.y = (float)std::round(cursor.y);
                cursor.z = (float)std::round(cursor.z);
                std::string chunk = ChunkLoader::findFile(camera_position.x, camera_position.z, false);
                if (chunk_rendering.find(chunk)->second.find(Block(camera_position,i)) != chunk_rendering.find(chunk)->second.end()) {
                    if (glfwGetMouseButton(window, 0 == GLFW_PRESS)) {
                        ChunkLoader::placeCube(cursor, combine);
                        chunk_rendering.find(chunk)->second.insert(Block(cursor,combine));
                    }

                    else if (glfwGetMouseButton(window, 1 == GLFW_PRESS)) {
                        ChunkLoader::deleteBlock(camera_position, i, ChunkLoader::findFile(camera_position.x, camera_position.z, false));
                        chunk_rendering.find(chunk)->second.erase(Block(camera_position, i));
                    }
                    last_call_time = now;
                    return;
                }
            }
        }
    }
}

/**
 * @brief Callback function for mouse position and movement.
 *
 * @param x_pos_in
 * @param y_pos_in
 */
void BetterBlox::mouseCallback(double x_pos_in, double y_pos_in) {
    float x_pos = static_cast<float>(x_pos_in);
    float y_pos = static_cast<float>(y_pos_in);

    if (first_mouse) {
        last_x = x_pos;
        last_y = y_pos;
        first_mouse = false;
    }

    float x_offset = x_pos - last_x;
    float y_offset = last_y - y_pos;

    last_x = x_pos;
    last_y = y_pos;

    camera.processMouseMovement(x_offset, y_offset);
}

/**
 * @brief Wrapper function for the mouseCallback() function.
 *
 * @detailed Since GLFW is a C API, it has no knowledge of classes, thus we cannot pass a non-static member function to GLFW when
 * setting the cursor callback function. This wrapper function wraps the non-static function and acts as a static function
 * so we can pass it into GLFW.
 * @param window
 * @param x_pos_in
 * @param y_pos_in
 */
void BetterBlox::mouseCallbackStatic(GLFWwindow *window, double x_pos_in, double y_pos_in) {
    BetterBlox* game = static_cast<BetterBlox*>(glfwGetWindowUserPointer(window));
    if (game != nullptr) {
        game->mouseCallback(x_pos_in, y_pos_in);
    }
}

/**
 * @brief Callback function for mouse scrolling.
 *
 * @param x_offset
 * @param y_offset
 */
void BetterBlox::scrollCallback(double x_offset, double y_offset) {
    camera.processMouseScroll(static_cast<float>(y_offset));
}

/**
 * @brief Wrapper function for the scrollCallback() function.
 *
 * @detailed Since GLFW is a C API, it has no knowledge of classes, thus we cannot pass a non-static member function to GLFW when
 * setting the scroll callback function. This wrapper function wraps the non-static function and acts as a static function
 * so we can pass it into GLFW.
 * @param window
 * @param x_pos_in
 * @param y_pos_in
 */
void BetterBlox::scrollCallbackStatic(GLFWwindow *window, double x_offset, double y_offset) {
    BetterBlox* game = static_cast<BetterBlox*>(glfwGetWindowUserPointer(window));
    if (game != nullptr) {
        game->scrollCallback(x_offset, y_offset);
    }
}

void BetterBlox::myglGradientBackground(float top_r, float top_g, float top_b, float top_a, float bot_r, float bot_g, float bot_b,
                             float bot_a) {
    glDisable(GL_DEPTH_TEST);

    static GLuint background_vao = 0;
    static GLuint background_shader = 0;

    if (background_vao == 0) {
        glGenVertexArrays(1, &background_vao);

        const char *vs_src = (const char *)SHADER_HEADER SHADER_STR
        (
                out vec2 v_uv;
                void main() {
                    uint idx = uint(gl_VertexID);
                    gl_Position = vec4(idx & 1U, idx >> 1U, 0.0, 0.5) * 4.0 - 1.0;
                    v_uv = vec2(gl_Position.xy * 0.5 + 0.5);
                }
        );

        const char *fs_src = (const char *)SHADER_HEADER SHADER_STR
        (
                uniform vec4 top_color;
                uniform vec4 bot_color;
                in vec2 v_uv;
                out vec4 frag_color;

                void main() {
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

void BetterBlox::loadTexture(unsigned int &texture, std::string path, unsigned int type, unsigned int rgb_type) {
    glGenTextures(1, &texture);
    int width, height, nr_channels;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nr_channels, 0);
    if (!data) {
        std::cout << "Failed to load data" << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, type);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, type);

    float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, rgb_type, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}
