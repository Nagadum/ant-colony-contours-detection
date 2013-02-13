#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp" 
#include <opencv2/highgui/highgui.hpp>

#include <iostream>  

#include "colony.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

  if (argc < 3) {
    cout << "usage : contour source dest" << endl;
    return 1;
  }

  cout << "loading" << endl;

  Mat image, imagec;
  image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
  imagec = imread(argv[1], CV_LOAD_IMAGE_COLOR); 

  Colony c(image);
  c.run();
  
  namedWindow( "Display window", CV_WINDOW_AUTOSIZE );

  for (int y = 0; y < imagec.cols; y++) {
    for (int x = 0; x < imagec.rows; x++) {
      char t = c.pheros.at<double>(x,y) * 255;
	imagec.at<cv::Vec3b>(x,y) = Vec3b(t,t,t);
    }
  }

  imshow( "Display window", imagec ); 
  imwrite(argv[2], imagec);
  waitKey(0);

  cout << "Image closed" << endl;
  
  return 0;
};

