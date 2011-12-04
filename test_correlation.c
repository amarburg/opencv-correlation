
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <assert.h>

#include <opencv2/core/core_c.h>
#include <opencv2/core/types_c.h>
#include <opencv2/highgui/highgui_c.h>

#include "lib/cross_correlation.h"
#include "lib/phase_correlation.h"


static void help_and_exit( void )
{
  printf("Help!\n");
  printf("    -v         Verbose output\n");
  printf("    -g <sd>    Add Gaussian noise w/ std. dev. = [sd]\n" );
  exit(-1);
}

const unsigned int SUBIMAGE_WIDTH = 640;
const unsigned int SUBIMAGE_HEIGHT = 480;
const unsigned int MAX_SHIFT = 50;

static CvPoint make_subimages( IplImage *base, IplImage **one, IplImage **two )
{
  assert( base->width > SUBIMAGE_WIDTH );
  assert( base->height > SUBIMAGE_HEIGHT );
  CvSize subimageSize = cvSize( SUBIMAGE_WIDTH, SUBIMAGE_HEIGHT );
  int xshift = rint( rand()*1.0*MAX_SHIFT/RAND_MAX );
  int yshift = rint( rand()*1.0*MAX_SHIFT/RAND_MAX );
  CvPoint shift = cvPoint( xshift, yshift );
  int xcorner = rint( rand()*1.0*(base->width - subimageSize.width - xshift-1)/RAND_MAX);
  int ycorner = rint( rand()*1.0*(base->height - subimageSize.height - yshift-1)/RAND_MAX);
  CvPoint cornerOne = cvPoint( xcorner, ycorner );
  CvPoint cornerTwo = cvPoint( xcorner+xshift, ycorner+yshift );

  //printf("Shifting by %d, %d\n", xshift,yshift );
  //printf("Image one at %d, %d\n", cornerOne.x, cornerOne.y );
  //printf("Image two at %d, %d\n", cornerTwo.x, cornerTwo.y );

  CvRect rectOne = cvRect( cornerOne.x, cornerOne.y, subimageSize.width, subimageSize.height );
  cvSetImageROI( base, rectOne );
  *one = cvCreateImage( subimageSize, base->depth, base->nChannels );
  cvCopy( base, *one, NULL );
  cvResetImageROI( base );

  rectOne = cvRect( cornerTwo.x, cornerTwo.y, subimageSize.width, subimageSize.height );
  cvSetImageROI( base, rectOne );
  *two = cvCreateImage( subimageSize, base->depth, base->nChannels );
  cvCopy( base, *two, NULL );

  return shift;
}


static void  add_gaussian_noise( IplImage *img, unsigned int stddev )
{
  // This function assumes 8-bit greyscale images 
  assert( img->nChannels == 1 );
  assert( img->depth == IPL_DEPTH_8U );

  CvRNG rng = cvRNG( 0x1231123 );
    CvMat *mat = cvCreateMat( img->height, img->width, CV_8SC1 );

  cvRandArr( &rng, mat, CV_RAND_NORMAL, cvScalar(0,0,0,0), cvScalar( stddev,stddev,stddev,stddev ) );

  cvAdd( img, mat, img, NULL );
}

int main( int argc, char **argv )
{
  IplImage *imageOne = NULL, *imageTwo = NULL, *imageBase = NULL;
  bool verbose = false;
  CvPoint shift;
  char c;
  int gaussian_stddev = 0;

  srand(50);

  while( (c = getopt( argc, argv, "?hvg:")) != -1 )
    switch( c ) {
      case 'v':
        verbose = true;
        break;
      case 'g':
        gaussian_stddev = atoi( optarg );
        break;
      case 'h':
      case '?':
        help_and_exit();
        break;
      default:
        printf("Don't understand option \'%c\'\n", c );
        help_and_exit(); 
    }

  switch( argc - optind )
  {
    case 2:
      if( verbose ) {
        printf( "Loading \"%s\" as image one.\n", argv[optind] );
        printf( "Loading \"%s\" as image two.\n", argv[optind+1] );
      }

      imageOne = cvLoadImage( argv[optind], CV_LOAD_IMAGE_GRAYSCALE );
      imageTwo = cvLoadImage( argv[optind+1], CV_LOAD_IMAGE_GRAYSCALE );

      // this case is untested
      break;
    case 1:
      if( verbose ) {
        printf( "Loading \"%s\" as base image.\n", argv[ optind ] );
      }

      imageBase = cvLoadImage( argv[optind], CV_LOAD_IMAGE_GRAYSCALE );
      shift = make_subimages( imageBase, &imageOne, &imageTwo );

      if( verbose ) printf("Shift is %d, %d\n", shift.x, shift.y );

      break;
    default:
      printf("Need to specify an image file on the command line.\n");
  }

  if( gaussian_stddev > 0 ) {
    if( verbose ) printf("Adding gaussian noise with stddev = %d to both images\n", gaussian_stddev );
    add_gaussian_noise( imageOne, gaussian_stddev );
    add_gaussian_noise( imageTwo, gaussian_stddev );
  }

  cvSaveImage("imageOne.png", imageOne, 0 );
  cvSaveImage("imageTwo.png", imageTwo, 0 );


  //CvMat *result = cross_correlation( imageOne, imageTwo );
  IplImage *result = phase_correlation( imageOne, imageTwo );

  double minVal, maxVal;
  CvPoint minLoc, maxLoc;
  cvMinMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, NULL );
  printf("In result array:\n");
  printf("   Min value of %f at %d,%d\n", minVal, minLoc.x, minLoc.y );
  printf("   Max value of %f at %d,%d\n", maxVal, maxLoc.x, maxLoc.y );

  printf("At shift location (%d,%d), result value is %f\n", shift.x, shift.y, cvGetReal2D(result,shift.x+result->width/2,shift.y+result->height/2));

  // Result file is 1-channel 32bit float.
  // Convert to a 8-bit greyscale 
  CvMat *resultGrey = cvCreateMat( result->height, result->width, CV_8UC1 );
  cvLog( result, result );
  cvMinMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, NULL );
  cvSubS( result, cvScalar( minVal, 0,0,0), result, NULL );
  cvConvertScale( result, resultGrey, 255/(maxVal-minVal), 0 );

  cvMinMaxLoc( resultGrey, &minVal, &maxVal, &minLoc, &maxLoc, NULL );
  printf("In result greyscale array:\n");
  printf("   Min value of %f at %d,%d\n", minVal, minLoc.x, minLoc.y );
  printf("   Max value of %f at %d,%d\n", maxVal, maxLoc.x, maxLoc.y );

  cvSaveImage("ccorr.png", resultGrey, 0 );


  return 0;
}
