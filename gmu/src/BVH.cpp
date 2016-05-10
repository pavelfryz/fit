#include "BVH.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/ext.hpp"
#include <chrono>
#include <algorithm> 
#include <bitset>

#define MAX_CUBES 20000
#define MAX_CUBES_VERTICES MAX_CUBES*24
#define MAX_CUBES_INDICES MAX_CUBES_VERTICES*2
#define WORLD_MIN -100
#define WORLD_MAX 100
#define WORLD_VEC_MIN glm::vec3(WORLD_MIN,WORLD_MIN,WORLD_MIN)
#define WORLD_VEC_MAX glm::vec3(WORLD_MAX,WORLD_MAX,WORLD_MAX)
#define BOX_MIN 0.5
#define BOX_MAX 5

#define SELECTED_DEVICE_TYPE CL_DEVICE_TYPE_CPU

using namespace std;

// false if fully outside, true if inside or intersects
bool boxInFrustum( glm::vec4 const (& fru)[6], BoundingBox const & box )
{
  // check box outside/inside of frustum
  for( int i=0; i<6; i++ ){
    int out = 0;
    out += (glm::dot(fru[i], glm::vec4(box.bl, 1.0f) ) < 0.0 )?1:0;
    out += (glm::dot(fru[i], glm::vec4(box.tr.x, box.bl.y, box.bl.z, 1.0f) ) < 0.0 )?1:0;
    out += (glm::dot(fru[i], glm::vec4(box.bl.x, box.tr.y, box.bl.z, 1.0f) ) < 0.0 )?1:0;
    out += (glm::dot(fru[i], glm::vec4(box.tr.x, box.tr.y, box.bl.z, 1.0f) ) < 0.0 )?1:0;
    out += (glm::dot(fru[i], glm::vec4(box.bl.x, box.bl.y, box.tr.z, 1.0f) ) < 0.0 )?1:0;
    out += (glm::dot(fru[i], glm::vec4(box.tr.x, box.bl.y, box.tr.z, 1.0f) ) < 0.0 )?1:0;
    out += (glm::dot(fru[i], glm::vec4(box.bl.x, box.tr.y, box.tr.z, 1.0f) ) < 0.0 )?1:0;
    out += (glm::dot(fru[i], glm::vec4(box.tr, 1.0f) ) < 0.0 )?1:0;
    if( out==8 ) return false;
  }

  return true;
}

double getEventTime(cl::Event i_event)
{
  return double(i_event.getProfilingInfo<CL_PROFILING_COMMAND_END>() - i_event.getProfilingInfo<CL_PROFILING_COMMAND_START>()) / 1000000;
}

BVH::BVH():zoom(32),rmbPressed(false),lmbPressed(false),posx(0),posy(0),rotx(0),roty(0),
wireframe(false),update_model(false),draw_tree(false),frustrum_culling(false),cpu_enabled(true),
generator(std::chrono::system_clock::now().time_since_epoch().count()),
posDist(WORLD_MIN, WORLD_MAX-BOX_MIN),sizeDist(BOX_MIN,BOX_MAX),colorDist(0.0,1.0),randPosition(std::bind(posDist,generator)),
randSize(std::bind(sizeDist,generator)),randColor(std::bind(colorDist,generator)){
}

void BVH::init() {  
  /*
  * Vytvoreni shader programu
  */

  /* Muzete zmenit podle potreby (linux/mingw)*/
  //string prefix = "../shaders/";
  string prefix = "./shaders/";

  /*
  * Kompilace shaderu
  */
  vs = compileShader(GL_VERTEX_SHADER, loadFile(prefix + "bvh.vert"));
  fs = compileShader(GL_FRAGMENT_SHADER, loadFile(prefix + "bvh.frag"));
  prog = linkShader(2, vs, fs);
  
  // ziskani atributu
  positionAttrib = glGetAttribLocation(prog, "position");
  normalAttrib = glGetAttribLocation(prog, "normal");
  colorAttrib = glGetAttribLocation(prog, "color");
  
  // ziskani uniform
  mvpUniform = glGetUniformLocation(prog, "mvp");
  lightPositionUniform = glGetUniformLocation(prog, "lightPosition");
  LaUniform = glGetUniformLocation(prog, "La");
  LdUniform = glGetUniformLocation(prog, "Ld");
  
  // vygenerovani jmena pro VAO 
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  /*
  * Vytvoreni bufferu 
  */
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*MAX_CUBES_VERTICES*2, NULL, GL_DYNAMIC_DRAW);
  
  glEnableVertexAttribArray(positionAttrib);
  glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(normalAttrib);
  glVertexAttribPointer(normalAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
  glEnableVertexAttribArray(colorAttrib);
  glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint)*MAX_CUBES_INDICES*2, NULL,GL_DYNAMIC_DRAW);
  
  glBindVertexArray(0);
  

  cl_int err_msg, err_msg2;
  std::vector<cl::Platform> platforms;
  std::vector<cl::Device> platform_devices;
  // Ziskani platforem
  CheckOpenCLError(cl::Platform::get(&platforms), "cl::Platform::get");
  std::cout<<"Platforms:"<<std::endl;
  for(size_t i = 0; i < platforms.size(); i++){
    std::cout<<" "<<i<<". platform name: "<<platforms[i].getInfo<CL_PLATFORM_NAME>(&err_msg)<<"."<<std::endl;
    CheckOpenCLError(err_msg,"cl::Platform::getInfo<CL_PLATFORM_NAME>");
    CheckOpenCLError(platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &platform_devices), "getDevices");
    for(size_t j = 0; j < platform_devices.size(); j++){
      std::cout<<"  "<<j<<". device name: "<<platform_devices[j].getInfo<CL_DEVICE_NAME>(&err_msg)<<"."<<std::endl;
      CheckOpenCLError(err_msg, "cl::Device::getInfo<CL_DEVICE_NAME>");
    }
    platform_devices.clear();
  }

  bool device_found = false;
  // Vybrani zarizeni
  for(unsigned int i = 0; i < platforms.size(); i++){
    CheckOpenCLError(platforms[i].getDevices(SELECTED_DEVICE_TYPE, &platform_devices), "getDevices");
    if(platform_devices.size() != 0){
      device_found = true;
      device = platform_devices[0];
      break;
    }
  }
  if(!device_found) CheckOpenCLError(CL_DEVICE_NOT_FOUND, "GPU device");
    
  std::cout<<"Selected device name: "<<device.getInfo<CL_DEVICE_NAME>()<<"."<<std::endl;
  platforms.clear();

  context = cl::Context(device, NULL, NULL, NULL, &err_msg);
  CheckOpenCLError(err_msg,"cl::Context");

  queue = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err_msg);
  CheckOpenCLError(err_msg,"cl::CommandQueue");
    
  std::string program_source = loadFile(prefix + "bvh.cl");
  cl::Program::Sources sources(1,std::make_pair(program_source.c_str(),program_source.size()));

  program = cl::Program(context, sources, &err_msg);
  CheckOpenCLError(err_msg,"clCreateProgramWithSource");
  
  // Prelozeni kernelu
  if((err_msg = program.build(std::vector<cl::Device>(1,device), "", NULL, NULL)) == CL_BUILD_PROGRAM_FAILURE){
    std::cout<<"Build log:"<<std::endl<<" "<<program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device, &err_msg2)<<std::endl;
    CheckOpenCLError(err_msg2, "cl::Program::getBuildInfo<CL_PROGRAM_BUILD_LOG>");
  }
  CheckOpenCLError(err_msg, "clBuildProgram");

  centers_dev = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(cl_float)*3*MAX_CUBES, NULL, &err_msg);
  CheckOpenCLError(err_msg,"clCreateBuffer: centers_dev");
  mortons_dev = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(cl_int)*MAX_CUBES, NULL, &err_msg);
  CheckOpenCLError(err_msg,"clCreateBuffer: mortons_dev");
  indexes_dev = cl::Buffer (context, CL_MEM_READ_ONLY, sizeof(cl_int)*MAX_CUBES, NULL, &err_msg);
  CheckOpenCLError(err_msg,"clCreateBuffer: centers_dev");
  tree_dev = cl::Buffer (context, CL_MEM_READ_WRITE, sizeof(cl_int)*2*(MAX_CUBES-1), NULL, &err_msg);
  CheckOpenCLError(err_msg,"clCreateBuffer: tree_dev");
  
  for(int i=0;i<90;++i)addObject();
}


//pomocna funkce na porovnani morton kodu pri razeni
struct compMorton {
  std::vector<cl_uint> &mortons;
  compMorton(std::vector<cl_uint> &mortons):mortons(mortons){}
  bool operator() (cl_int i,cl_int j) { return (mortons[i]<mortons[j]);}
};

//rekurzivni vykresleni uzlu bvh stromu
void BVH::drawTree(glm::vec4 const (& fru)[6]){
  vertices.clear();
  indices.clear();
  box2draw.clear();
  object2draw=0;
  drawNode(0,fru);
  for(size_t i=0;i<box2draw.size();++i) drawBox(box2draw[i]);
  
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  void* verts = glMapBuffer(GL_ARRAY_BUFFER,GL_WRITE_ONLY);
  memcpy(verts,vertices.data(), sizeof(Vertex)*vertices.size());
  glUnmapBuffer(GL_ARRAY_BUFFER);
    
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo);
  void* indis = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER,GL_WRITE_ONLY);
  memcpy(indis,indices.data(), sizeof(GLuint)*indices.size());
  glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
}

void BVH::drawNode(cl_int i,glm::vec4 const (& fru)[6]){
  if(i<0){ 
    i=-i-1;
    if(!frustrum_culling||boxInFrustum(fru,objects[i])){
      object2draw++;
      drawBox(objects[i]);
    }
  }else if(!frustrum_culling||boxInFrustum(fru,treeBB[i])){
    drawNode(tree[2*i],fru);
    drawNode(tree[2*i+1],fru);
    box2draw.push_back(treeBB[i]);
  }
}

void BVH::draw(){
  if(update_model){
    update_model = false;
    
    //pocitani mortonova kod pro kazdy objekt
    double cpu_start = GetTime();
    if(cpu_enabled)
      calcMortonsCPU();
    double cpu_end = GetTime();
    calcMortonsGPU();
    
    printf("calcMortons: cpu:%.3fms ocl:%.3fms ocl_copy:%.3fms ocl_kernel:%.3fms\n",
      (cpu_end - cpu_start) * 1000,
      getEventTime(in_mortons_event) + getEventTime(out_mortons_event) + getEventTime(kernel_mortons_event),
      getEventTime(in_mortons_event) + getEventTime(out_mortons_event),
      getEventTime(kernel_mortons_event));
    
    //setrideni objetu podle mortonova kodu
    compMorton comp(mortons);
    std::sort(indexes.begin(),indexes.end(),comp);
    //vytvoreni bvh stromu
    double cpu_start2 = GetTime();
    if(cpu_enabled)
      generateHierarchyCPU();
    double cpu_end2 = GetTime();
    generateHierarchyGPU();
    
    printf("generateHierarchy: cpu:%.3fms ocl:%.3fms ocl_copy:%.3fms ocl_kernel:%.3fms\n",
      (cpu_end2 - cpu_start2) * 1000,
      getEventTime(in_hierarchy_event) + getEventTime(out_hierarchy_event) + getEventTime(kernel_hierarchy_event),
      getEventTime(in_hierarchy_event) + getEventTime(out_hierarchy_event),
      getEventTime(kernel_hierarchy_event));
      
    printf("Both: cpu:%.3fms ocl:%.3fms ocl_copy:%.3fms ocl_kernel:%.3fms\n",
      (cpu_end - cpu_start) * 1000 + (cpu_end2 - cpu_start2) * 1000,
      getEventTime(in_mortons_event) + getEventTime(out_mortons_event) + getEventTime(kernel_mortons_event) + 
      getEventTime(in_hierarchy_event) + getEventTime(out_hierarchy_event) + getEventTime(kernel_hierarchy_event),
      getEventTime(in_mortons_event) + getEventTime(out_mortons_event) + 
      getEventTime(in_hierarchy_event) + getEventTime(out_hierarchy_event),
      getEventTime(kernel_mortons_event) + 
      getEventTime(kernel_hierarchy_event));  
      
    //vypocet bounding boxu jednotlivych uzlu stromu  
    computeBoundingBoxes();
  }
  
  // Matice
  updateMatrix();
  
  glViewport(0, 0, width, height);
  
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  
  /* Aktivace vertex a array*/
  glBindVertexArray(vao);
  /* Aktivace programu */
  glUseProgram(prog);
  
  glm::mat4 mvp = projection*view*model;
  glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(mvp));
  
  glm::vec4 right( mvp[0][3] - mvp[0][0], mvp[1][3] - mvp[1][0], mvp[2][3] - mvp[2][0], mvp[3][3] - mvp[3][0] );
  glm::vec4 left( mvp[0][3] + mvp[0][0], mvp[1][3] + mvp[1][0], mvp[2][3] + mvp[2][0], mvp[3][3] + mvp[3][0] );
  glm::vec4 bottom( mvp[0][3] + mvp[0][1], mvp[1][3] + mvp[1][1], mvp[2][3] + mvp[2][1], mvp[3][3] + mvp[3][1] );
  glm::vec4 top( mvp[0][3] - mvp[0][1], mvp[1][3] - mvp[1][1], mvp[2][3] - mvp[2][1], mvp[3][3] - mvp[3][1] );
  glm::vec4 far( mvp[0][3] - mvp[0][2], mvp[1][3] - mvp[1][2], mvp[2][3] - mvp[2][2], mvp[3][3] - mvp[3][2] );
  glm::vec4 near( mvp[0][3] + mvp[0][2], mvp[1][3] + mvp[1][2], mvp[2][3] + mvp[2][2], mvp[3][3] + mvp[3][2] );
  glm::vec4 frustrum[6]={right,left,bottom,top,far,near};

  drawTree(frustrum);
  if(frustrum_culling) std::cout <<"Total objects: "<<objects.size()<<" Drawn objects: "<<object2draw<<std::endl;

  // Prepinani na dratovy model - klavesa W
  if (wireframe) {
    glLineWidth(2.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  glDrawElements(GL_TRIANGLES, object2draw*36, GL_UNSIGNED_INT, 0);
  
  // Zapinani vykreslovani stromu - klavesa X
  if(draw_tree){
    glLineWidth(2.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, box2draw.size()*36, GL_UNSIGNED_INT, (GLvoid*)(object2draw * 36 * sizeof(GLuint)));
  }
}

void BVH::onMouseMove(int dx, int dy, int x, int y){
  if (rmbPressed||lmbPressed) {
    rotx += dx;
    roty += dy;
    roty = max(min(roty, 89), -89);
  }
  posx = x;
  posy = y;
}

void BVH::onMousePress(Uint8 button, int x, int y){
  switch (button) {
  case SDL_BUTTON_LEFT:
    lmbPressed = true;
    break;
  case SDL_BUTTON_MIDDLE:
    break;
  case SDL_BUTTON_RIGHT:
    rmbPressed = true;
    break;
  }
}

void BVH::onMouseRelease(Uint8 button, int x, int y){
  switch (button) {
  case SDL_BUTTON_LEFT:
    lmbPressed = false;
    break;
  case SDL_BUTTON_MIDDLE:
    break;
  case SDL_BUTTON_RIGHT:
    rmbPressed = false;
    break;
  }
}

void BVH::onMouseWheel(int delta){
  if (delta > 0) {
    zoom /= 1.1;
  }else {
    zoom *= 1.1;
  }
}

void BVH::onKeyPress(SDL_Keycode key, Uint16 mod){
  switch (key) {
  case SDLK_ESCAPE:
    quit();
    break;
  case SDLK_d:
    if(objects.size()<=100){
      if(objects.size()>2)
        delObject();
    }else
      for(int i=0;i<100;++i)
        delObject();
    break;
  case SDLK_a:
    if(objects.size()<100)
      addObject();
    else
      for(int i=0;i<100;++i)
        addObject();
    break;
  case SDLK_w:
    wireframe = !wireframe;
    break;
  case SDLK_x:
    draw_tree = !draw_tree;
    break;
  case SDLK_c:
    frustrum_culling = !frustrum_culling;
    break;
  case SDLK_z:
    cpu_enabled = !cpu_enabled;
    break;
  }
}

void BVH::updateMatrix() {
  model = glm::mat4(1.0);

  float radx = glm::radians((float)rotx);
  float rady = glm::radians((float)roty);
  float x = zoom * cos(rady) * cos(radx);
  float y = zoom * sin(rady);
  float z = zoom * cos(rady) * sin(radx);

  glm::vec3 eye(x, y, z);
  glm::vec3 center(0, 0, 0);
  glm::vec3 up(0, 1, 0);
  view = glm::lookAt(eye, center, up);
  projection = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
}

//pridani objektu do sceny
void BVH::addObject() {
  if(objects.size()<MAX_CUBES){
    glm::vec3 position(randPosition(),randPosition(),randPosition());
    glm::vec3 size(randSize(),randSize(),randSize());
    BoundingBox bb=BoundingBox(position,glm::min(position+size,WORLD_VEC_MAX),glm::vec3(randColor(),randColor(),randColor()));
    glm::vec3 center=bb.center();
    objects.push_back(bb);
    centers.push_back(center.x);
    centers.push_back(center.y);
    centers.push_back(center.z);
    indexes.push_back(indexes.size());
    mortons.push_back(0);
    mortons_cpu.push_back(0);
    update_model = true;
  }
}

//odebrani objektu ze sceny
void BVH::delObject() {
  objects.pop_back();
  centers.pop_back();
  centers.pop_back();
  centers.pop_back();
  for(size_t i = 0;i<indexes.size();++i){
    if(indexes[i]==(cl_int)indexes.size()-1){
      indexes[i]=indexes.back();
      indexes.pop_back();
      break;
    }
  }
  mortons.pop_back();
  mortons_cpu.pop_back();
  update_model = true;
}

unsigned int iCeilTo(unsigned int data, unsigned int align_size)
{
  return ((data - 1 + align_size) / align_size) * align_size;
}

//rozsireni 10 bitoveho cisla na 30 bitove, pridanim 2 nul
//pred kazdy bit
//napr.: z    0   0   1   1   0   1   0   1   1   0
//       na 000 000 001 001 000 001 000 001 001 000
//https://fgiesen.wordpress.com/2009/12/13/decoding-morton-codes/
cl_int shift2(cl_int x){
  x = (x ^ (x << 16)) & 0xff0000ff; 
  x = (x ^ (x <<  8)) & 0x0300f00f; 
  x = (x ^ (x <<  4)) & 0x030c30c3; 
  x = (x ^ (x <<  2)) & 0x09249249; 
  return x;
}

//vypocet 30 bitove mortonova kodu na CPU
void BVH::calcMortonsCPU() {
  glm::vec3 worldmin = WORLD_VEC_MIN;
  glm::vec3 scale = 1024.0f/(WORLD_VEC_MAX-WORLD_VEC_MIN);
  
  for(size_t i=0;i<objects.size();++i){
    cl_int x = (centers[3*i]-worldmin.x)*scale.x; 
    cl_int y = (centers[3*i+1]-worldmin.y)*scale.y; 
    cl_int z = (centers[3*i+2]-worldmin.z)*scale.z; 
    if(x>=1024) x = 1023;
    if(y>=1024) y = 1023;
    if(z>=1024) z = 1023;
    x = shift2(x);
    y = shift2(y);
    z = shift2(z);
    mortons_cpu[i] = (x<<2) + (y<<1) + z;
  }
}

//vypocet 30 bitove mortonova kodu na GPU
void BVH::calcMortonsGPU() {
  cl_int err_msg;
  glm::vec3 worldmin = WORLD_VEC_MIN;
  glm::vec3 scale = 1024.0f/(WORLD_VEC_MAX-WORLD_VEC_MIN);
  
  cl::make_kernel<cl::Buffer&, cl::Buffer&, cl_int&, cl_float&, cl_float&,cl_float&, 
      cl_float&,cl_float&, cl_float&>calcMortonsKernel(program, "calcMortons", &err_msg);
  
  cl::NDRange local(256);
  cl::NDRange global(iCeilTo(objects.size(), local[0]));
  cl_int count = objects.size();
  cl_float worldminx = worldmin.x;
  cl_float worldminy = worldmin.y;
  cl_float worldminz = worldmin.z;
  cl_float scalex = scale.x;
  cl_float scaley = scale.y;
  cl_float scalez = scale.z;
  
  CheckOpenCLError(queue.enqueueWriteBuffer(centers_dev, CL_FALSE, 0, sizeof(cl_float)*centers.size(), centers.data(), 
      NULL, &in_mortons_event),"clEnqueueWriteBuffer: centers_dev"); 
  kernel_mortons_event = calcMortonsKernel(cl::EnqueueArgs(queue, global, local), 
      centers_dev, mortons_dev,count,worldminx,worldminy,
      worldminz, scalex,scaley,scalez);
  CheckOpenCLError(queue.enqueueReadBuffer(mortons_dev, CL_FALSE, 0, sizeof(cl_int)*objects.size(), mortons.data(), 
      NULL, &out_mortons_event),"clEnqueueReadBuffer: mortons_dev");
  // synchronize queue
  CheckOpenCLError(queue.finish(), "clFinish");
}

//funkce pro vypocet poctu nulovych bitu
//napr.: pro cizlo 00010010... vrati 3
//http://www.docjar.com/html/api/java/lang/Integer.java.html numberOfLeadingZeros
cl_int clz(cl_uint x) {
  if (x == 0) return 32;
  cl_uint n = 1;
  if (x >> 16 == 0) { n += 16; x <<= 16; }
  if (x >> 24 == 0) { n +=  8; x <<=  8; }
  if (x >> 28 == 0) { n +=  4; x <<=  4; }
  if (x >> 30 == 0) { n +=  2; x <<=  2; }
  n -= x >> 31;
  return n;
}

//funkce pro vypocet poctu shodnych bitu dvou mortonovych kodu
cl_int BVH::clzMorton(cl_int indexA,cl_int indexB)
{
  if(indexB<0||indexB>=(cl_int)indexes.size()) return -1;
  cl_int n = clz(mortons[indexes[indexA]]^mortons[indexes[indexB]]);
  if(n==32) n+=clz(indexA^indexB);
  return n;
}


BoundingBox BVH::computeBoundingBoxes(cl_int i,glm::vec3 color){
  if(i<0)
    return objects[indexes[-i-1]];
  glm::vec3 childcolor = glm::vec3(randColor(),randColor(),randColor());
  BoundingBox left = computeBoundingBoxes(tree[2*i],childcolor);
  BoundingBox right = computeBoundingBoxes(tree[2*i+1],childcolor);
  return treeBB[i]=BoundingBox(glm::min(left.bl,right.bl),glm::max(left.tr,right.tr),color);
}

void BVH::computeBoundingBoxes() {
  treeBB.resize(objects.size()-1);
  computeBoundingBoxes(0,glm::vec3(1,1,1));
}

void BVH::generateHierarchyGPU() {
  tree.resize(2*(objects.size()-1));
  
  cl_int err_msg;
  cl::make_kernel<cl::Buffer&, cl::Buffer&, cl::Buffer&, cl_int&> generateHierarchyKernel(program, "generateHierarchy", &err_msg);
  
  cl::NDRange local(256);
  cl::NDRange global(iCeilTo(objects.size()-1, local[0]));
  cl_int count = objects.size()-1;
  
  CheckOpenCLError(queue.enqueueWriteBuffer(indexes_dev, CL_FALSE, 0, sizeof(cl_int)*indexes.size(), indexes.data(), 
      NULL, &in_hierarchy_event),"clEnqueueWriteBuffer: indexes_dev"); 
  kernel_hierarchy_event = generateHierarchyKernel(cl::EnqueueArgs(queue, global, local), 
      indexes_dev, mortons_dev,tree_dev,count);
      
  CheckOpenCLError(queue.enqueueReadBuffer(tree_dev, CL_FALSE, 0, sizeof(cl_int)*tree.size(), tree.data(), 
      NULL, &out_hierarchy_event),"clEnqueueReadBuffer: tree_dev");

  CheckOpenCLError(queue.finish(), "clFinish");
  
}
  
void BVH::generateHierarchyCPU() {
  tree_cpu.resize(2*(objects.size()-1));
 
  for(cl_int i=0;i<(cl_int)objects.size()-1;++i){
    // determine direction of range
    cl_int dir=(clzMorton(i,i+1)-clzMorton(i,i-1)>=0) ? 1 : -1;
    //compute upper bound for length of the range
    cl_int dMin = clzMorton(i,i-dir);
    cl_int lMax = 2;
    while(clzMorton(i,i+lMax*dir)>dMin) lMax *=2;
    //find the other using binary search
    cl_int l = 0;
    for(cl_int t=lMax/2;t>0;t/=2)
      if(clzMorton(i,i+(l+t)*dir)>dMin) l+=t;
    cl_int j = i+l*dir;  
    //find the split position using binary search
    cl_int first = min(i,j);
    cl_int last = max(i,j);
    cl_int commonPrefix = clzMorton(first,last);
    cl_int split = first;
    cl_int step = last-first;
    do{
      step=(step+1)/2;
      if(split+step<last&&clzMorton(first,split+step)>commonPrefix)
        split+=step;
    }while(step>1);
    
    //output child pointers
    cl_int left,right;
    if(first==split) left = -split-1; else left = split;
    if(last==split+1) right = -split-2; else right = split+1;
    tree_cpu[2*i]=left;
    tree_cpu[2*i+1]=right;
  }
}

void BVH::drawBox(BoundingBox box){
  GLuint i = vertices.size();
  Vertex v;
  v.color = box.color;
  //bottom
  v.normal = glm::vec3(0.0,-1.0,0.0);
  v.position = box.bl; vertices.push_back(v);
  v.position.x = box.tr.x; vertices.push_back(v);
  v.position.z = box.tr.z; vertices.push_back(v);
  v.position.x = box.bl.x; vertices.push_back(v);
  indices.push_back(i); indices.push_back(i+1); indices.push_back(i+2);
  indices.push_back(i); indices.push_back(i+2); indices.push_back(i+3);
  i+=4;
  //top
  v.normal = glm::vec3(0.0,1.0,0.0);
  v.position = box.tr; vertices.push_back(v);
  v.position.z = box.bl.z; vertices.push_back(v);
  v.position.x = box.bl.x; vertices.push_back(v);
  v.position.z = box.tr.z; vertices.push_back(v);
  indices.push_back(i); indices.push_back(i+1); indices.push_back(i+2);
  indices.push_back(i); indices.push_back(i+2); indices.push_back(i+3);
  i+=4;
  //left
  v.normal = glm::vec3(-1.0,0.0,0.0);
  v.position = box.bl; vertices.push_back(v);
  v.position.z = box.tr.z; vertices.push_back(v);
  v.position.y = box.tr.y; vertices.push_back(v);
  v.position.z = box.bl.z; vertices.push_back(v);
  indices.push_back(i); indices.push_back(i+1); indices.push_back(i+2);
  indices.push_back(i); indices.push_back(i+2); indices.push_back(i+3);
  i+=4;
  //right
  v.normal = glm::vec3(1.0,0.0,0.0);
  v.position = box.tr; vertices.push_back(v);
  v.position.y = box.bl.y; vertices.push_back(v);
  v.position.z = box.bl.z; vertices.push_back(v);
  v.position.y = box.tr.y; vertices.push_back(v);
  indices.push_back(i); indices.push_back(i+1); indices.push_back(i+2);
  indices.push_back(i); indices.push_back(i+2); indices.push_back(i+3);
  i+=4;
  //front
  v.normal = glm::vec3(0.0,0.0,-1.0);
  v.position = box.bl; vertices.push_back(v);
  v.position.y = box.tr.y; vertices.push_back(v);
  v.position.x = box.tr.x; vertices.push_back(v);
  v.position.y = box.bl.y; vertices.push_back(v);
  indices.push_back(i); indices.push_back(i+1); indices.push_back(i+2);
  indices.push_back(i); indices.push_back(i+2); indices.push_back(i+3);
  i+=4;
  //back
  v.normal = glm::vec3(0.0,0.0,1.0);
  v.position = box.tr; vertices.push_back(v);
  v.position.x = box.bl.x; vertices.push_back(v);
  v.position.y = box.bl.y; vertices.push_back(v);
  v.position.x = box.tr.x; vertices.push_back(v);
  indices.push_back(i); indices.push_back(i+1); indices.push_back(i+2);
  indices.push_back(i); indices.push_back(i+2); indices.push_back(i+3);
}

int main(int /*argc*/, char ** /*argv*/)
{
  BVH app;
  return app.run();
}
