#ifndef ANT_H
#define ANT_H

#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"

class Ant {

 public:
  Ant(unsigned int _x, unsigned int _y, unsigned int _maxX, unsigned int _maxY);
  void move(cv::Mat& heuris, cv::Mat& pheros);

  unsigned int getX();
  unsigned int getY();


 private:
  unsigned int x, y, maxX, maxY;

  int chooseDest(cv::Mat& heuris, cv::Mat& pheros);
  void makeMove(int dir);

};

#endif
