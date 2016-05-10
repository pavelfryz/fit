#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>




//  Funkce pro generování bílého šumu.
//  Intenzita udává maximální velikost šumu.
void bilySum( const cv::Mat& src, cv::Mat& dst, float intenzita )
{
	cv::RNG rng;
	cv::Mat sum = cv::Mat::zeros( src.size(), CV_32SC1 );
	for( int i = 0; i < sum.rows*sum.cols; ++i ) {
		// vygenerujte hodnoty pro bílý šum a uložte do obrazu se šumem
		// využijte: rng.gaussian(double sigma)
		// a vztah pro gaussův pásmový bílý šum
		// vzorky bílého šumu s výkonem (rozptylem) např. 20 vypočteme:
		// v = sqrt(výkon) * gaussian(sigma=1.0)
		// funkce gaussian() generuje náhodné vzorky s normálním rozlošením se střední hodnotou 0 a rozptylem 1.
		//sum.at<int>(i) = .......

		/* *** ZAČÁTEK VLASTNÍ IMPLEMENTACE 1 *** */
		sum.at<int>(i) = sqrt(intenzita)*rng.gaussian(1.0);
		/* *** KONEC VLASTNÍ IMPLEMENTACE 1 *** */

	}
	cv::Mat tmp;
	src.convertTo( tmp, sum.type() );
	tmp = tmp+sum;
	tmp.convertTo( dst, src.type() );
}




//  Funkce pro filtraci obrazu Gaussovým filtrem.
//     - velikost filtru v pixelech
//	   - sigma je rozptyl pro výpočet hodnot Gaussovy funkce
void separabilityGauss( const cv::Mat& src, int velikost, double sigma, cv::Mat& sepDst, cv::Mat& noSepDst, int &noSepCnt, int &sepCnt )
{
	// velikost - musí být liché číslo, minimálně 3
	int stred = velikost/2;
	stred = MAX(1,stred);
	velikost = 2*stred+1;

	// připravte Gaussův filtr v 1D 
	cv::Mat gauss1D = cv::Mat::zeros( 1, velikost, CV_64FC1 );


	// zde implementujte výpočet koeficientù Gaussova filtru - ručně, dosazením do Gaussovy funkce
	/* *** ZAČÁTEK VLASTNÍ IMPLEMENTACE 2 *** */
	double b = 1/(2*sigma*sigma);
	for( int i = 0; i < velikost; ++i ) {
		double hodnota = exp( -b * (i-stred)*(i-stred) );
		//hodnota = a*exp( -b* (i-center)*(i-center) );
		gauss1D.at<double>(i) = hodnota;
	}
	/* *** KONEC VLASTNÍ IMPLEMENTACE 2 *** */


	// normalizace hodnot
	gauss1D = gauss1D / sum(gauss1D).val[0];

	// připravíme Gaussův filtr ve 2D 
	// využijeme konvoluce 1D Gauss. jádra ve směru x a y s jednotkovým impulsem
	// nastudujte parametry funkce filter2D - http://docs.opencv.org/modules/imgproc/doc/filtering.html#filter2d
	cv::Mat gauss2D = cv::Mat::zeros( velikost, velikost, CV_64FC1 );
	gauss2D.at<double>(stred,stred) = 1.;
	filter2D( gauss2D, gauss2D, -1, gauss1D );
	filter2D( gauss2D, gauss2D, -1, gauss1D.t() );
	gauss2D = gauss2D / sum(gauss2D).val[0];

	// rozmazání obrazu s využitím separability operátoru - využít 1D filtr
	/* *** ZAČÁTEK VLASTNÍ IMPLEMENTACE 3 *** */
	filter2D( src, sepDst, -1, gauss1D );
	filter2D( sepDst, sepDst, -1, gauss1D.t() );
	/* *** KONEC VLASTNÍ IMPLEMENTACE 3 *** */


	// rozmazání obrazu bez využití separability operátoru - využít 2D filtr
	/* *** ZAČÁTEK VLASTNÍ IMPLEMENTACE 4 *** */
	filter2D( src, noSepDst, -1, gauss2D );
	/* *** KONEC VLASTNÍ IMPLEMENTACE 4 *** */

	// ručně spočtěte a nechte vypsat na výstup - počet operací pro verzi s/bez využití separability 
	// stačí zjednodušený výpočet - počet operací násobení - např. src.rows*src.cols, apod.

	/* *** ZAČÁTEK VLASTNÍ IMPLEMENTACE 5 *** */
	sepCnt   = src.rows*src.cols*(velikost+velikost);	
	noSepCnt = src.rows*src.cols*velikost*velikost;	
	/* *** KONEC VLASTNÍ IMPLEMENTACE 5 *** */
	
	return;
}



/* Vyhodnocení/porovnání výsledku s referenčním obrazem. */
void checkDifferences( const cv::Mat test, const cv::Mat ref, std::string tag, bool save = false);


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//
// Examples of input parameters
// mt-04 input_image [noise_aplitude] [filter_size] [sigma]
// e.g. ./mt-04 ../../data/garden.png 0.2 31 2.0

int main(int argc, char* argv[])
{
    std::string img_path = "";
	float noise_amp = 0.1;
    int filter_size = 45;
	float sigma = 8.0;

	// check input parameters
	if( argc > 1 ) img_path = std::string( argv[1] );
	if( argc > 2 ) noise_amp = atof( argv[2] );
	if( argc > 3 ) filter_size = atoi( argv[3] );
	if( argc > 4 ) sigma = atof( argv[4] );

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

	cv::Mat diff, gray, zasum, sepOut, noSepOut, gauss_ref;
	int noSepCnt, sepCnt;
	
	bilySum( src_gray, zasum, noise_amp );
	
	separabilityGauss( zasum, filter_size, sigma, sepOut, noSepOut, noSepCnt, sepCnt );
	cv::GaussianBlur( zasum, gauss_ref, cv::Size(filter_size,filter_size), sigma );
	
	// vyhodnocení
	checkDifferences( zasum,     src_gray, "04_noise", true );
	checkDifferences( sepOut,   gauss_ref, "04_gaussSep", true );
	checkDifferences( noSepOut, gauss_ref, "04_gaussNoSep", true );
	std::cout << " " << noSepCnt << " " << sepCnt << " " << noSepCnt/sepCnt << std::endl;

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
		if( !diff.empty() ) { 	
			diff.convertTo( diff, CV_64FC1 );
			diff *= 1./255;
			pow( diff, 1./5, diff );
			diff *= 255.;
			diff.convertTo( diff, CV_8UC1 );
			cv::imwrite( (tag+"_err.png").c_str(), diff ); 
		}
	}

	printf( "%s %.2f %.2f %.2f ", tag.c_str(), err, nonzeros, mav );

	return;
}


