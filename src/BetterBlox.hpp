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

// Header Files
#include "Block.hpp"
#include "Camera.hpp"
#include "ChunkLoader.hpp"
#include "Inventory.hpp"
#include "perlin.hpp"
#include "Shader.hpp"
#include "stb_image.h"

class BetterBlox {
private:
    // Constants
    const unsigned int SCR_WIDTH = 2200;
    const unsigned int SCR_HEIGHT = 1200;

    // Since VAO and VBO arrays need this before compile time, we have to use `static constexpr`
    // to initialize it at compile time.
    static constexpr unsigned int NUM_TRIANGLES = 1;

    Camera camera;
    ChunkLoader chunk_loader;
    std::unordered_set<Block> block_rendering;
    float last_x = SCR_WIDTH / 2.0f;
    float last_y = SCR_HEIGHT / 2.0f;
    bool first_mouse = true;
    std::unordered_set<Block> cube_positions;
    GLFWwindow *window; // Check BetterBlox::initialize() for initialization
    int combine;
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

    // Shaders
    // These need to be pointers as they do not have a default constructor.
    Shader *shader;
    Shader *dot_shader;
    Shader *block_shader;
    Shader *inventory_shader;

    // Settings
    float water_level = 5;
    int render_distance = 3;
    int buffer = 1;

    // Function Prototypes
    void initialize();
    void updateFrame();

    // These functions need to be static to be able to pass them to GLFW.
    static void frameBufferSizeCallback(GLFWwindow *window, int width, int height);
    static void errorCallback(int error, const char *msg);

    void processInput(GLFWwindow *window, int &combine, float &x_offset, float &y_offset, std::unordered_set<Block>& block_rendering);

    // Static wrapper functions are needed to pass these member functions to GLFW since they access other members.
    void mouseCallback(double x_pos_in, double y_pos_in);
    static void mouseCallbackStatic(GLFWwindow *window, double x_pos_in, double y_pos_in);
    void scrollCallback(double x_offset, double y_offset);
    static void scrollCallbackStatic(GLFWwindow *window, double x_offset, double y_offset);

    void myglGradientBackground(float top_r, float top_g, float top_b, float top_a, float bot_r, float bot_g, float bot_b,
                                 float bot_a);
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
    // throw std::runtime_error(std::string("Test error.")); // TODO: Exceptions within class do not work for some reason.

    initialize();
    while(!glfwWindowShouldClose(window)) {
        updateFrame();
    }
    glfwTerminate(); // We could probably have a terminate function.
}

void BetterBlox::initialize() {
    camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    inventory = Inventory(10);

    // Opengl treats the 0,0 locations on images to be the bottom. This flips the images so the 0, 0 will be at the top.
    stbi_set_flip_vertically_on_load(true);

    // Setting up the window stuff
    glfwSetErrorCallback(errorCallback);

    if (GL_TRUE != glfwInit()) {
        throw std::runtime_error(std::string("Failed to initialize GLFW."));
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif


    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "BetterBlox", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        throw std::runtime_error(std::string("Failed to create GLFW window."));
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallbackStatic);
    glfwSetScrollCallback(window, scrollCallbackStatic);

    glfwSetWindowUserPointer(window, this);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error(std::string("Failed to initialize GLAD."));
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
    std::stack<std::pair<int,int>> chunk_buffer;
    int relative_x, relative_z;
    if(camera.getPosition().x < 0) relative_x = ((camera.getPosition().x - 16)/16);
    else relative_x = (camera.getPosition().x/16);
    if(camera.getPosition().z < 0) relative_z = ((camera.getPosition().z - 16)/16);
    else relative_z = (camera.getPosition().z/16);
    for(int i = -render_distance - buffer; i <= render_distance + buffer; i++){
        for(int j = -render_distance - buffer; j <= render_distance + buffer; j++){
            if(!chunk_loader.checkFile(chunk_loader.findFile(relative_x + i, relative_z + j, true))) {
                chunk_buffer.emplace(relative_x + i, relative_z + j);
                std::cerr << "writing file: Chunk(" << relative_x + i  << ',' << relative_z + j << ").txt" << std::endl;
            }
        }
    }

    if (!chunk_buffer.empty()) {
        if (chunk_loader.checkFile(chunk_loader.findFile(chunk_buffer.top().first, chunk_buffer.top().second, true)));
        chunk_loader.updateChunk(chunk_buffer.top().first, chunk_buffer.top().second);
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

    //rendering
    std::unordered_set<Block> temp;
    for(int i = -render_distance; i <= render_distance; i++){
        for(int j = -render_distance; j <= render_distance; j++) {
            temp = chunk_loader.readFile(chunk_loader.findFile(relative_x + i, relative_z + j, true));
            block_rendering.merge(temp);
            temp.clear();
        }
    }

    std::unordered_set<Block>::iterator itr;
    std::unordered_set<Block> l = block_rendering;
//        for(auto l : blockRendering) {
    for (itr = l.begin(); itr != l.end(); itr++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, itr->getPosition());
        model_loc = glGetUniformLocation(block_shader->getId(), "model");
        block_shader->setInt("texture2", ((Block) *itr).getBlockType());
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
//        }
    processInput(window, combine, x_offset, y_offset, block_rendering);
    block_rendering.clear();

    dot_shader->use();
    glBindVertexArray(vao_dot);
    glDrawArrays(GL_TRIANGLES, 0, 12);
    // Draw the inventory here.
    // inventoryShader.use();
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(((float)((float)SCR_HEIGHT / (float)SCR_WIDTH)), 1.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.8f, -0.8f, 0.0f));
    inventory_shader->use();
    for (int i = 0; i < 4; i++) {
        inventory_shader->setInt("texturein", (i));
        model = glm::translate(model, glm::vec3(0.6f, 0.0f, 0.0f));
        glBindVertexArray(inventory_vao);
        unsigned int projection_2d_loc = glGetUniformLocation(inventory_shader->getId(), "projection");
        unsigned int model2dLoc = glGetUniformLocation(inventory_shader->getId(), "model");
        unsigned int view_2d_loc = glGetUniformLocation(inventory_shader->getId(), "view");
        glUniformMatrix4fv(projection_2d_loc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_2d_loc, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));

        glDrawArrays(GL_TRIANGLES, 0, 6);
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

void BetterBlox::processInput(GLFWwindow *window, int &combine, float &x_offset, float &y_offset, std::unordered_set<Block>& block_rendering) {
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
        glm::vec3 position;
        bool placed = false;
        for (float distance = 1; distance < 8; distance++) {
            glm::vec3 camera_position = camera.getPosition() + (camera.getFront() * distance);
            position.x = (float)std::round(camera_position.x);
            position.y = (float)std::round(camera_position.y);
            position.z = (float)std::round(camera_position.z);
            for (int i = 0; i < 10; i++) {   // Check for a valid block id
                if (block_rendering.find(Block(position, i)) != block_rendering.end() && !placed) {
                    glm::vec3 cursor = camera.getPosition() + (camera.getFront() * (distance - 1));
                    if (glfwGetMouseButton(window, 0 == GLFW_PRESS))
                        chunk_loader.placeCube(cursor, combine);
                    if (glfwGetMouseButton(window, 1 == GLFW_PRESS)) {
                        chunk_loader.deleteBlock(position, i, chunk_loader.findFile(position.x, position.z, false));
                        block_rendering.erase(Block(position));
                    }

                    placed = true;
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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, rgb_type, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}
