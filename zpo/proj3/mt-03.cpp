#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


#define LOW_PASS_FILTER 1
#define HIGH_PASS_FILTER 2


/* Připravené podpůrné funkce  */
/* Funkce přeskládá kvadranty Fourierova obrazu tak, aby byl počátek uprostřed obrazu. */
void rearrangeSpectrum( cv::Mat& s );
/* Funkce vypočítá amplitudové spektrum - absolutní hodnoty z komplexních čísel v pixelech. */
cv::Mat spectrumMagnitude( cv::Mat & specCplx );
/* Vyhodnocení/porovnání výsledku s referenčním obrazem. */
void checkDifferences( const cv::Mat test, const cv::Mat ref, std::string tag, bool save = false);



/* Funkce k dopracování ... */
/*  Funkce pro filtraci obrazu - dolní, resp. horní propust. */
void passFilter( const cv::Mat& src, cv::Mat& dst, int limit_frequency, int flag, cv::Mat * spektrum = NULL )
{
	// najdeme optimální velikost obrazu pro efektivní výpočet DFT
	cv::Size dftSize;
	dftSize.width = cv::getOptimalDFTSize(src.cols);
	dftSize.height = cv::getOptimalDFTSize(src.rows);

	// připravíme nový obraz optimální velikosti - hodnoty mimo zdrojový obraz nastavíme na 0
	cv::Mat srcPadded;
	cv::copyMakeBorder( src, srcPadded, 0, dftSize.height-src.rows, 0, dftSize.width-src.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    
	// připravíme komplexní reprezentaci zdrojového obrazu
	cv::Mat srcCplx, planes[] = {cv::Mat_<float>(srcPadded), cv::Mat::zeros(srcPadded.size(), CV_32F)};
	cv::merge(planes, 2, srcCplx);         

	// využijeme funkci dft knihovny OpenCV pro výpočet spektra
	cv::Mat spectrumCplx;
	cv::dft(srcCplx, spectrumCplx, 0, src.rows );  

	// přeskládáme kvadranty spektra
	rearrangeSpectrum( spectrumCplx );

	cv::Mat mask;
	// připravíme masku na vymazání částí spektra (propust/zádrž)
	int lim_freq = MIN( limit_frequency, MIN(spectrumCplx.rows,spectrumCplx.cols ) );
	cv::Rect band( (spectrumCplx.cols-lim_freq)/2, (spectrumCplx.rows-lim_freq)/2, lim_freq, lim_freq );

	if( flag == HIGH_PASS_FILTER ) {
		// ponecháme pouze hodnoty spektra nad limitní frekvencí
		/* *** ZAČÁTEK VLASTNÍ IMPLEMENTACE 1 *** */
		for( int j = 0; j < spectrumCplx.rows; ++j ) {
			for( int i = 0; i < spectrumCplx.cols; ++i ) {
				if(j >= band.y && i >= band.x && j<band.y+band.height && i<band.x+band.width){
					spectrumCplx.at<double>(j,i) = 0.0;
				}
			}
		}
		/* *** KONEC VLASTNÍ IMPLEMENTACE 1 *** */
	}
	else if( flag == LOW_PASS_FILTER ) {
		// ponecháme pouze hodnoty spektra pod limitní frekvencí
		/* *** ZAČÁTEK VLASTNÍ IMPLEMENTACE 2 *** */
		for( int j = 0; j < spectrumCplx.rows; ++j ) {
			for( int i = 0; i < spectrumCplx.cols; ++i ) {
				if(!(j >= band.y && i >= band.x && j<band.y+band.height && i<band.x+band.width)){
					spectrumCplx.at<double>(j,i) = 0.0;
				}
			}
		}
		/* *** KONEC VLASTNÍ IMPLEMENTACE 2 *** */
	}

	// zkopírujeme spektrum pro pozdější ladící/hodnotící účet
	if( spektrum != NULL ) *spektrum = spectrumCplx.clone();
		
	// přeskládáme kvadranty spektra zpět
	rearrangeSpectrum( spectrumCplx );

	// využijeme funkci dft (nebo idft) knihovny OpenCV pro zpětnou transformaci
	cv::dft( spectrumCplx, srcCplx, cv::DFT_INVERSE+cv::DFT_SCALE, src.rows );

	cv::split(srcCplx, planes);  
	cv::normalize( planes[0], planes[0], 0, 1, CV_MINMAX );
	planes[0](cv::Rect(cv::Point(0,0),src.size())).convertTo( dst, src.type(), 255, 0 );

	return;
}





//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//
// Examples of input parameters
//
// mt-03 image_path [limit frequency] [reference image]
// e.g. ./mt-03 ../data/garden.png 20


int main(int argc, char* argv[])
{
    std::string img_path = "";
    std::string ref_path = "";
    int limit_frequency = 100;

	// check input parameters
	if( argc > 1 ) img_path = std::string( argv[1] );
	if( argc > 2 ) limit_frequency = atoi( argv[2] );
	if( argc > 3 ) ref_path = std::string( argv[3] );

	// load testing images
	cv::Mat src_rgb = cv::imread( img_path );

	// check testing images
	if( src_rgb.empty() ) {
		std::cout << "Failed to load image: " << img_path << std::endl;
		return -1;
	}

	cv::Mat src_gray;
	cv::cvtColor( src_rgb, src_gray, CV_BGR2GRAY );

	//---------------------------------------------------------------------------

	cv::Mat low_pass, high_pass;
	cv::Mat low_spec_cplx, high_spec_cplx;
	cv::Mat low_spec, high_spec;
	passFilter( src_gray, low_pass,  limit_frequency, LOW_PASS_FILTER,  &low_spec_cplx );
	passFilter( src_gray, high_pass, limit_frequency, HIGH_PASS_FILTER, &high_spec_cplx );
	spectrumMagnitude(low_spec_cplx).convertTo(low_spec, CV_8UC1, 255 );
	spectrumMagnitude(high_spec_cplx).convertTo(high_spec, CV_8UC1, 255 ) ;

	cv::Mat low_pass_ref, high_pass_ref;
	cv::Mat low_spec_ref, high_spec_ref;
	low_pass_ref  = cv::imread( ref_path+"low_pass_ref.png",  cv::IMREAD_GRAYSCALE );
	high_pass_ref = cv::imread( ref_path+"high_pass_ref.png", cv::IMREAD_GRAYSCALE );
	low_spec_ref  = cv::imread( ref_path+"low_spec_ref.png",  cv::IMREAD_GRAYSCALE );
	high_spec_ref = cv::imread( ref_path+"high_spec_ref.png", cv::IMREAD_GRAYSCALE );
	if( low_pass_ref.empty() || low_spec_ref.empty() || high_pass_ref.empty() || high_spec_ref.empty() ) 
		std::cout << "WARNING: references data failed to load." << std::endl;
	
	// vyhodnocení
	checkDifferences( low_pass, low_pass_ref,   "03_low_pass", true );
	checkDifferences( low_spec, low_spec_ref,   "03_low_spec", true );
	checkDifferences( high_pass, high_pass_ref, "03_high_pass", true );
	checkDifferences( high_spec, high_spec_ref, "03_high_spec", true );
	std::cout << std::endl;

    return 0;
}
//---------------------------------------------------------------------------




void checkDifferences( const cv::Mat test, const cv::Mat ref, std::string tag, bool save )
{
	double mav = 255., err = 255., nonzeros = 255.;
	cv::Mat diff;

	if( !test.empty() && !ref.empty() ) {
		cv::absdiff( test, ref, diff );
		cv::minMaxLoc( diff, NULL, &mav );
		err = ( cv::sum(diff).val[0] / (diff.rows*diff.cols) );
		nonzeros = 1. * cv::countNonZero( diff ) / (diff.rows*diff.cols);
	}
	
	if( save ) {
		if( !test.empty() ) { cv::imwrite( (tag+".png").c_str(), test ); }
		if( !diff.empty() ) { diff *= 255;	cv::imwrite( (tag+"_err.png").c_str(), diff ); }
	}

	printf( "%s %.2f %.2f %.2f ", tag.c_str(), err, nonzeros, mav );

	return;
}



void rearrangeSpectrum( cv::Mat& s )
{
    int cx = s.cols/2;
    int cy = s.rows/2;

    cv::Mat q0(s, cv::Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    cv::Mat q1(s, cv::Rect(cx, 0, cx, cy));  // Top-Right
    cv::Mat q2(s, cv::Rect(0, cy, cx, cy));  // Bottom-Left
    cv::Mat q3(s, cv::Rect(cx, cy, cx, cy)); // Bottom-Right

    cv::Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);
}

cv::Mat spectrumMagnitude( cv::Mat & specCplx )
{
	cv::Mat specMag, planes[2];
	cv::split(specCplx, planes);						
	cv::magnitude(planes[0], planes[1], planes[0]);		
	cv::log( (planes[0] + cv::Scalar::all(1)), specMag );
	cv::normalize( specMag, specMag, 0, 1, CV_MINMAX );
	return specMag;
}

