#pragma once

#include "BaseApp.h"
#include "glm/glm.hpp"
#include <random>
#include <functional>

struct BoundingBox
{
  glm::vec3 bl; //front bottom left
  glm::vec3 tr; //back top right
  glm::vec3 color;
  BoundingBox(glm::vec3 p1=glm::vec3(0,0,0),glm::vec3 p2=glm::vec3(0,0,0),glm::vec3 c=glm::vec3(1,0,0)):bl(glm::min(p1,p2)),tr(glm::max(p1,p2)),color(c){}
  glm::vec3 center(){
    return bl+((tr-bl)*0.5f);
  }
};

struct Vertex
{
  glm::vec3 color;
  glm::vec3 position;
  glm::vec3 normal;
};

class BVH: public BaseApp{
public:
  BVH();
protected:

  /**
  * Init funkce, vola se jednou pri startu aplikace
  */
  virtual void init();

  /**
  * Draw funkc, vola se v kazdem snimku
  */
  virtual void draw();

  virtual void onMouseMove(int dx, int dy, int x, int y);
  virtual void onMousePress(Uint8 button, int x, int y);
  virtual void onMouseRelease(Uint8 button, int x, int y);
  virtual void onMouseWheel(int delta);
  virtual void onKeyPress(SDL_Keycode key, Uint16 mod);

  void updateMatrix();
  void addObject();
  void delObject();
  void drawBox(BoundingBox box);

  /* Ovladani */
  float zoom;
  bool rmbPressed;
  bool lmbPressed;
  int posx, posy;
  int rotx, roty;
  bool wireframe;
  bool update_model;
  bool draw_tree;
  bool frustrum_culling;
  bool cpu_enabled;
  
  void calcMortonsCPU();
  void calcMortonsGPU();
  void generateHierarchyCPU();
  void generateHierarchyGPU();
  void computeBoundingBoxes();
  BoundingBox computeBoundingBoxes(cl_int i,glm::vec3 color);
  void drawTree(glm::vec4 const (& fru)[6]);
  void drawNode(cl_int i,glm::vec4 const (& fru)[6]);
  cl_int clzMorton(cl_int indexA,cl_int indexB);
  
  /* Matice */
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;
  
  /* Shadery */
  GLuint vs, fs, prog;
  
  /* Buffery */
  GLuint vao; // vertex array
  GLuint ebo; // element buffer
  GLuint vbo; // vertex buffer
  
  /* Atributy */
  GLuint positionAttrib;
  GLuint normalAttrib;
  GLuint colorAttrib;

  /* Uniformy */
  GLuint mvpUniform;
  GLuint lightPositionUniform;
  GLuint LaUniform;
  GLuint LdUniform;
  
  /* Vrcholy k vykresleni */
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  
  /* Mnozina primitivnich objektu, pro ktere se bude
     vytvaret bvh strom. Jedna se o kvadry, takze
     maji implicitne definovany svuj bounding box */
  std::vector<BoundingBox> objects;
  std::vector<cl_float> centers;
  std::vector<cl_uint> mortons;
  std::vector<cl_uint> mortons_cpu;
  std::vector<cl_int> indexes;
  
  cl_uint object2draw;
  std::vector<BoundingBox> box2draw;
  
  std::vector<BoundingBox> treeBB;
  std::vector<cl_int> tree;
  std::vector<cl_int> tree_cpu;
  
  std::default_random_engine generator;
  std::uniform_real_distribution<float> posDist;
  std::uniform_real_distribution<float> sizeDist;
  std::uniform_real_distribution<float> colorDist;
  std::function<float ()> randPosition;
  std::function<float ()> randSize;
  std::function<float ()> randColor;
  
  cl::Device device;
  cl::Context context;
  cl::CommandQueue queue;
  cl::Program program;
  cl::Buffer centers_dev;
  cl::Buffer mortons_dev;
  cl::Buffer indexes_dev;
  cl::Buffer tree_dev;
  
  cl::UserEvent in_mortons_event;
  cl::UserEvent in_hierarchy_event;
  cl::UserEvent out_mortons_event;
  cl::UserEvent out_hierarchy_event;
  cl::Event kernel_mortons_event;
  cl::Event kernel_hierarchy_event;
};
