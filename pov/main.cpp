#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>

using namespace std; 
using namespace cv;

#define VISUAL 0
#define DRAW_FIR 1
#define DRAW_EDGE 1
#define DRAW_DIAG 1

#define FIPS_RATIO 0.3       
#define FIP_TO_EDGE_RATIO 10
#define EDGES_RATIO 0.27
#define EDGES_ANGLE 0.27
#define EDGE_FIP_ANGLE 0.33

#define ANGLE_HIST_SIZE 64
#define ANGLE_HIST_HALF ANGLE_HIST_SIZE/2
#define ANGLE_SEARCH_DIST 3


struct Fip {
  int id;
  int size;
  Point center;
  Rect box;
  Point2f dir1;
  Point2f dir2;
};

struct Edge {
  Fip f1;
  Fip f2;
  Point2f dir;
  double length;
};

int detectFips(Mat &img, std::vector<Fip> &fips);
int createEdgesFromFips(Mat &img, std::vector<Fip> &fips, std::vector<Edge> &edges);

int main( int argc, char* argv[])
{
  if(argc!=3){
    cerr << "Usage: ./qr input output" << endl;
    return -1;
  }
  
  Mat image = imread(argv[1]);

  if(image.empty()){
    cerr << "Error: Unable to open input image \"" << argv[1] << "\"." << endl;
    return -1;
  }
  
  std::vector<Fip> fips; 
  std::vector<Edge> edges;
  
  detectFips(image,fips);
  createEdgesFromFips(image,fips, edges);

  for(int i = 0; i < edges.size(); i++ ) {
    for(int j = i + 1; j < edges.size(); j++) {
      //zjisti jestli dve hrany maji spolecny finder pattern - bod
      if(edges[i].f1.id!=edges[j].f1.id && edges[i].f1.id!=edges[j].f2.id && 
         edges[i].f2.id!=edges[j].f2.id && edges[i].f2.id!=edges[j].f1.id) continue;
      
      double maxlength = max(edges[i].length,edges[j].length);
      if((abs(edges[i].length-edges[j].length)/maxlength) > EDGES_RATIO || //zkontroluje pomer stran a
          abs(edges[i].dir.dot(edges[j].dir)) > EDGES_ANGLE) continue;     //uhel ktery sviraji
      
      //najde spolecny bod C, a dva zbyvajici A a B
      Fip c = edges[i].f1, a, b;
      if(c.id==edges[j].f1.id){
        a=edges[i].f2;
        b=edges[j].f2;
      }else{
        c=edges[i].f2;
        a=edges[i].f1;
        b=(c.id==edges[j].f1.id)?edges[j].f2:edges[j].f1;
      }
      
      if(DRAW_DIAG) line(image,a.center,b.center,Scalar(0,0,255),8,8,0);
    }
  }
      
  if(VISUAL){
    namedWindow("Image",WINDOW_NORMAL);
    imshow ( "Image", image );
    waitKey();
  }
  
  imwrite(argv[2],image);
  return 0;
}

int detectFips(Mat &img,std::vector<Fip> &fips) 
{
  // Nacteni klasifikatoru
  CascadeClassifier cascade;
  if( !cascade.load("cascade.xml") ){ 
    cerr<<"Error: Unable to load classifier\n" << endl;
    return -1; 
  }
  
  Mat gray;
  cvtColor(img, gray,CV_BGR2GRAY);
  equalizeHist( gray, gray );
  
  // Detekovani find patternu(fip)
  std::vector<Rect> rect;
  cascade.detectMultiScale(gray, rect);
  
  fips.reserve(rect.size());
  for( int i = 0; i < rect.size(); i++ ) {
    Mat roi(gray, rect[i]);
    resize(roi,roi,Size(48,48));
    Canny( roi, roi, 50, 200, 3 );
    
    // Detekce primek pro urceni natoceni fip
    std::vector<Vec2f> lines;
    HoughLines(roi, lines, 1, CV_PI/360, 20);
    
    // Histogram uhlu detekovanych primek
    int angleHist[ANGLE_HIST_SIZE]={0};
    for(int i = 0; i < lines.size(); i++ )
      angleHist[min((int)(lines[i][1]*ANGLE_HIST_SIZE/CV_PI),ANGLE_HIST_SIZE-1)]++;
    
    // Najiti maxim v histogramu detekovanych primek
    int max1=0;
    for(int i=1;i<ANGLE_HIST_SIZE;++i)
      if(angleHist[i]>angleHist[max1]) max1=i;
    int max2 = (max1+ANGLE_HIST_HALF)%ANGLE_HIST_SIZE;
    for(int i=-ANGLE_SEARCH_DIST;i<ANGLE_SEARCH_DIST;++i)
      if(angleHist[(max1+ANGLE_HIST_HALF+i)%ANGLE_HIST_SIZE]>angleHist[max2]) max2=(max1+ANGLE_HIST_HALF+i)%ANGLE_HIST_SIZE;
    
    // Pridani fip do seznamu
    double dl;
    Fip fip;
    fip.id = i;
    fip.box = rect[i];
    fip.size = max(rect[i].width,rect[i].height);
    fip.center = Point( rect[i].x + rect[i].width*0.5, rect[i].y + rect[i].height*0.5 );
    fip.dir1 = Point2f(-sin((max1+0.5)*CV_PI/ANGLE_HIST_SIZE),cos((max1+0.5)*CV_PI/ANGLE_HIST_SIZE));
    fip.dir2 = Point2f(-sin((max2+0.5)*CV_PI/ANGLE_HIST_SIZE),cos((max2+0.5)*CV_PI/ANGLE_HIST_SIZE));
    dl = sqrt(fip.dir1.x*fip.dir1.x + fip.dir1.y*fip.dir1.y);
    fip.dir1.x /= dl;
    fip.dir1.y /= dl;
    dl = sqrt(fip.dir2.x*fip.dir2.x + fip.dir2.y*fip.dir2.y);
    fip.dir2.x /= dl;
    fip.dir2.y /= dl;
    
    fips.push_back(fip);
    
    if(DRAW_FIR) ellipse(img,fip.center,Size(rect[i].width/2,rect[i].height/2),0,0,360,Scalar(255,0,255),3,8,0);
  }
  
  return 0;
}

int createEdgesFromFips(Mat &img, std::vector<Fip> &fips, std::vector<Edge> &edges)
{
  Edge edge;
  for( size_t i = 0; i < fips.size(); i++ ) {
    for( size_t j = i + 1; j < fips.size(); j++) {
      edge.dir = fips[i].center - fips[j].center;
      edge.length = sqrt(edge.dir.x*edge.dir.x + edge.dir.y*edge.dir.y);
      edge.dir.x /= edge.length;
      edge.dir.y /= edge.length;
      
      int maxsize = max(fips[i].size,fips[j].size);                        // aby vznikla hrana tak se nesmi
      if((abs(fips[i].size-fips[j].size)/(double)maxsize) > FIPS_RATIO ||// prilis lisit velikost find paternu
          edge.length>FIP_TO_EDGE_RATIO*maxsize ||                       // hrana byt prilis dlouha vuci velikosti paternu
                                                                        // detekovany uhel na paternu prilis lisit od smeru hrany
          (abs(edge.dir.dot(fips[i].dir1)) > EDGE_FIP_ANGLE && abs(edge.dir.dot(fips[i].dir2)) > EDGE_FIP_ANGLE) ||              
          (abs(edge.dir.dot(fips[j].dir1)) > EDGE_FIP_ANGLE && abs(edge.dir.dot(fips[j].dir2)) > EDGE_FIP_ANGLE)) continue;
      edge.f1 = fips[i];
      edge.f2 = fips[j];
      
      // pridani hrany
      edges.push_back(edge);
      
      if(DRAW_EDGE) line(img,edge.f1.center,edge.f2.center,Scalar(0,255,255),3,8,0);
    }
  }
  return 0;
}
