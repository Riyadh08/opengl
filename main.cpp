//
//  main.cpp
//  3D Object Drawing
//
//  Created by Nazirul Hasan on 4/9/23.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "basic_camera.h"
#include "pointLight.h"
#include "cube.h"
#include "hexagon.h"
#include "pyramid.h"
#include "stb_image.h"

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);
glm::mat4 myRotate(glm::mat4 R, float theta, glm::vec3 k);
glm::mat4 myPerspective(float fov, float aspect, float near, float far);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

// modelling transform
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 2.0;
float scale_Y = 2.0;
float scale_Z = 2.0;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 0.5, eyeZ = 4.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);

// lights
// positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(1.50f,  1.50f,  0.0f),
    glm::vec3(1.5f,  -1.5f,  0.0f),
    glm::vec3(-1.5f,  1.5f,  0.0f),
    glm::vec3(-1.5f,  -1.5f,  0.0f)
};
PointLight pointlight1(

    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.48f, 0.48f, 0.48f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    1       // light number
);
PointLight pointlight2(

    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.48f, 0.48f, 0.48f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    2       // light number
);
PointLight pointlight3(

    pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.48f, 0.48f, 0.48f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    3       // light number
);
PointLight pointlight4(

    pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.48f, 0.48f, 0.48f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    4       // light number
);


// light settings
bool dirLightOn = true;
bool pointLightOn = true;
bool spotLightOn = true;
bool ambientToggle = true;
bool diffuseToggle = true;
bool specularToggle = true;


// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 4208: Computer Graphics Laboratory", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------

    Shader lightingShaderWithTexture("vertexShaderForPhongShadingWithTexture.vs", "fragmentShaderForPhongShadingWithTexture.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    //string laughEmoPath = "emoji.png";
    string laughEmoPath = "hexa.png";

    //  unsigned int              loadTexture(char const* path,  GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);
    unsigned int laughEmoji = loadTexture(laughEmoPath.c_str(), GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    Pyramid pyra = Pyramid(laughEmoji);
    Hexagon hex = Hexagon(laughEmoji);
    Cube cube = Cube(laughEmoji);

    //Sphere sphere = Sphere();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShaderWithTexture.use();
        lightingShaderWithTexture.setVec3("viewPos", basic_camera.eye);

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = myPerspective(glm::radians(basic_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);
        lightingShaderWithTexture.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = basic_camera.createViewMatrix();
        //glm::mat4 view = basic_camera.createViewMatrix();
        lightingShaderWithTexture.setMat4("view", view);

        // Modelling Transformation
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, model;
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = myRotate(translateMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = myRotate(rotateXMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = myRotate(rotateYMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(rotateZMatrix, glm::vec3(scale_X, scale_Y, scale_Z));

        lightingShaderWithTexture.use();
        // point light 1
        pointlight1.setUpPointLight(lightingShaderWithTexture);
        // point light 2
        pointlight2.setUpPointLight(lightingShaderWithTexture);
        // point light 3
        pointlight3.setUpPointLight(lightingShaderWithTexture);
        // point light 4
        pointlight4.setUpPointLight(lightingShaderWithTexture);

        glm::mat4 modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-0.0f, -0.4f, -2.8f));

        // pyra.draw(lightingShaderWithTexture, modelMatrixForContainer * model);
        hex.draw(lightingShaderWithTexture, modelMatrixForContainer * model);
        // cube.draw(lightingShaderWithTexture, modelMatrixForContainer * model);

        // also draw the lamp object(s)
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
        for (unsigned int i = 0; i < 4; i++)
        {
            Cube lightCube = Cube(glm::vec3(0.8f, 0.8f, 0.8f));
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightCube.drawColor(ourShader, model);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        eyeZ -= 2.5 * deltaTime;
        lookAtZ -= 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        eyeZ += 2.5 * deltaTime;
        lookAtZ += 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        eyeX -= 2.5 * deltaTime;
        lookAtX -= 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        eyeX += 2.5 * deltaTime;
        lookAtX += 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        if (rotateAxis_X) rotateAngle_X -= 0.1;
        else if (rotateAxis_Y) rotateAngle_Y -= 0.1;
        else rotateAngle_Z -= 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) translate_Y += 0.01;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) translate_Y -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) translate_X += 0.01;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) translate_X -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) translate_Z += 0.01;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) translate_Z -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) scale_X += 0.01;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) scale_X -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) scale_Y += 0.01;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) scale_Y -= 0.01;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) scale_Z += 0.01;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) scale_Z -= 0.01;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        rotateAngle_X += 0.1;
        rotateAxis_X = 1.0;
        rotateAxis_Y = 0.0;
        rotateAxis_Z = 0.0;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
        rotateAngle_Y += 1.0;
        rotateAxis_X = 0.0;
        rotateAxis_Y = 1.0;
        rotateAxis_Z = 0.0;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        rotateAngle_Z += 0.1;
        rotateAxis_X = 0.0;
        rotateAxis_Y = 0.0;
        rotateAxis_Z = 1.0;
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    {
        eyeX += 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        eyeX -= 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        eyeZ += 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        eyeZ -= 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        eyeY += 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        eyeY -= 2.5 * deltaTime;
        basic_camera.eye = glm::vec3(eyeX, eyeY, eyeZ);
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        lookAtX += 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        lookAtX -= 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        lookAtY += 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        lookAtY -= 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        lookAtZ += 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        lookAtZ -= 2.5 * deltaTime;
        basic_camera.lookAt = glm::vec3(lookAtX, lookAtY, lookAtZ);
    }
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        basic_camera.V = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {
        basic_camera.V = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
    {
        basic_camera.V = glm::vec3(0.0f, 0.0f, 1.0f);
    }

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        if (pointLightOn)
        {
            pointlight2.turnOff();
            pointlight3.turnOff();
            pointlight4.turnOff();
            pointLightOn = !pointLightOn;
        }
        else
        {
            pointlight2.turnOn();
            pointlight3.turnOn();
            pointlight4.turnOn();
            pointLightOn = !pointLightOn;
        }
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    basic_camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax)
{
    unsigned int textureID;// Create Texture 
    glGenTextures(1, &textureID);// Generate texture Object in GPU memory and give it an ID

    int width, height, nrComponents;

    // stb_image loads images with top-left origin by default
    // OpenGL expects textures to have bottom-left origin
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);// Bind the texture object 
        glTexImage2D(
            GL_TEXTURE_2D,      // target: we are creating a 2D texture
            0,                  // level: mipmap level (0 = base level)
            format,             // internal format: how GPU should store the texture (RGB, RGBA, etc.)
            width,              // width of the texture in pixels
            height,             // height of the texture in pixels
            0,                  // border: must be 0 (legacy stuff)
            format,             // format: format of the pixel data you are uploading
            GL_UNSIGNED_BYTE,   // type: data type of each channel (e.g., unsigned byte)
            data                // pointer to the actual image data array
        );


        glGenerateMipmap(GL_TEXTURE_2D);// Generate Mipmaps for the currently bound texture object

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrappingModeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrappingModeT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilteringModeMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilteringModeMax);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

glm::mat4 myRotate(glm::mat4 R, float theta, glm::vec3 k)
{
    float k_x = k.x;
    float k_y = k.y;
    float k_z = k.z;
    float sin_theta = sin(theta);
    float cos_theta = cos(theta);
    
    glm::mat4 I(1.0f);
    glm::mat4 K(0.0f);
    
    // Construct skew-symmetric matrix K
    // Since glm uses column-major order, K[col][row]
    // Row 0: 0, -k_z, k_y
    K[0][0] = 0.0f;  K[1][0] = -k_z;  K[2][0] = k_y;   K[3][0] = 0.0f;
    // Row 1: k_z, 0, -k_x
    K[0][1] = k_z;   K[1][1] = 0.0f;  K[2][1] = -k_x;  K[3][1] = 0.0f;
    // Row 2: -k_y, k_x, 0
    K[0][2] = -k_y;  K[1][2] = k_x;   K[2][2] = 0.0f;  K[3][2] = 0.0f;
    // Row 3
    K[0][3] = 0.0f;  K[1][3] = 0.0f;  K[2][3] = 0.0f;  K[3][3] = 0.0f;

    glm::mat4 K2 = K * K;
    
    // Rodrigues' Formula: R = I + (sin(theta))K + (1 - cos(theta))K^2
    glm::mat4 rot = I + (sin_theta * K) + ((1.0f - cos_theta) * K2);
    
    // Apply rotation matrix
    return R * rot;
}

glm::mat4 myPerspective(float fov, float aspect, float near, float far)
{
    glm::mat4 P(0.0f);
    float tanHalfFov = tan(fov / 2.0f);
    
    P[0][0] = 1.0f / (aspect * tanHalfFov);
    P[1][1] = 1.0f / tanHalfFov;
    P[2][2] = -(far + near) / (far - near);
    P[2][3] = -1.0f;
    P[3][2] = -(2.0f * far * near) / (far - near);
    
    return P;
}
