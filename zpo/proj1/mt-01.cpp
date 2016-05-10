#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


/*
	Implementujte vlastnoručně funkci, která převede barevný 3-kanálový obraz ve formátu BGR do jednokanálového šedotónového obrazu.
	G = 0.299*R + 0.587*G + 0.114*B;

	Upozornění: ...
	Note that the default color format in OpenCV is often referred to as RGB but it is actually BGR (the bytes are reversed). 
	So the first byte in a standard (24-bit) color image will be an 8-bit Blue component, the second byte will be Green, and 
	the third byte will be Red.

	Realizujte pomocí ukazatelů přímo do paměti: unsigned char * ptr.
	Ukazatel na data matice je: Mat.data.
	Pozor, šířka řádku nemusí z důvodu zarovnání v paměti odpovídat: počet_kanálů * šířka_matice
		- tedy ne: Mat.cols*Mat.channels()
		- ale využít atribut: Mat.step

*/
void bgr2gray( const cv::Mat bgr, cv::Mat& gray )
{
	gray = cv::Mat::zeros(bgr.rows, bgr.cols, CV_8UC1 );

	/*
		Zde bude vlastnoruční implementace algoritmu pro převod BGR na GRAY obrázek.
	*/
	uchar* src = bgr.data;  //ukazatel na zdrojova data
	uchar* dst = gray.data; //ukazatel na cilova data

	int pixels = bgr.rows * bgr.cols;
	for(int i = 0; i < pixels; ++i) {
		//                               modra         zelena          cervena
		*dst = cv::saturate_cast<uchar>(*src*0.114 + *(src+1)*0.587 + *(src+2)*0.299);
		dst += gray.step[1]; //posun v cilovem obraze o jeden pixel
		src += bgr.step[1];  //posun ve zdrojovem obraze o jeden pixel
	}

}



/*  Funkce pro výpočet histogramu šedotónového obrazu (0-255) a pomocí ekvalizace histogramu zlepšení jeho kontrastu.
    Můžete využít funkci pro přístup k hodnotám matice: Mat.at<>
	Příklad: hodnotu jasu pixelu na řádku 100 a sloupci 200 (tedy x=200, y=100) šedotónového obrazu získáme:
		unsigned char pixel_value = obraz.at<unsigned chat>(100,200);
	
	Dále se hodí funkce saturate_cast, která provádí efektivní a přesný převod mezi číselnými typy.
	Příklad snížení kontrastu na polovic a snížení intensity jasu celého obrazu o 127 pak může pro jeden pixel vypadat takto:
		obraz.at<unsigned chat>(100,200) = saturate_cast<unsigned char>( 0.5 * obraz.at<unsigned chat>(100,200) - 127.0 );
	
	Informace o ekvalizaci histogramu, např.:
		http://docs.opencv.org/doc/tutorials/imgproc/histograms/histogram_equalization/histogram_equalization.html

*/
void histogramEqualization( const cv::Mat gray, cv::Mat& eq_gray )
{
	cv::Mat h = cv::Mat::zeros( 1, 256, CV_32SC1 );
	eq_gray   = cv::Mat::zeros(gray.size(), CV_8UC1 );

	// projděte obraz a spočtěte histogram obrazu
	for(int y = 0; y < gray.rows; ++y)
	for(int x = 0; x < gray.cols; ++x) {
		h.at<int>(gray.at<uchar>(y,x))++;
	}


	// spočtěte akumulovaný histogram
	// hodnota v akumulovaném histogramu pro danou intensitu je rovna součtu všech hodnot histogramu s nižšími intenzitami.
	int min = h.at<int>(0); //minimalni nenulova hodnota v akumulovaném histogramu
	for(int i = 1; i < h.cols; ++i) {
		h.at<int>(i) += h.at<int>(i-1);
		if(!min) min = h.at<int>(i);
	}


	// přepočti hodnoty jasů v obraze
	double norm_coef = 255./(gray.cols*gray.rows-min);
	for(int y = 0; y < gray.rows; ++y)
	for(int x = 0; x < gray.cols; ++x) {
		int value = h.at<int>(gray.at<uchar>(y,x))-min;
		eq_gray.at<uchar>(y,x) = cv::saturate_cast<uchar>(value*norm_coef);
	}

	return;
}



//---------------------------------------------------------------------------
void checkDifferences( const cv::Mat test, const cv::Mat ref, std::string tag, bool save = false);
//---------------------------------------------------------------------------


//
// Examples of input parameters
//
// ./mt-01 ../data/garden.png 


int main(int argc, char* argv[])
{
    std::string img_path = "";

	// check input parameters
	if( argc > 1 ) {
	    img_path = std::string( argv[1] );
	}

	// load testing images
	cv::Mat src_rgb = cv::imread( img_path );

	// check testing images
	if( src_rgb.empty() ) {
		std::cout << "Failed to load image: " << img_path << std::endl;
		return -1;
	}


	// prepare reference solution
	cv::Mat gray_ref, equa_ref;
	cv::cvtColor( src_rgb, gray_ref, CV_BGR2GRAY );	
	cv::equalizeHist( gray_ref, equa_ref );
	

	// execute implemented function
	cv::Mat gray_man, equa_man;
	bgr2gray( src_rgb, gray_man );		
	// using reference gray image		
	histogramEqualization( gray_ref, equa_man );


	// evaluate solution
	checkDifferences( gray_man, gray_ref, "01_bgr2gray", true );
	checkDifferences( equa_man, equa_ref, "01_hist_equ", true );
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
