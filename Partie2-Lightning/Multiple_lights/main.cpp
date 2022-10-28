#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "src/shader.h"
#include "src/camera.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char* filename);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera;

bool firstMouse = true;
float lastX =  SCR_WIDTH / 2.0f;
float lastY =  SCR_HEIGHT / 2.0;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// Lights parameters 
const int NR_POINT_LIGHTS = 4;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Configure all OpenGL states
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader cubeShader("shaders/shader.vs", "shaders/shader.fs");
    Shader lightCubeShader("shaders/light_cube_shader.vs", "shaders/light_cube_shader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    glm::vec3 cube_position[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    unsigned int diffuseMap = loadTexture("ressources/texture/container2.png");
    unsigned int specularMap = loadTexture("ressources/texture/container2_specular.png");

    cubeShader.use();
    cubeShader.setInt("material.diffuse",0);
    cubeShader.setInt("material.specular",1);
    
    int index = 0;

    glm::vec3 dirLightDir(-0.2f, -1.0f, -0.3f);
    glm::vec3 dirLghtAmbient(0.05f, 0.05f, 0.05f);
    glm::vec3 dirLghtDiffuse(0.4f, 0.4f, 0.4f);
    glm::vec3 dirLghtSpecular(0.5f, 0.5f, 0.5f);

    glm::vec3 lightPointPos[NR_POINT_LIGHTS] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    glm::vec3 lightPointColor[NR_POINT_LIGHTS] = {
        glm::vec3( 1.0f,  0.2f,  0.2f),
        glm::vec3( 0.2f, 1.0f, 0.2f),
        glm::vec3(1.0f,  0.2f, 0.2f),
        glm::vec3(1.0f)
    };

    glm::vec3 lightSpotPos(1.0f,1.0f,1.0f);

    glm::vec3 lightAmbient(0.1f, 0.1f, 0.1f);
    glm::vec3 lightDiffuse(1.0f);
    

    glm::vec3 spotLightAmbient(0.1f, 0.1f, 0.1f);
    glm::vec3 spotLightDiffuse(1.0f, 1.0f, 1.0f);
    glm::vec3 spotLightSpecular(1.0f);


    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cubeShader.use();

        // common var
        cubeShader.setVec3("cameraPos",glm::value_ptr(camera.Position));

        // material properties
        cubeShader.setFloat("material.shininess", 128.0f);

        // dirLight parameters
        cubeShader.setVec3("dirLight.direction", glm::value_ptr(dirLightDir));
        cubeShader.setVec3("dirLight.ambient", glm::value_ptr(dirLghtAmbient));
        cubeShader.setVec3("dirLight.diffuse", glm::value_ptr(dirLghtDiffuse));
        cubeShader.setVec3("dirLight.specular", glm::value_ptr(dirLghtSpecular));

        // Points lights
        cubeShader.setVec3("pointLights[0].position", glm::value_ptr(lightPointPos[0]));
        cubeShader.setVec3("pointLights[0].ambient", glm::value_ptr(lightAmbient));
        cubeShader.setVec3("pointLights[0].diffuse", glm::value_ptr(lightDiffuse));
        cubeShader.setVec3("pointLights[0].specular", glm::value_ptr(lightPointColor[0]));
        cubeShader.setFloat("pointLights[0].quadratic", 1.0f);
        cubeShader.setFloat("pointLights[0].linear", 0.045f);
        cubeShader.setFloat("pointLights[0].constant", 0.0075f);

        cubeShader.setVec3("pointLights[1].position", glm::value_ptr(lightPointPos[1]));
        cubeShader.setVec3("pointLights[1].ambient", glm::value_ptr(lightAmbient));
        cubeShader.setVec3("pointLights[1].diffuse", glm::value_ptr(lightDiffuse));
        cubeShader.setVec3("pointLights[1].specular", glm::value_ptr(lightPointColor[1]));
        cubeShader.setFloat("pointLights[1].quadratic", 1.0f);
        cubeShader.setFloat("pointLights[1].linear", 0.045f);
        cubeShader.setFloat("pointLights[1].constant", 0.0075f);

        cubeShader.setVec3("pointLights[2].position", glm::value_ptr(lightPointPos[2]));
        cubeShader.setVec3("pointLights[2].ambient", glm::value_ptr(lightAmbient));
        cubeShader.setVec3("pointLights[2].diffuse", glm::value_ptr(lightDiffuse));
        cubeShader.setVec3("pointLights[2].specular", glm::value_ptr(lightPointColor[2]));
        cubeShader.setFloat("pointLights[2].quadratic", 1.0f);
        cubeShader.setFloat("pointLights[2].linear", 0.045f);
        cubeShader.setFloat("pointLights[2].constant", 0.0075f);

        cubeShader.setVec3("pointLights[3].position", glm::value_ptr(lightPointPos[3]));
        cubeShader.setVec3("pointLights[3].ambient", glm::value_ptr(lightAmbient));
        cubeShader.setVec3("pointLights[3].diffuse", glm::value_ptr(lightDiffuse));
        cubeShader.setVec3("pointLights[3].specular", glm::value_ptr(lightPointColor[3]));
        cubeShader.setFloat("pointLights[3].quadratic", 1.0f);
        cubeShader.setFloat("pointLights[3].linear", 0.045f);
        cubeShader.setFloat("pointLights[3].constant", 0.0075f);

        // Spot lights
        cubeShader.setVec3("spotLight.position", glm::value_ptr(camera.Position));
        cubeShader.setVec3("spotLight.direction", glm::value_ptr(camera.Front));
        cubeShader.setVec3("spotLight.ambient", glm::value_ptr(spotLightAmbient));
        cubeShader.setVec3("spotLight.diffuse", glm::value_ptr(spotLightDiffuse));
        cubeShader.setVec3("spotLight.specular", glm::value_ptr(spotLightSpecular));
        cubeShader.setFloat("spotLight.constant", 1.0f);
        cubeShader.setFloat("spotLight.linear", 0.045f);
        cubeShader.setFloat("spotLight.quadratic", 0.0075f);
        cubeShader.setFloat("spotLight.cutoff", glm::cos(glm::radians(12.5f)));
        cubeShader.setFloat("spotLight.cutoff_big", glm::cos(glm::radians(15.0f)));  

        // light properties
        if (index < 100){
            cubeShader.setFloat("spotLight.flash",0.0f);
        } else {
            cubeShader.setFloat("spotLight.flash",1.0f);
        }

        // view projection transformation
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix((float(SCR_WIDTH)/float(SCR_HEIGHT)));
        cubeShader.setMat4f("view",glm::value_ptr(view));
        cubeShader.setMat4f("projection",glm::value_ptr(projection));

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);

        // bind textures
        // ------------
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // render all cubes
        glBindVertexArray(cubeVAO);

        for (int i = 0; i <9;i++){
            model = glm::mat4(1.0f);
            model = glm::translate(model,cube_position[i]);
            model = glm::rotate(model,(float)glfwGetTime(),glm::vec3(0.5f,0.5f,0.5f));
            cubeShader.setMat4f("model",glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        // also draw lamps object
        // ----------------- 
        lightCubeShader.use();
        lightCubeShader.setMat4f("projection", glm::value_ptr(projection));
        lightCubeShader.setMat4f("view", glm::value_ptr(view));


        model = glm::mat4(1.0f);
        glBindVertexArray(lightCubeVAO);

        // Draw points lights
        for (int i = 0; i < NR_POINT_LIGHTS ; i++){
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPointPos[i]);
            model = glm::scale(model,glm::vec3(0.1f,0.1f,0.3f));
            lightCubeShader.setMat4f("model", glm::value_ptr(model));
            lightCubeShader.setVec3("color", glm::value_ptr(lightPointColor[i]));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // // Draw Spot light
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, lightSpotPos);
        // model = glm::scale(model,glm::vec3(0.2f,0.2f,0.2f));
        // lightCubeShader.setMat4f("model", glm::value_ptr(model));
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        index++;
        if (index > 200){
            index = 0;
        }
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // clavier mouvement
    float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.Position += cameraSpeed * camera.Front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.Position -= cameraSpeed * camera.Front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.Position -= glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.Position += glm::normalize(glm::cross(camera.Front, camera.Up)) * cameraSpeed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
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

    camera.ProcessMouseMovement(xoffset,yoffset,true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const * filename)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << filename << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}