#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include "shader_s.h"
#include <Model.h> //Import error: Delete stb_image.h of current file, then include this lib before defining STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
//#include "stb_image.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include <vector>
#include "MagicCube.h"
#include <cstdlib>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <shader.h>
#include <cube.h>
#include <arcball.h>
#include <cmath>


void init_callback();
void init_texture();
void processInput(GLFWwindow* window);
GLFWwindow* glAllInit();
unsigned int loadTexture(const char*);
void nextVelPos(double velocity, double position, double groundHeight, double dt);
Camera camera(glm::vec3(0, 0, 20), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), 0, 0);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int key, int action, int mod);
unsigned int loadTexture(const char*);


float initialHeight = 0.0f, groundHeight = -3.0f;
glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, initialHeight, 0));
glm::mat4 model_drop = model;
instruction my_ins(1, 0, 8);
int replay_ins_all[8] = { 1,0,3,2,5,4,7,6 };
bool firstMouse = true, mouse_right_down = false, mouse_left_down = false, replay = false, disturb = false, dropping = false;
double lastX, lastY, actSX, actSY;
int whichx, whichy, count_disturb = 0;
std::vector<instruction*> instructions;
Shader* myShader, * tableShader;
GLFWwindow* mainWindow = NULL;
unsigned int SCR_WIDTH = 1920, SCR_HEIGHT = 1080;
Model* tableModel;
bool prevDKeyState = false;
double initialVel = 3.0, initialPos = initialHeight;
double immediateVel, immediatePos;

// Physics variables
float GRAVITY_FACTOR = 2.0f;
float startTime = 0.0f;
float dropTime = 0.0f;
const float timeDiff = 0.01f;

// Magic cube
MagicCube magicCube;

// for texture
static unsigned int cubeTexture1; // Array of texture ids.
static unsigned int cubeTexture2;
static unsigned int cubeTexture3;
static unsigned int cubeTexture4;
static unsigned int cubeTexture5;
int textureNumber = 1;


int main()
{
   mainWindow = glAllInit();

   //====data init====//
   //==============set callback====================//
   init_callback();
   glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
   glfwSetCursorPosCallback(mainWindow, mouse_callback);
   glfwSetMouseButtonCallback(mainWindow, mouse_button_callback);
   //==============set callback====================//
   //====bind texture====//
   cubeTexture1 = loadTexture("img_out-1.jpg");
   cubeTexture2 = loadTexture("img_out-2.jpg");
   cubeTexture3 = loadTexture("img_out-3.jpg");
   cubeTexture4 = loadTexture("img_out-4.jpg");
   cubeTexture5 = loadTexture("img_out-5.jpg");

   //init_texture();
   //unsigned int texture;
   //glGenTextures(1, &texture);
   //glActiveTexture(GL_TEXTURE0);
   //glBindTexture(GL_TEXTURE_2D, texture);
   //int width, height, nrChannels;
   //unsigned char* data = stbi_load("img_out-1.jpg", &width, &height, &nrChannels, 0);
   //if (data)
   //{
   //   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
   //   glGenerateMipmap(GL_TEXTURE_2D);
   //}
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   //stbi_image_free(data);
   //====bind texture====//
   //==============bind VAO========================//
   GLuint VAO, VBO;
   glGenVertexArrays(1, &VAO);
   glBindVertexArray(VAO);
   glGenBuffers(1, &VBO);
   glBindBuffer(GL_ARRAY_BUFFER, VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(magicCube.vertices), magicCube.vertices, GL_DYNAMIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
   glEnableVertexAttribArray(2);
   glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 5));
   glEnableVertexAttribArray(3);
   //==============bind VAO========================//
   glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
   glEnable(GL_DEPTH_TEST);

   tableModel = new Model("table/table.obj");
   myShader = new Shader("shader.vs", "shader.fs");
   tableShader = new Shader("modelLoading.vs", "modelLoading.fs");
   //my_shader.use();

   glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
   glm::mat4 ortho = glm::ortho(-14.5f, 1.5f, -7.5f, 1.5f, 0.1f, 100.0f);
   glm::mat4 view = glm::lookAt(camera.camPos, camera.camPos + camera.camDir, camera.camUp);
   srand(glfwGetTime());
   float deltaTime = 0;
   int count = 0;
   float coe = 1;

   myShader->use();
   myShader->setInt("texture1", 0);
   myShader->setMat4("model", model);
   myShader->setMat4("view", view);
   myShader->setMat4("projection", projection);
   myShader->setFloat("coe", coe);

   // TABLE DRAW
   glm::mat4 tablePos = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));
   tablePos = glm::translate(tablePos, glm::vec3(0, -75, 0));

   //tableModel->Draw(*tableShader);

   while (!glfwWindowShouldClose(mainWindow))
   {
      processInput(mainWindow);
      float currentFrame = glfwGetTime();

      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      tableShader->use();
      tableShader->setMat4("model", tablePos);
      tableShader->setMat4("view", view);
      tableShader->setMat4("projection", projection);
      tableModel->Draw(*tableShader);

      myShader->use();
      glActiveTexture(GL_TEXTURE0);
      glBindVertexArray(VAO);

      if (!dropping) {
         if (!replay && !disturb)
            magicCube.rotate(my_ins);
         else if (disturb)
         {
            if (!magicCube.cubeMoving)
            {
               if (count_disturb > 40)
               {
                  disturb = false;
                  count_disturb = 0;
               }
               else
               {
                  ++count_disturb;
                  int tempx = rand() % 3;
                  int tempy = rand() % 3;
                  int dir = rand() % 5;
                  if (dir == 4)
                     dir = rand() % 4 + 4;
                  instructions.push_back(new instruction(tempx, tempy, dir));
                  instruction tempIns(tempx, tempy, dir);
                  magicCube.rotate(tempIns);
               }
            }
            else
            {
               instruction tempIns(0, 0, 8);
               magicCube.rotate(tempIns);
            }
         }
         else // Replay 동작
         {
            if (!instructions.empty())
            {
               if (!magicCube.cubeMoving)
               {
                  instruction replay_ins(*instructions.back());
                  replay_ins.dir = replay_ins_all[replay_ins.dir];
                  magicCube.rotate(replay_ins);
                  instructions.pop_back();
               }
               else
               {
                  instruction tempIns(0, 0, 8);
                  magicCube.rotate(tempIns);
               }
            }
            else
               replay = false;
         }

         switch (textureNumber) {
         case 1:
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cubeTexture1);
            myShader->setInt("texture1", 0); // 1
            break;
         case 2:
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, cubeTexture2);
            myShader->setInt("texture1", 1); // 2
            break;
         case 3:
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, cubeTexture3);
            myShader->setInt("texture1", 2); // 3
            break;
         case 4:
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, cubeTexture4);
            myShader->setInt("texture1", 3); // 4
            break;
         case 5:
             glActiveTexture(GL_TEXTURE4);
             glBindTexture(GL_TEXTURE_2D, cubeTexture5);
             myShader->setInt("texture1", 4); // 5
             break;
         default:
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cubeTexture1);
            myShader->setInt("texture1", 0); // 1
         }

         glBindBuffer(GL_ARRAY_BUFFER, VAO);
         glBufferData(GL_ARRAY_BUFFER, sizeof(magicCube.vertices), magicCube.vertices, GL_DYNAMIC_DRAW);
         for (int x = 0; x < 3; x++)
            for (int y = 0; y < 3; y++)
               for (int z = 0; z < 3; z++)
               {
                  int index = (x * 9 + y * 3 + z);
                  myShader->setMat4("model", model);
                  myShader->setFloat("coe", 1.0f);
                  myShader->setMat4("projection", projection);
                  glDrawArrays(GL_TRIANGLES, index * 36, 36);
                  if (magicCube.units[index].x == whichx && magicCube.units[index].y == whichy && magicCube.units[index].z == 0)
                     myShader->setFloat("coe", 0.1f);
                  myShader->setMat4("projection", ortho);
                  glDrawArrays(GL_TRIANGLES, index * 36, 36);
               }
      }
      else { // Simulate dropping with physics
         // ========================== TEST ========================== //
         model_drop = glm::mat4(1.0f);
         if ((glfwGetTime() - startTime) > timeDiff) {
            nextVelPos(immediateVel, immediatePos, groundHeight, timeDiff);

            startTime = glfwGetTime();
         }
         model_drop = glm::translate(glm::mat4(1.0f), glm::vec3(0, immediatePos, 0));
         float timePassedSinceDrop = glfwGetTime() - dropTime;
         //model_drop = glm::translate(model_drop, glm::vec3(timePassedSinceDrop * -0.6, 0, timePassedSinceDrop * -0.4));
         //glm::vec3 center_drop = glm::vec3(model_drop * glm::vec4(0, 0, 0, 1));
         //glm::mat4 view_drop = glm::lookAt(camera.camPos, center_drop, camera.camUp);
         // ========================================================== //

         glBindBuffer(GL_ARRAY_BUFFER, VBO);
         glBufferData(GL_ARRAY_BUFFER, sizeof(magicCube.vertices), magicCube.vertices, GL_DYNAMIC_DRAW);
         for (int x = 0; x < 3; x++)
            for (int y = 0; y < 3; y++)
               for (int z = 0; z < 3; z++)
               {
                  int index = (x * 9 + y * 3 + z);
                  myShader->setMat4("model", model_drop);
                  myShader->setFloat("coe", 1.0f);
                  myShader->setMat4("projection", projection);
                  glDrawArrays(GL_TRIANGLES, index * 36, 36);
               }
      }

      glBindVertexArray(0);

      glfwSwapBuffers(mainWindow);
      glfwPollEvents();
   }
   glfwTerminate();
   return 0;
}
void init_callback()
{
}
void init_texture()
{
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
   int text = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
   if (firstMouse)
   {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
   }
   lastX = xpos;
   lastY = ypos;

   if (mouse_right_down)
   {
      float xoffset = xpos - actSX;
      float yoffset = actSY - ypos;
      float sensitivity = 0.2;
      xoffset *= sensitivity;
      yoffset *= sensitivity;
      if (abs(xoffset) > 70)xoffset = 70 * xoffset / abs(xoffset);
      if (abs(yoffset) > 70)yoffset = 70 * yoffset / abs(yoffset);
      model = glm::rotate(glm::mat4(1.0f), glm::radians(xoffset), glm::vec3(0.0f, 1.0f, 0.0f));
      model = glm::rotate(model, glm::radians(yoffset), glm::vec3(-1.0f, 0.0f, 0.0f));

   }
}
void mouse_button_callback(GLFWwindow* window, int key, int action, int mod)
{
   if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
   {
      actSX = lastX;
      actSY = lastY;
      mouse_left_down = true;
   }
   if (key == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
   {
      actSX = lastX;
      actSY = lastY;
      mouse_right_down = true;
   }
   if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
   {
      float xoffset = lastX - actSX;
      float yoffset = lastY - actSY;
      if (abs(xoffset) + abs(yoffset) > 20)
      {
         if (abs(xoffset) > abs(yoffset))
         {
            if (xoffset > 0)
               my_ins.dir = 7;
            else
               my_ins.dir = 6;
         }
         else
         {
            if (yoffset > 0)
               my_ins.dir = 5;
            else
               my_ins.dir = 4;
         }
         int tempx = ((int)actSX - 1561) / 120;
         int tempy = 2 - ((int)actSY - 1) / 120;
         my_ins.x = tempx;
         my_ins.y = tempy;
         if (tempx >= 0 && tempx <= 2 && tempy >= 0 && tempy <= 2)
            my_ins.dir = my_ins.dir - 4;
      }
      mouse_left_down = false;
      instructions.push_back(new instruction(my_ins));
   }
   if (key == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
   {
      model = glm::mat4(1.0f);
      mouse_right_down = false;
   }

}
void processInput(GLFWwindow* window)
{
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
   if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
      replay = true;
   if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
      disturb = true;
   if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
      textureNumber = 1;
      std::cout << "textureNumber: " << textureNumber << std::endl;
   }
   if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
      textureNumber = 2;
      std::cout << "textureNumber: " << textureNumber << std::endl;
   }
   if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
      textureNumber = 3;
      std::cout << "textureNumber: " << textureNumber << std::endl;
   }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
       textureNumber = 4;
       std::cout << "textureNumber: " << textureNumber << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
       textureNumber = 5;
       std::cout << "textureNumber: " << textureNumber << std::endl;
    }

   // 현재 D 키 상태를 가져옴
   bool currentDKeyState = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

   // 현재 D 키가 눌려있고 이전 D 키가 눌려있지 않았을 때 (즉, 처음 눌렸을 때)
   if (currentDKeyState && !prevDKeyState)
   {
      // dropping 변수의 상태를 토글
      dropping = !dropping;
      if (dropping) {
         model_drop = model;
         startTime = glfwGetTime();
         dropTime = glfwGetTime();
         immediateVel = initialVel;
         immediatePos = initialPos;
      }
   }

   // 이전 D 키 상태를 현재 상태로 업데이트
   prevDKeyState = currentDKeyState;


   if (!mouse_left_down)
   {
      whichx = ((int)lastX - 1561) / 120;
      whichy = 2 - ((int)lastY - 1) / 120;
   }
   else
   {
      whichx = ((int)actSX - 1561) / 120;
      whichy = 2 - ((int)actSY - 1) / 120;
   }
   //std::cout << my_ins.x << "-" << my_ins.y << std::endl;
}

GLFWwindow* glAllInit()
{
   // Initialize GLFW
   glfwInit();

   // Set all the required options for GLFW
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

   // Create a GLFWwindow object that we can use for GLFW's functions
   GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Rubik's Cube", NULL, NULL);

   if (nullptr == window)
   {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      exit(-1);
   }

   glfwMakeContextCurrent(window);

   // glad
   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "Failed to initialize GLAD" << std::endl;
      exit(0);
   }

   // OpenGL initialization stuffs
   glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

   return(window);
}
void nextVelPos(double velocity, double position, double groundHeight, double dt) {
   double g = 10 * GRAVITY_FACTOR;
   double elasticity = 0.8;
   double v = velocity;
   double s = position;
   double next_v = v - g * dt;
   double next_s = s + v * dt;
   if (next_s < groundHeight) {
      next_s = groundHeight;
      next_v = -next_v * elasticity;
   }
   if ((next_s < groundHeight + abs(groundHeight) * 0.0005) && next_v < 0.0005) {
      next_s = groundHeight;
      next_v = 0;
   }
   immediateVel = next_v;
   immediatePos = next_s;
}
unsigned int loadTexture(char const* fileName) {
   unsigned int texture;

   // Create texture ids.
   glGenTextures(1, &texture);

   // All upcomming GL_TEXTURE_2D operations now on "texture" object
   glBindTexture(GL_TEXTURE_2D, texture);

   // Set texture parameters for wrapping.
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

   // Set texture parameters for filtering.
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   int width, height, nrChannels;
   unsigned char* image = stbi_load(fileName, &width, &height, &nrChannels, 0);
   if (!image) {
      printf("texture %s loading error ... \n", fileName);
   }
   else printf("texture %s loaded\n", fileName);

   GLenum format;
   if (nrChannels == 1) format = GL_RED;
   else if (nrChannels == 3) format = GL_RGB;
   else if (nrChannels == 4) format = GL_RGBA;

   glBindTexture(GL_TEXTURE_2D, texture);
   glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
   glGenerateMipmap(GL_TEXTURE_2D);

   return texture;
}
