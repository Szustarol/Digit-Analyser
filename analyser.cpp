/*
* (C) Karol Szustakowski 2019
* Digit Analyser - software to analyse
* handwritten digits using "neural network"
* that evolves to fit a right score
* This program is very simple, not containing
* any advanced AI stuff, It was created to
* learn both OpenGL programming which is heavily
* used here, and grasp some knowledge about
* how a neural network works.
*
* Please keep in mind that this software lacks comments
* and was written in a hurry - It might not be a
* good source of knowledge.
*
* Huge thanks to Sean Barrett for providing the
* STBI library
* And to sol-prog for the perlin noise class
* https://github.com/sol-prog
*/


#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <vector>
#include <future>
#define STB_IMAGE_IMPLEMENTATION
#include "shader.hpp"
#include "UI/uiObject.hpp"
#include "UI/drawArea.hpp"
#include "UI/button.hpp"
#include "data/texture.hpp"
#include "data/calculations.hpp"
#include "data/network.hpp"
#include "dirent.h"

const std::string tex_suffix = "data/digit_";

std::vector <UIObject *> objects;
bool mousePressed = false;

unsigned anDigit = 1;

GLFWwindow * setup();
void keyPressCallback(GLFWwindow * window, int key, int scancode, int action, int mods){
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

unsigned aSteps = 0;

std::vector<std::string> dFN; //digit data file - names;
std::vector<int> dFnum; //digit data file - digit

bool ShouldAnalyze = false;

void toggleAnalyze(int param){
  if(ShouldAnalyze == true){
    ShouldAnalyze = false;
    for(unsigned i = 0; i < objects.size(); i++){
      if(objects.at(i)->name == "learning_toggle"){
        objects.at(i)->tex = Texture::findByPath("data/learnStart.png");
        break;
      }
    }
  }
  else{
    ShouldAnalyze = true;
    for(unsigned i = 0; i < objects.size(); i++){
      if(objects.at(i)->name == "learning_toggle"){
        objects.at(i)->tex = Texture::findByPath("data/learnStop.png");
        break;
      }
    }
  }
}

int analyseBunch(std::vector<network *> networks, const std::string & fname, int fnum){
  int iWidth, iHeight, nchan;
  unsigned char * data = stbi_load(fname.c_str(), &iWidth, &iHeight, &nchan, 0);
  for(unsigned i = 0; i < networks.size(); i++){
    networks.at(i)->analyse(iWidth, iHeight, 0, fnum, data);
  }
  stbi_image_free((void*)data);
  return 0;
}

void analyseParalell(){
  std::vector<std::vector<network *> > networks(10);
  unsigned part = network::networks.size() / 10;
  for(unsigned i = 0; i < 10; i++){
    for(unsigned j = i*part; j < i*part+part; j++){
      networks.at(i).push_back(&network::networks.at(j));
    }
    if(i == 9){
      for(unsigned j = i*part + part; j < network::networks.size(); j++){
        networks.at(i).push_back(&network::networks.at(j));
      }
    }
  }
  std::random_shuffle(networks.begin(), networks.end());
  std::vector<std::string> fnames;
  std::vector<int> fnums;
  if(dFN.size() > 10){
    unsigned pos = 0;
    int indexes[10] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    while(pos != 10){
      indexes[pos] = rand() % dFN.size();
      for(unsigned i = 0; i < pos; i++){
        if(indexes[i] == indexes[pos]) continue;
      }
      pos++;
    }

    for(unsigned i = 0; i < 10; i++){
      fnames.push_back(dFN.at(indexes[i]));
      fnums.push_back(dFnum.at(indexes[i]));
    }
  }
  else{
    fnames = dFN;
    fnums = dFnum;
  }

  std::vector<std::thread > compute;
  for(unsigned i = 0; i < 10; i++){
    compute.emplace_back(std::thread(analyseBunch, networks.at(i), fnames.at(i), fnums.at(i)));
  }

  for(unsigned i = 0; i < compute.size(); i++){
    compute.at(i).join();
  }
}

void analyseNext(){
  analyseParalell();
  aSteps++;
  if(aSteps == 90){//next generation
    std::sort(network::networks.begin(), network::networks.end(), [](const network& lhs, const network& rhs){return lhs.gGuess > rhs.gGuess; });
    network::networks.erase(network::networks.begin()+10, network::networks.end());
    network::saveAll();
    std::cout << network::networks.at(0).gGuess/90 << "-" << network::networks.at(9).gGuess/90 << std::endl;
    for(unsigned i = 0; i < 10; i++){//10 best networks are saved
      network::networks.at(i).lastscore = network::networks.at(i).gGuess/90;
      network::networks.at(i).gGuess = 0;
      network::networks.at(i).bGuess = 0;
      for(unsigned j = 0; j < 5; j++){
        network n = network::networks.at(i);
        n.mutate();
        network::networks.push_back(n);
      }
    }
    aSteps = 0;
  }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
      mousePressed = false;
      double xpos, ypos;
      glfwGetCursorPos(window, &xpos, &ypos);
      for(unsigned i = 0; i < objects.size(); i++){
        if(objects.at(i)->type == UIObject::BUTTON){
          Button * btn = (Button*)objects.at(i);
          if(btn->posX < xpos && btn->posX+btn->realWidth > xpos
          && btn->posY < ypos && btn->posY+btn->realHeight > ypos){
            btn->click();
            break;
          }
        }
      }
    }
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
      mousePressed = true;

}

void scrollCallback(GLFWwindow * window, double xoffset, double yoffset){
  for(unsigned i = 0; i < objects.size(); i++){
    if(objects.at(i)->name == "main_drawing_area"){
      ((DrawArea*)(objects.at(i)))->resize(yoffset);
    }
  }
}

void coursorPosCallback(GLFWwindow * window, double posX, double posY){
  if(mousePressed){
    int id = -1;
    for(unsigned i = 0; i < objects.size(); i++){
      if(objects.at(i)->name == "main_drawing_area"){
        id = i;
        break;
      }
    }
    if(id != -1){
      DrawArea * a = (DrawArea*)objects.at(id);
      if(posX > a->posX && posX < a->posX + a->realWidth && posY > a->posY && posY < a->posY + a->realHeight){
        a->putPoint(posX-a->posX, posY-a->posY);
      }
    }
  }
}

void framebufferSizeCallback(GLFWwindow * window, int width, int height){
  glViewport(0, 0, width, height);
  for(unsigned i = 0; i < objects.size(); i++){
    objects.at(i)->updateGlue(width, height);
    if(objects.at(i)->name == "main_drawing_area"){
      ((DrawArea*)(objects.at(i)))->crop();
    }
  }
}

void saveImageCallback(int digit){
  for(unsigned i = 0; i < objects.size(); i++){
    if(objects.at(i)->type == UIObject::DRAWAREA){
      ((DrawArea*)objects.at(i))->saveAndClear(digit);
      break;
    }
  }
}

int main(){

  GLFWwindow * mainWindow = setup();

  Shader shader("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");
  shader.use();

  shader.setAttribf(0, 3, sizeof(vertex), offsetof(vertex, pos));
  shader.setAttribf(1, 4, sizeof(vertex), offsetof(vertex, color));
  shader.setAttribf(2, 2, sizeof(vertex), offsetof(vertex, texPos));

  for(unsigned i = 0; i < 9; i++){
    Button * obj = new Button;
    obj->posX = 20;
    obj->posY = 40 + 60 * i;
    obj->stick = 0;
    obj->realWidth = 50;
    obj->realHeight = 50;
    obj->stretchX = true;
    obj->stretchY = 1;
    obj->name = "btn_" + std::to_string(i);
    for(unsigned j = 0 ; j < textures.size(); j++){
      if(textures.at(j)->name == tex_suffix + std::to_string(i+1) + ".png"){
        obj->tex = textures.at(j);
        obj->id = j+1;
        break;
      }
    }
    obj->updateGlue(800, 600);

    obj->onClick = &saveImageCallback;

    objects.push_back(obj);
  }

  DrawArea * obj = new DrawArea();
  obj->realWidth = 350;
  obj->realHeight= 540;
  obj->stretchX = true;
  obj->stretchY = true;
  obj->posX = 80;
  obj->posY = 40;
  obj->updateGlue(800, 600);
  obj->name = "main_drawing_area";
  objects.push_back(obj);

  UIObject * pred = new UIObject();
  pred->realWidth = 200;
  pred->realHeight = 200;
  pred->stretchX = true;
  pred->stretchY = true;
  pred->posY = 40;
  pred->posX = 440;
  pred->updateGlue(800, 600);
  pred->name = "result_prediciton";
  pred->setColor(0.0f, 0.5f, 0.2f);
  objects.push_back(pred);

  UIObject * learn = new Button();
  learn->realWidth = 200;
  learn->realHeight = 200;
  learn->stretchX = true;
  learn->stretchY = true;
  learn->posY = 260;
  learn->posX= 440;
  learn->tex = Texture::findByPath("data/learnStart.png");
  ((Button *)learn)->onClick = &toggleAnalyze;
  learn->updateGlue(800, 600);
  learn->name = "learning_toggle";
  learn->setColor(0.2f, 0.0f, 0.3f);
  objects.push_back(learn);

  while(!glfwWindowShouldClose(mainWindow)){
    glClear(GL_COLOR_BUFFER_BIT);

    if(ShouldAnalyze)
      analyseNext();

    rawData * rd = nullptr;

    for(unsigned i = 0; i < objects.size(); i++){
      if(objects.at(i)->name == "main_drawing_area"){
        DrawArea * da = (DrawArea*)objects.at(i);
        da->draw(shader);
        if(!ShouldAnalyze)
          rd = da->getData();
      }
      else{
        objects.at(i)->draw(shader);
      }
    }

    if(!mousePressed && !ShouldAnalyze){
      if(rd != nullptr){
        int counts[10];
        for(unsigned i = 0; i < 10; i++) counts[i] = 0;
        for(unsigned i = 0; i < 10; i++){
          int result = network::networks.at(i).analyse(rd->width, rd->height, 0, -1, rd->imageDataGr);
          counts[result] ++;
        }
        int largestCnt = 0; int cntIdx = 0;
        for(unsigned i = 0; i < 10; i++){
          if(counts[i] > largestCnt){
            largestCnt = counts[i];
            cntIdx = i;
          }
        }
        for(unsigned i = 0; i < objects.size(); i++){
          if(objects.at(i)->name == "result_prediciton"){
            objects.at(i)->tex = Texture::findByPath(tex_suffix + std::to_string(cntIdx) + ".png");
            break;
          }
        }
    }
    }

    glfwSwapBuffers(mainWindow);
    glfwPollEvents();
  }

  glfwTerminate();

  for(int i = objects.size()-1; i >= 0; i--)
    delete objects.at(i);

  Texture::freeAll();
}


GLFWwindow * setup(){

  glfwInit();

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

  GLFWwindow * w = glfwCreateWindow(800, 600, "Digit Analyser", NULL, NULL);

  if(!w){
    std::cerr << "Error while opening a window." << std::endl;
  }

  glfwMakeContextCurrent(w);

  glewExperimental = GL_TRUE;

  glewInit();

  glViewport(0, 0, 800, 600);

  glfwSetFramebufferSizeCallback(w, framebufferSizeCallback);
  glfwSetKeyCallback(w, keyPressCallback);
  glfwSetCursorPosCallback(w, coursorPosCallback);
  glfwSetMouseButtonCallback(w, mouseButtonCallback);
  glfwSetScrollCallback(w, scrollCallback);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  for(unsigned i = 0; i < 9; i++){
    std::string name= tex_suffix + std::to_string(i+1) + ".png";
    Texture::load(name.c_str());
  }

  Texture::load("data/learnStart.png");
  Texture::load("data/learnStop.png");

  for(unsigned i = 1; i <= 9; i++){
    DIR * dir;
    dirent * ent;
    dir = opendir(("data/" + std::to_string(i)).c_str());
    while((ent = readdir(dir)) != NULL){
      if(strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0){
        dFN.push_back("data/" + std::to_string(i) + "/" + std::string(ent->d_name));
        dFnum.push_back(i);
      }
    }
    closedir(dir);
  }

  network::loadAll();
  pattern::loadAll();
  srand(time(NULL));

  for(unsigned i = 0; i < 10; i++){//10 best networks are saved
    for(unsigned j = 0; j < 5; j++){
      network n = network::networks.at(i);
      n.mutate();
      network::networks.push_back(n);
    }
  }


  pattern::loadAll();

  return w;
}
