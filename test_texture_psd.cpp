
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <assert.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <fftw3.h>

using namespace cv;

static void help_and_exit( void )
{
  printf("Help!\n");
  printf("    -v         Verbose output\n");
  exit(-1);
}

const unsigned int PSD_WIDTH = 16;
const unsigned int PSD_HEIGHT = PSD_WIDTH;


int main( int argc, char **argv )
{
  bool verbose = false;
  char c;

  srand(50);

  while( (c = getopt( argc, argv, "?hvg:")) != -1 )
    switch( c ) {
      case 'v':
        verbose = true;
        break;
      case 'h':
      case '?':
        help_and_exit();
        break;
      default:
        printf("Don't understand option \'%c\'\n", c );
        help_and_exit(); 
    }

  //printf("Need to specify an image file on the command line.\n");

  if( verbose ) {
    printf( "Loading \"%s\" as image one.\n", argv[optind] );
  }

  Mat imageOne = imread( argv[optind], CV_LOAD_IMAGE_GRAYSCALE );

  //if( gaussian_stddev > 0 ) {
  //  if( verbose ) printf("Adding gaussian noise with stddev = %d to both images\n", gaussian_stddev );
  //  add_gaussian_noise( imageOne, gaussian_stddev );
 //}

  unsigned int gutter = PSD_WIDTH/2;

  // Check image
  if( imageOne.depth() != CV_8U || imageOne.channels() != 1 ) {
    printf("Image format not correct.\n");
    exit(-1);
  }

  fftw_complex *data_in;
  fftw_complex *fft_out;

  //data_in = (fftw_complex *)fftw_malloc( sizeof( fftw_complex ) * PSD_WIDTH * PSD_HEIGHT );
  //fft_out = (fftw_complex *)fftw_malloc( sizeof( fftw_complex ) * PSD_WIDTH * PSD_HEIGHT );

  data_in = (fftw_complex *)fftw_malloc( sizeof( fftw_complex ) * PSD_WIDTH * PSD_HEIGHT );
  fft_out = (fftw_complex *)fftw_malloc( sizeof( fftw_complex ) * PSD_WIDTH * PSD_HEIGHT );
  
  // Copy image data to data_in
  int k = 0;
  for( unsigned int i = 0; i < PSD_WIDTH; i++ ) {
    for( unsigned int j = 0; j < PSD_WIDTH;  j++ ) {
      data_in[k][0] = imageOne.at<unsigned char>( i, j );
      data_in[k][1] = 0.0;
      k++;
    }
  }

  fftw_plan planner = fftw_plan_dft_2d( PSD_WIDTH, PSD_HEIGHT, data_in, fft_out, FFTW_FORWARD, FFTW_MEASURE );
  fftw_print_plan( planner );
  fftw_execute( planner );

  // Copy data to an image and save
  Mat data_out( PSD_WIDTH, PSD_HEIGHT, CV_32F, Scalar(0) );
  Mat data_as_8u(PSD_WIDTH, PSD_HEIGHT, CV_8U, Scalar(0) );

  k = 0;
  for( unsigned int i = 0; i < PSD_WIDTH; i++ ) {
    for( unsigned int j = 0; j < PSD_WIDTH;  j++ ) {
      // Use magnitude
      printf("%d,%d  %f,%f\n", i,j,fft_out[k][0], fft_out[k][1] );
      data_out.at<float>(i,j) = sqrt( fft_out[k][0]*fft_out[k][0] + fft_out[k][1]*fft_out[k][1] );
      k++;
    }
  }

  data_out.convertTo(  data_as_8u, CV_8U, 256, 0 );
  imwrite("fftw.png", data_as_8u );



  fftw_destroy_plan( planner );
  fftw_free( data_in );
  fftw_free( fft_out );


//  //CvMat *result = cross_correlation( imageOne, imageTwo );
//  IplImage *result = phase_correlation( imageOne, imageTwo );
//
//  // Try monkeying with the values at DC
//  CvScalar avg = cvAvg( result, NULL );
//  cvSetReal2D( result, 0, 0, avg.val[0] );
//
//  double minVal, maxVal;
//  CvPoint minLoc, maxLoc;
//  cvMinMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, NULL );
//  printf("In result array:\n");
//  printf("   Min value of %f at %d,%d\n", minVal, minLoc.x, minLoc.y );
//  printf("   Max value of %f at %d,%d\n", maxVal, maxLoc.x, maxLoc.y );
//
//  //printf("At shift location (%d,%d), result value is %f\n", shift.x, shift.y, cvGetReal2D(result,shift.x+result->width/2,shift.y+result->height/2));
//  printf("At shift location (%d,%d), result value is %f\n", shift.x, shift.y, cvGetReal2D(result,shift.x,shift.y) );
//
//  // Result file is 1-channel 32bit float.
//  // Convert to a 8-bit greyscale 
//  CvMat *resultGrey = cvCreateMat( result->height, result->width, CV_8UC1 );
//  cvLog( result, result );
//  cvMinMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, NULL );
//  cvSubS( result, cvScalar( minVal, 0,0,0), result, NULL );
//  cvConvertScale( result, resultGrey, 255/(maxVal-minVal), 0 );
//
//  cvMinMaxLoc( resultGrey, &minVal, &maxVal, &minLoc, &maxLoc, NULL );
//  printf("In result greyscale array:\n");
//  printf("   Min value of %f at %d,%d\n", minVal, minLoc.x, minLoc.y );
//  printf("   Max value of %f at %d,%d\n", maxVal, maxLoc.x, maxLoc.y );
//
//  cvSaveImage("ccorr.png", resultGrey, 0 );


  return 0;
}
