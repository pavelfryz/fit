#include "BaseApp.h"

#include <fstream>
#include <iterator>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif //WIN32

using namespace std;

/**
 * Vrati retezec pro opencl error kod
 */
const char *CLErrorString(cl_int _err) {
    switch (_err) {
    case CL_SUCCESS:
        return "Success!";
    case CL_DEVICE_NOT_FOUND:
        return "Device not found.";
    case CL_DEVICE_NOT_AVAILABLE:
        return "Device not available";
    case CL_COMPILER_NOT_AVAILABLE:
        return "Compiler not available";
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        return "Memory object allocation failure";
    case CL_OUT_OF_RESOURCES:
        return "Out of resources";
    case CL_OUT_OF_HOST_MEMORY:
        return "Out of host memory";
    case CL_PROFILING_INFO_NOT_AVAILABLE:
        return "Profiling information not available";
    case CL_MEM_COPY_OVERLAP:
        return "Memory copy overlap";
    case CL_IMAGE_FORMAT_MISMATCH:
        return "Image format mismatch";
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:
        return "Image format not supported";
    case CL_BUILD_PROGRAM_FAILURE:
        return "Program build failure";
    case CL_MAP_FAILURE:
        return "Map failure";
    case CL_INVALID_VALUE:
        return "Invalid value";
    case CL_INVALID_DEVICE_TYPE:
        return "Invalid device type";
    case CL_INVALID_PLATFORM:
        return "Invalid platform";
    case CL_INVALID_DEVICE:
        return "Invalid device";
    case CL_INVALID_CONTEXT:
        return "Invalid context";
    case CL_INVALID_QUEUE_PROPERTIES:
        return "Invalid queue properties";
    case CL_INVALID_COMMAND_QUEUE:
        return "Invalid command queue";
    case CL_INVALID_HOST_PTR:
        return "Invalid host pointer";
    case CL_INVALID_MEM_OBJECT:
        return "Invalid memory object";
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
        return "Invalid image format descriptor";
    case CL_INVALID_IMAGE_SIZE:
        return "Invalid image size";
    case CL_INVALID_SAMPLER:
        return "Invalid sampler";
    case CL_INVALID_BINARY:
        return "Invalid binary";
    case CL_INVALID_BUILD_OPTIONS:
        return "Invalid build options";
    case CL_INVALID_PROGRAM:
        return "Invalid program";
    case CL_INVALID_PROGRAM_EXECUTABLE:
        return "Invalid program executable";
    case CL_INVALID_KERNEL_NAME:
        return "Invalid kernel name";
    case CL_INVALID_KERNEL_DEFINITION:
        return "Invalid kernel definition";
    case CL_INVALID_KERNEL:
        return "Invalid kernel";
    case CL_INVALID_ARG_INDEX:
        return "Invalid argument index";
    case CL_INVALID_ARG_VALUE:
        return "Invalid argument value";
    case CL_INVALID_ARG_SIZE:
        return "Invalid argument size";
    case CL_INVALID_KERNEL_ARGS:
        return "Invalid kernel arguments";
    case CL_INVALID_WORK_DIMENSION:
        return "Invalid work dimension";
    case CL_INVALID_WORK_GROUP_SIZE:
        return "Invalid work group size";
    case CL_INVALID_WORK_ITEM_SIZE:
        return "Invalid work item size";
    case CL_INVALID_GLOBAL_OFFSET:
        return "Invalid global offset";
    case CL_INVALID_EVENT_WAIT_LIST:
        return "Invalid event wait list";
    case CL_INVALID_EVENT:
        return "Invalid event";
    case CL_INVALID_OPERATION:
        return "Invalid operation";
    case CL_INVALID_GL_OBJECT:
        return "Invalid OpenGL object";
    case CL_INVALID_BUFFER_SIZE:
        return "Invalid buffer size";
    case CL_INVALID_MIP_LEVEL:
        return "Invalid mip-map level";
    default:
        return "Unknown";
    }
}

BaseApp::BaseApp(){
  width = 1024;
  height = 768;
  resizable = true;
  fullscreen = false;  
  multisample = 1;
}

BaseApp::~BaseApp(){
}

int BaseApp::run(){
  try {
    initGL();
    init();

    SDL_Event e;
    bool quit = false;
    while (!quit) {
    
      while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
          quit = true;
          break;
        case SDL_KEYDOWN:
          onKeyPress(e.key.keysym.sym, e.key.keysym.mod);
          break;
        case SDL_KEYUP:
          onKeyRelease(e.key.keysym.sym, e.key.keysym.mod);
          break;
        case SDL_MOUSEMOTION:
          onMouseMove(e.motion.xrel,e.motion.yrel,e.motion.x,e.motion.y);
          break;
        case SDL_MOUSEBUTTONDOWN:
          onMousePress(e.button.button, e.button.x, e.button.y);
          break;
        case SDL_MOUSEBUTTONUP:
          onMouseRelease(e.button.button,e.button.x,e.button.y);
          break;
        case SDL_MOUSEWHEEL:
          onMouseWheel(e.wheel.y);
          break;

        case SDL_WINDOWEVENT:
          switch (e.window.event) {
          case SDL_WINDOWEVENT_RESIZED:
            width = e.window.data1;
            height = e.window.data2;
            onResize(width, height);
            break;
          
          }
          break;        
        }
      }


      draw();
      SDL_GL_SwapWindow(window);
    }



  }
  catch (string s) {
    cerr << s;
    return -1;
  }
  catch (const exception &e) {
    cerr << "Fatal Exception " << e.what() << endl;
    return -1;
  }

  return 0;
}

void BaseApp::quit(){
  SDL_Event event;
  event.type = SDL_QUIT;
  SDL_PushEvent(&event);
}

void BaseApp::texImageFileBMP(GLuint target, GLuint level, GLuint internalFormat, std::string file){
  SDL_Surface* surface = SDL_LoadBMP(file.c_str());
  if (!surface) throw "Failed to load texture " + file + " error:" + SDL_GetError() + "\n";
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  if ((surface->format->Rmask == 0xff0000) &&
    (surface->format->Gmask == 0xff00) &&
    (surface->format->Bmask == 0xff) &&
    (surface->format->Amask == 0))
  {
    glTexImage2D(target, level, internalFormat, surface->w, surface->h, 0, GL_BGR, GL_UNSIGNED_BYTE, surface->pixels);
  }
  else if ((surface->format->Rmask == 0xff) &&
    (surface->format->Gmask == 0xff00) &&
    (surface->format->Bmask == 0xff0000) &&
    (surface->format->Amask == 0))
  {
    glTexImage2D(target, level, internalFormat, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
  }
}

GLuint BaseApp::textureFromBMP(std::string file){
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  texImageFileBMP(GL_TEXTURE_2D,0,GL_RGB,file);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  return tex;
}

string BaseApp::loadFile(const string file) {
  return loadFile(file.c_str());
}
string BaseApp::loadFile(const char * const file)
{
  ifstream stream(file);
  if (stream.fail()) throw runtime_error(string("Can't open \'") + file + "\'");
  return string(istream_iterator<char>(stream >> noskipws), istream_iterator<char>());
}

string BaseApp::getShaderInfo(GLuint shader) {
  int length = 0;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
  if (length <= 1) return "";
  string log(length, ' ');
  glGetShaderInfoLog(shader, length, NULL, &log[0]);
  return log;
}

string BaseApp::getProgramInfo(GLuint shader) {
  int length = 0;
  glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &length);
  if (length <= 1) return "";
  string log(length, ' ');
  glGetProgramInfoLog(shader, length, NULL, &log[0]);
  return log;
}

GLuint BaseApp::compileShader(const GLenum type, string src){
  GLuint shader = glCreateShader(type);
  const char * srcPtr = src.c_str();
  glShaderSource(shader, 1, &srcPtr, NULL);
  glCompileShader(shader);
  /*
  string log = getShaderInfo(shader);
  if (!log.empty()) cout<< "Shader compile log:"<<endl << log;
  */
  return shader;
}

GLuint BaseApp::linkShader(unsigned int count, ...){
  GLuint program = glCreateProgram();

  // Attach shaders
  va_list args;
  va_start(args, count);
  for (size_t i = 0; i < count; ++i)
  {
    glAttachShader(program, va_arg(args, GLuint));
  }
  va_end(args);

  // Link program and check for errors
  glLinkProgram(program);
  string log = getProgramInfo(program);
  if (!log.empty()) cout << "Program link log:" << endl << log;

  int linkStatus;
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
  if (linkStatus == GL_FALSE) throw std::runtime_error("Shader linking failed");

  return program;
}

void BaseApp::CheckOpenCLError(cl_int _ciErr, const char *_sMsg, ...)
{
  unsigned int uiDebug = 1;
  char buffer[1024];

  va_list arg;
  va_start (arg, _sMsg);
  vsprintf (buffer, _sMsg, arg);
  va_end (arg);

  if(_ciErr!=CL_SUCCESS && _ciErr != CL_DEVICE_NOT_FOUND) {
    std::cout<<GetTime()<<": ERROR: "<<buffer<<": ("<<_ciErr<<")"<<CLErrorString(_ciErr)<<std::endl;
    throw std::runtime_error("OpenCL failed");
  } else {
    if(uiDebug>1) {
      std::cout<<GetTime()<<":    OK: "<<buffer<<std::endl;
    }
  }
}

double BaseApp::GetTime(void)
{
#if _WIN32                                  /* toto jede na Windows */
  static int initialized = 0;
  static LARGE_INTEGER frequency;
  LARGE_INTEGER value;

  if (!initialized) {                     /* prvni volani */
    initialized = 1;
    if (QueryPerformanceFrequency(&frequency) == 0) {                   /* pokud hi-res pocitadlo neni podporovano */
      //assert(0 && "HiRes timer is not available.");
      throw std::runtime_error("HiRes timer is not available");
    }
  }

  //assert(QueryPerformanceCounter(&value) != 0 && "This should never happen.");  /* osetreni chyby */
  QueryPerformanceCounter(&value);
  return (double)value.QuadPart / (double)frequency.QuadPart;   /* vrat hodnotu v sekundach */

#else                                           /* toto jede na Linux/Unixovych systemech */
  struct timeval tv;
  if (gettimeofday(&tv, NULL) == -1) {          /* vezmi cas */
    //assert(0 && "gettimeofday does not work.");   /* osetri chyby */
    throw std::runtime_error("Gettimeofday does not work");
  }
  return (double)tv.tv_sec + (double)tv.tv_usec/1000000.;   /* vrat cas v sekundach */
#endif
}

#ifdef __linux__
#define __stdcall
#endif

void __stdcall debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, void * userParam)
{
  cout << message << endl;
}


void BaseApp::initGL() {
  /* SDL */
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    string er = string("SDL_INIT_ERROR") + SDL_GetError();
    throw er;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

  if (multisample > 1){
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, multisample);
  }
  /* Window */
  SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
  window = SDL_CreateWindow("OpenGL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width,height,
    SDL_WINDOW_OPENGL
    | (resizable?SDL_WINDOW_RESIZABLE:0)
    | (fullscreen?SDL_WINDOW_FULLSCREEN_DESKTOP:0));
  SDL_GetWindowSize(window, &width, &height);
  

  if (window == NULL) {
    string er = string("SDL_CreateWindow") + SDL_GetError();
    throw er;
  }
  atexit(SDL_Quit);
  
  /* Context */
  context = SDL_GL_CreateContext(window);
  if (SDL_GL_SetSwapInterval(-1) == -1) {
    SDL_GL_SetSwapInterval(1);
  }

  /* Glew */
  glewExperimental = GL_TRUE;
  GLenum error = glewInit();
  if (error != GLEW_OK) {
    string er = string((char*)glewGetErrorString(error));
    throw er;
  }

  /* Debug logger*/

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
  glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_PERFORMANCE, GL_DONT_CARE, 0, NULL, GL_FALSE);
  glDebugMessageCallback((GLDEBUGPROC)debugMessageCallback, NULL);  

}

