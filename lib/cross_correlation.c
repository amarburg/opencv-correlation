
#include <stdio.h>

#include <opencv2/core/core_c.h>
#include <opencv2/core/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>


// Computes the normalized cross-correlation between two images 
CvMat *cross_correlation( IplImage *img, IplImage *templ )
{
  // Zero-pad "img->
  IplImage *paddedImg = cvCreateImage( cvSize( img->width*2-1, img->height*2-1 ), img->depth, img->nChannels );
  cvSet( paddedImg, cvScalar( 0,0,0,0 ), NULL );
  // TODO: Will break on odd-sized images
  CvRect roi = cvRect( img->width/2, img->height/2, img->width, img->height );
  cvSetImageROI( paddedImg, roi );
  cvCopy( img, paddedImg, NULL );
  cvResetImageROI( paddedImg );

  CvMat *result = cvCreateMat( img->height, img->width, CV_32FC1 );

  printf("Result size %d,%d\n", result->cols, result->rows );
  printf("Img size    %d,%d\n", img->width, img->height );
  printf("Templ size  %d,%d\n", templ->width, templ->height );
  printf("Padded size %d,%d\n", paddedImg->width, paddedImg->height );

  assert( result->cols == ( abs(paddedImg->width - templ->width) + 1 ) );
  assert( result->rows == ( abs(paddedImg->height - templ->height) + 1 ) );

  cvMatchTemplate( paddedImg, templ, result, CV_TM_CCORR_NORMED );

  return result;
}
