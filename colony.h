#ifndef COLONY_H
#define COLONY_H

#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp" 

#include "ant.h"

class Colony {

 public:
  Colony(cv::Mat _img);
  ~Colony();
  void run();

  cv::Mat img;
  cv::Mat heuris;
  cv::Mat pheros;
  cv::Mat pheros2;

 private:
  std::vector<Ant*> m_vAntToMove;
  std::vector<Ant*> m_vAntMoved;

  double f(double);
  void computeHeuris(cv::Mat&);
  void moveAnt(Ant*);
  double computeThreshold();

};

#endif
