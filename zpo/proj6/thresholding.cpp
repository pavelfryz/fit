#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#define MEAN 1
#define MEDIAN 2
#define MIDGRAY 3
#define BERNSEN 4
#define NIBLACK 5

void meanWithDeviation( const cv::Mat& src, cv::Mat& dst, cv::Mat& std_dev, int velikost )
{
  // velikost - musí být liché číslo, minimálně 3
  int stred = velikost/2;
  stred = MAX(1,stred);
  velikost = 2*stred+1;

  // zvětšíme obraz a okopírujeme krajní hodnoty do okrajů
  cv::Mat srcBorder;
  copyMakeBorder( src, srcBorder, stred, stred, stred, stred, cv::BORDER_REPLICATE );

  // připravíme výstupní obraz
  dst = cv::Mat( src.size(), src.type() );
  std_dev = cv::Mat( src.size(), src.type() );

  for( int j = 0; j < dst.rows; ++j ) {
    for( int i = 0; i < dst.cols; ++i ) 
    {
      float sum = 0;
      
      for(int y=0;y<velikost;++y) 
      for(int x=0;x<velikost;++x){
          sum += srcBorder.at<float>(y+j,x+i);
      }
      
      // výslednou hodnotu průměru uložte do výstupního obrazu
      dst.at<float>(j,i) = sum/(velikost*velikost);
      
      sum = 0;
      
      for(int y=0;y<velikost;++y) 
      for(int x=0;x<velikost;++x){
          float diff = srcBorder.at<float>(y+j,x+i) - dst.at<float>(j,i);
          sum += diff*diff;
      }
      
      // výslednou hodnotu směrodatné odchylky uložte do výstupního obrazu
      std_dev.at<float>(j,i) = sqrt(sum/(velikost*velikost));
    }
  }

  return;
}

void mean( const cv::Mat& src, cv::Mat& dst, int velikost )
{
  // velikost - musí být liché číslo, minimálně 3
  int stred = velikost/2;
  stred = MAX(1,stred);
  velikost = 2*stred+1;

  // zvětšíme obraz a okopírujeme krajní hodnoty do okrajů
  cv::Mat srcBorder;
  copyMakeBorder( src, srcBorder, stred, stred, stred, stred, cv::BORDER_REPLICATE );

  // připravíme výstupní obraz
  dst = cv::Mat( src.size(), src.type() );

  for( int j = 0; j < dst.rows; ++j ) {
    for( int i = 0; i < dst.cols; ++i ) 
    {
      int sum = 0;
      
      for(int y=0;y<velikost;++y) 
      for(int x=0;x<velikost;++x){
          sum += srcBorder.at<uchar>(y+j,x+i);
      }
      
      // výslednou hodnotu uložte do výstupního obrazu
      dst.at<uchar>(j,i) = sum/(velikost*velikost);
    }
  }

  return;
}

void minmax( const cv::Mat& src, cv::Mat& min, cv::Mat& max, int velikost )
{
  // velikost - musí být liché číslo, minimálně 3
  int stred = velikost/2;
  stred = MAX(1,stred);
  velikost = 2*stred+1;

  // zvětšíme obraz a okopírujeme krajní hodnoty do okrajů
  cv::Mat srcBorder;
  copyMakeBorder( src, srcBorder, stred, stred, stred, stred, cv::BORDER_REPLICATE );

  // připravíme výstupní obraz
  min = cv::Mat( src.size(), src.type() );
  max = cv::Mat( src.size(), src.type() );

  // připravíme vektor k řazení prvků
  std::vector<uchar> buff(velikost*velikost);
  for( int j = 0; j < src.rows; ++j ) {
    for( int i = 0; i < src.cols; ++i ) 
    {
      // připravíme vektor tak, aby jsme k němu mohli přistupovat jako k matici hodnot
      cv::Mat m(buff);

      // do matice nakopírujte hodnoty z okolí pro následné seřazení
      for(int y=0;y<velikost;++y) 
      for(int x=0;x<velikost;++x){
          buff[y*velikost+x] = srcBorder.at<uchar>(y+j,x+i);
      }
      
      // seřadíme hodnoty ve vektoru
      std::sort(buff.begin(), buff.end());

      // výslednou hodnotu uložte do výstupního obrazu
      min.at<uchar>(j,i) = buff[0];
      max.at<uchar>(j,i) = buff[velikost*velikost-1];
    }
  }

  return;
}

void median( const cv::Mat& src, cv::Mat& dst, int velikost )
{
  // velikost - musí být liché číslo, minimálně 3
  int stred = velikost/2;
  stred = MAX(1,stred);
  velikost = 2*stred+1;

  // zvětšíme obraz a okopírujeme krajní hodnoty do okrajů
  cv::Mat srcBorder;
  copyMakeBorder( src, srcBorder, stred, stred, stred, stred, cv::BORDER_REPLICATE );

  // připravíme výstupní obraz
  dst = cv::Mat( src.size(), src.type() );

  // připravíme vektor k řazení prvků
  std::vector<uchar> buff(velikost*velikost);
  for( int j = 0; j < dst.rows; ++j ) {
    for( int i = 0; i < dst.cols; ++i ) 
    {
      // připravíme vektor tak, aby jsme k němu mohli přistupovat jako k matici hodnot
      cv::Mat m(buff);

      // do matice nakopírujte hodnoty z okolí pro následné seřazení
      for(int y=0;y<velikost;++y) 
      for(int x=0;x<velikost;++x){
          buff[y*velikost+x] = srcBorder.at<uchar>(y+j,x+i);
      }
      
      // seřadíme hodnoty ve vektoru
      std::sort(buff.begin(), buff.end());

      // výslednou hodnotu uložte do výstupního obrazu
      dst.at<uchar>(j,i) = buff[stred*velikost+stred];
    }
  }

  return;
}

void medianThreshold( const cv::Mat& src, cv::Mat& dst, int windowSize, int c)
{
  cv::Mat med;
#ifdef USE_OPENCV
  cv::medianBlur(src, med, windowSize);
#else
  median(src,med,windowSize);
#endif  
  
  dst = cv::Mat( src.size(), src.type() );
  
  for( int j = 0; j < dst.rows; ++j ) {
    for( int i = 0; i < dst.cols; ++i ) 
    {
      //pixel = ( pixel > median - c ) ? object : background
      dst.at<uchar>(j,i) = ( src.at<uchar>(j,i) > med.at<uchar>(j,i) - c ) ? 255 : 0;
    }
  }
  return;
}

void midGrayThreshold( const cv::Mat& src, cv::Mat& dst, int windowSize ,int c)
{
  cv::Mat min, max;
#ifdef USE_OPENCV
  cv::Mat kernel(cv::Size(windowSize, windowSize),CV_8UC1,cv::Scalar(1));
  cv::erode(src, min, kernel);
  cv::dilate(src, max, kernel);
#else
  minmax(src,min,max,windowSize);
#endif
  
  dst = cv::Mat( src.size(), src.type() );
  
  for( int j = 0; j < dst.rows; ++j ) {
    for( int i = 0; i < dst.cols; ++i ) 
    {
      //pixel = ( pixel > ( ( max + min ) / 2 ) - c ) ? object : background
      dst.at<uchar>(j,i) = ( src.at<uchar>(j,i) > ( ( max.at<uchar>(j,i) + min.at<uchar>(j,i) ) / 2 ) - c ) ? 255 : 0;
    }
  }
  return;
}

void meanThreshold( const cv::Mat& src, cv::Mat& dst, int windowSize ,int c)
{
  cv::Mat meanMat;
#ifdef USE_OPENCV
  cv::blur(src, meanMat, cv::Size(windowSize, windowSize));
#else
  mean(src,meanMat,windowSize);
#endif
  
  dst = cv::Mat( src.size(), src.type() );
  
  for( int j = 0; j < dst.rows; ++j ) {
    for( int i = 0; i < dst.cols; ++i ) 
    {
      //pixel = ( pixel > mean - c ) ? object : background
      dst.at<uchar>(j,i) = ( src.at<uchar>(j,i) > meanMat.at<uchar>(j,i) - c ) ? 255 : 0;
    }
  }
  return;
}

void niblackThreshold( const cv::Mat& src, cv::Mat& dst, int windowSize ,int c, float k)
{
  cv::Mat meanMat, std_dev, srcf;
  src.convertTo( srcf, CV_32FC1 );
#ifdef USE_OPENCV  
  cv::Mat mean2;
  cv::Size window(windowSize, windowSize);
  cv::blur(srcf, meanMat, window);
  cv::blur(srcf.mul(srcf), mean2, window);
  cv::sqrt(mean2 - meanMat.mul(meanMat), std_dev);
#else
  meanWithDeviation(srcf,meanMat,std_dev,windowSize);
#endif

  dst = cv::Mat( src.size(), src.type() );

  for( int j = 0; j < dst.rows; ++j ) {
    for( int i = 0; i < dst.cols; ++i ) 
    {
      //pixel = ( pixel >  mean + k * standard_deviation - c) ? object : background
      dst.at<uchar>(j,i) = ( srcf.at<float>(j,i) > meanMat.at<float>(j,i) + k * std_dev.at<float>(j,i) - c ) ? 255 : 0;
    }
  }
  return;
}

void bernsenThreshold( const cv::Mat& src, cv::Mat& dst, int windowSize ,int contrast_threshold)
{
  cv::Mat min, max;
#ifdef USE_OPENCV
  cv::Mat kernel(cv::Size(windowSize, windowSize),CV_8UC1,cv::Scalar(1));
  cv::erode(src, min, kernel);
  cv::dilate(src, max, kernel);
#else
  minmax(src,min,max,windowSize);
#endif

  dst = cv::Mat( src.size(), src.type() );
  
  for( int j = 0; j < dst.rows; ++j ) {
    for( int i = 0; i < dst.cols; ++i ) 
    {
      //if ( local_contrast < contrast_threshold )
      //  pixel = ( ( max + min ) / 2 >= 128 ) ? object :  background
      //else
      //  pixel = ( pixel >= mid_gray ) ? object : background
      if(max.at<uchar>(j,i) - min.at<uchar>(j,i) < contrast_threshold)
        dst.at<uchar>(j,i) = ( ( max.at<uchar>(j,i) + min.at<uchar>(j,i) ) / 2 >=128 ) ? 255 : 0;
      else
        dst.at<uchar>(j,i) = ( src.at<uchar>(j,i) > ( ( max.at<uchar>(j,i) + min.at<uchar>(j,i) ) / 2 ) ) ? 255 : 0;
    }
  }
  return;
}

void printUsage()
{
  std::cout << 
#ifdef USE_OPENCV  
  "./thresholding-cv input-image output-image method [radius] [param1] [param2]\n"
#else
  "./thresholding input-image output-image method [radius] [param1] [param2]\n"
#endif
  "METHOD:\n"
  "  1:mean\n"
  "  2:median\n"
  "  3:midgray\n"
  "  4:bernsen\n"
  "  5:niblack\n"
  "EXAMPLE:\n"
#ifdef USE_OPENCV  
  "  ./thresholding-cv input-image output-image 2 3\n"
#else
  "  ./thresholding input-image output-image 2 3\n"
#endif
  "  thresholding with median threshold and window rectangle 7x7(2*radius+1)"
  << std::endl;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//
// Examples of input parameters
// mt-05 input_image [noise_amount] [filter_size]
// e.g. ./mt-05 ../data/garden.png 0.05 7

int main(int argc, char* argv[])
{
  std::string input;
  std::string output;
  
  int method;
  int window = 17;
  int param1 = 0;
  float param2 = -0.2;

  // check input parameters
  if( argc <=3 ){ printUsage(); return 1; }
  input = std::string( argv[1] );
  output = std::string( argv[2] );
  method = atoi( argv[3] );
  if(method<1 || 5<method ) { 
    printUsage(); 
    return 1; 
  }
  if( argc > 4 ) window = 2*atoi( argv[4] )+1;
  if( argc > 5 ) param1 = atoi( argv[5] );
  if( argc > 6 ) param2 = atof( argv[6] );
  
  // load testing images
  cv::Mat src_rgb = cv::imread( input );

  // check testing images
  if( src_rgb.empty() ) {
    std::cout << "Failed to load image: " << input << std::endl;
    return -1;
  }

  cv::Mat src_gray;
  cv::cvtColor( src_rgb, src_gray, CV_BGR2GRAY );

  //---------------------------------------------------------------------------

  cv::Mat out;
  if(method == MEAN){
    meanThreshold(src_gray, out, window, param1);
  }else if(method == MEDIAN){
    medianThreshold(src_gray, out, window, param1);
  }else if(method == MIDGRAY){
    midGrayThreshold(src_gray, out, window, param1);
  }else if(method == BERNSEN){
    bernsenThreshold(src_gray, out, window, param1);
  }else if(method == NIBLACK){
    niblackThreshold(src_gray, out, window, param1, param2);
  }
  
  cv::imwrite((output+".png").c_str(), out );
  return 0;
}
//---------------------------------------------------------------------------
