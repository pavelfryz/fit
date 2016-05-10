#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

/*
	Povolené metody a funkce OpenCV:
		Mat: rows, cols, step(), size(), at<>(), zeros(), ones(), eye()
*/

/*  Funkce pro šedotónový obraz (0-255) spočítá konvoluci se zadaným jádrem o velikost 3x3 typu float (CV_32FC1).
    Krajní hodnoty výsledného obrazu ponechte 0.
	Implementujte ručně průchodem obrazem, pro každý pixel projděte jeho okolí a proveďte konvoluci s jádrem.

*/
void convolution( cv::Mat& gray, const cv::Mat& kernel, cv::Mat& dst )
{
	dst = cv::Mat::zeros( gray.size(), CV_32FC1 );

	if( kernel.rows != 3 || kernel.cols != 3 )
		return;

	float norm_coef = 1.f/(cv::sum(abs(kernel)).val[0] + 0.000000001);

	// krajní hodnoty obrazu necháme 0
	for( int j = 1; j < gray.rows-1; ++j ) {
		for( int i = 1; i < gray.cols-1; ++i ) {

			float sum = 0;

			/*
				projdi okolí 3x3 aktuálního bodu a
					1. spočítej konvoluci
					2. normalizuj a
					3. ulož do výstupního obrazu
			*/
			// 1.
			for(int y=-1; y <= 1; ++y)
			for(int x=-1; x <= 1; ++x) {
				sum += gray.at<uchar>(j+y,i+x)*kernel.at<float>(1-y,1-x);
			}
			// 2.
			sum *= norm_coef;
			// 3.
			dst.at<float>(j,i) = sum;
		}
	}

}

// stejná funkce jako cv::invertAffineTransform - zde ukázka implementace inverze
cv::Mat invertTransformationMat( const cv::Mat& T )
{
	double M[6];
	cv::Mat matM(2, 3, CV_64F, M);
	T.convertTo(matM, matM.type());

    double D = M[0]*M[4] - M[1]*M[3];
    D = D != 0 ? 1./D : 0;
    double A11 = M[4]*D, A22=M[0]*D;
    M[0] = A11; M[1] *= -D;
    M[3] *= -D; M[4] = A22;
    double b1 = -M[0]*M[2] - M[1]*M[5];
    double b2 = -M[3]*M[2] - M[4]*M[5];
    M[2] = b1; M[5] = b2;

    cv::Mat out;
	matM.convertTo(out,T.type());
	return out;
}

/*  Funkce provede geometrickou transformaci obrazu s využitím interpolace nejbližším sousedem.
*/
void geometricalTransform( const cv::Mat& src, cv::Mat& dst, const cv::Mat& transformation )
{
	cv::Mat T0, T;
	T0 = invertTransformationMat( transformation );
	//cv::invertAffineTransform(transformation, T0);
	T0.convertTo(T,CV_32FC1);

	dst = cv::Mat::zeros(src.size(), CV_8UC1 );
	
	/*
		Pro každý pixel výstupního obrazu
			1. najdi jeho polohu ve zdrojovém obrazu (pomocí připravené inverzní transformace)
				viz http://docs.opencv.org/modules/imgproc/doc/geometric_transformations.html?highlight=warpaffine#warpaffine
			2. zkontroluj, není-li souřadnice mimo zdrojový obraz
			3. využij interpolace pomocí nejbližšího souseda k výpoču výsledného jasu cílového pixelu
			4. zaokrouhli a ulož výslednou hodnotu do výstupního obrazu
	*/
	for(int y = 0; y < dst.rows; ++y)
	for(int x = 0; x < dst.cols; ++x) {
		int srcX = roundf(T.at<float>(0,0)*x + T.at<float>(0,1)*y + T.at<float>(0,2));
		int srcY = roundf(T.at<float>(1,0)*x + T.at<float>(1,1)*y + T.at<float>(1,2));
		if(srcX < 0 || srcY < 0 || srcX >= src.cols || srcY >= src.rows ) continue;
		dst.at<uchar>(y,x) = src.at<uchar>(srcY,srcX);
	}

	return;
}




//---------------------------------------------------------------------------
void checkDifferences( const cv::Mat test, const cv::Mat ref, std::string tag, bool save = false);
//---------------------------------------------------------------------------

//
// Examples of input parameters
//
// mt-02 image_path [rotation in degrees] [scale]
// e.g. ./mt-02 ../data/garden.png 30 1.2


int main(int argc, char* argv[])
{
    std::string img_path = "";
    float s = 1.f;
    float r = 0.f;

	// check input parameters
	if( argc > 1 ) img_path = std::string( argv[1] );
	if( argc > 2 ) r = atof( argv[2] );
	if( argc > 3 ) s = atof( argv[3] );


	// load testing images
	cv::Mat src_rgb = cv::imread( img_path );

	// check testing images
	if( src_rgb.empty() ) {
		std::cout << "Failed to load image: " << img_path << std::endl;
		return -1;
	}

	cv::Mat src_gray;
	cvtColor( src_rgb, src_gray, CV_BGR2GRAY );

	//---------------------------------------------------------------------------

	// konvoluce
	float ker[9] = { -1, -2, -1, 0, 0, 0, 1, 2, 1 };
	cv::Mat kernel( 3, 3, CV_32FC1, ker );
	cv::Mat conv_man, conv_ref;

	// naprogramované řešení
	convolution( src_gray, kernel, conv_man );

	// referenční řešení
	cv::flip( kernel, kernel, -1 );
	cv::filter2D( src_gray, conv_ref, CV_32F, kernel );
	conv_ref *= 1.f/(cv::sum(abs(kernel)).val[0] + 0.000000001);
	// jelikož filter2D funkce počítá i hodnoty na okrajích výstupního obrazu (a my pro jednoduchost ne)
	// před srovnáním vymažeme krajní hodnoty obrazu
	cv::rectangle( conv_ref, cv::Point(0,0), cv::Point(conv_ref.cols-1,conv_ref.rows-1), cv::Scalar::all(0), 1 );

	//---------------------------------------------------------------------------

	// geometrická transformace
	cv::Mat M = cv::getRotationMatrix2D( cv::Point2f(0.5f*src_gray.cols,0.5f*src_gray.rows), r, s );
	
	// výsledná transformace obrazu
	cv::Mat tran_man, tran_ref;
	geometricalTransform( src_gray, tran_man, M );
	cv::warpAffine( src_gray, tran_ref, M, src_gray.size(), cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar::all(0) );

	//---------------------------------------------------------------------------


	// vyhodnocení
	checkDifferences( conv_man, conv_ref, "02_convolution", true );
	checkDifferences( tran_man, tran_ref, "02_geometry", true );
	std::cout << std::endl;

    return 0;
}
//---------------------------------------------------------------------------




void checkDifferences( const cv::Mat test, const cv::Mat ref, std::string tag, bool save )
{
	double mav = 255., err = 255., nonzeros = 255.;

	if( !test.empty() ) {
		cv::Mat diff;
		cv::absdiff( test, ref, diff );
		cv::minMaxLoc( diff, NULL, &mav );
		err = ( cv::sum(diff).val[0] / (diff.rows*diff.cols) );
		nonzeros = 1. * cv::countNonZero( diff ) / (diff.rows*diff.cols);

		if( save ) {
			diff *= 255;
			cv::imwrite( (tag+".png").c_str(), test );
			cv::imwrite( (tag+"_err.png").c_str(), diff );
		}
	}

	printf( "%s %.2f %.2f %.2f ", tag.c_str(), err, nonzeros, mav );

	return;
}
