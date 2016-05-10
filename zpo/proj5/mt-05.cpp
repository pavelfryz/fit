#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>



//  Funkce pro generování šumu typu pepř a sůl.
//  Počet udává, kolik pixelů v obraze bude šumem zkresleno.
void peprAsul( const cv::Mat& src, cv::Mat& dst, int pocet )
{
	dst = src.clone();
	for( int i = 0; i < pocet; ++i ) 
	{
		// Na náhodné pozice v obraze vygenerujte šum typu pepř a sůl.
		/* *** ZAČÁTEK VLASTNÍ IMPLEMENTACE 1 *** */
	//	dst.at<uchar>(rand()%dst.rows,rand()%dst.cols) = (rand()%2)*255;
		/* *** KONEC VLASTNÍ IMPLEMENTACE 1 *** */
	}
}


//  Funkce pro filtraci obrazu pomocí mediánového filtru.
//     - velikost filtru v pixelech

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
			/* *** ZAČÁTEK VLASTNÍ IMPLEMENTACE 2 *** */
			for(int y=0;y<velikost;++y) 
			for(int x=0;x<velikost;++x){
					buff[y*velikost+x] = srcBorder.at<uchar>(y+j,x+i);
			}
			/* *** KONEC VLASTNÍ IMPLEMENTACE 2 *** */
			
			// seřadíme hodnoty ve vektoru
			std::sort(buff.begin(), buff.end());

			// výslednou hodnotu uložte do výstupního obrazu
			/* *** ZAČÁTEK VLASTNÍ IMPLEMENTACE 3 *** */
			dst.at<uchar>(j,i) = buff[stred*velikost+stred];
			/* *** KONEC VLASTNÍ IMPLEMENTACE 3 *** */
		}
	}

	return;
}



/* Vyhodnocení/porovnání výsledku s referenčním obrazem. */
void checkDifferences( const cv::Mat test, const cv::Mat ref, std::string tag, bool save = false);


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//
// Examples of input parameters
// mt-05 input_image [noise_amount] [filter_size]
// e.g. ./mt-05 ../data/garden.png 0.05 7

int main(int argc, char* argv[])
{
    std::string img_path = "";
	float noise_amount = 0.05;
    int filter_size = 3;

	// check input parameters
	if( argc > 1 ) img_path = std::string( argv[1] );
	if( argc > 2 ) noise_amount = atof( argv[2] );
	if( argc > 3 ) filter_size = atoi( argv[3] );

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

	cv::Mat zasum, medi, medi_ref;

	int pocet = cvRound( noise_amount*src_gray.cols*src_gray.rows);
	peprAsul( src_gray, zasum, pocet );

	median( zasum, medi, filter_size );
	cv::medianBlur( zasum, medi_ref, filter_size );

	// vyhodnocení
	checkDifferences( zasum, src_gray, "05_noise", true );
	checkDifferences( medi,  medi_ref, "05_median", true );
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


