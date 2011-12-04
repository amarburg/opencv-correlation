
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <assert.h>

#include <opencv2/core/core_c.h>
#include <opencv2/core/types_c.h>
#include <opencv2/highgui/highgui_c.h>


static void help_and_exit( void )
{
  printf("Help!\n");
  exit(-1);
}

const unsigned int SUBIMAGE_WIDTH = 640;
const unsigned int SUBIMAGE_HEIGHT = 480;
const unsigned int MAX_SHIFT = 50;
const unsigned int NOISE_STDDEV = 5;

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

  srand(50);

  while( (c = getopt( argc, argv, "v")) != -1 )
    switch( c ) {
      case 'v':
        verbose = true;
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

      add_gaussian_noise( imageOne, NOISE_STDDEV );
      add_gaussian_noise( imageTwo, NOISE_STDDEV );

      cvSaveImage("imageOne.png", imageOne, 0 );
      cvSaveImage("imageTwo.png", imageTwo, 0 );

      break;
    default:
      printf("Need to specify an image file on the command line.\n");
  }




  return 0;
}
