#include "ant.h"

#include <cmath>

#include "consts.h"

Ant::Ant(unsigned int _x, unsigned int _y, unsigned int _maxX, unsigned int _maxY) {
  x = _x;
  y = _y;
  maxX = _maxX;
  maxY = _maxY;
}

unsigned int Ant::getX() {
  return x;
}

unsigned int Ant::getY() {
  return y;
}

void Ant::move(cv::Mat& heuris, cv::Mat& pheros) {

  int dir = chooseDest(heuris, pheros);
  makeMove(dir);
}

int Ant::chooseDest(cv::Mat& heuris, cv::Mat& pheros) {
  double d1 = std::pow(pheros.at<double>(0,0), ALPHA) * std::pow(heuris.at<double>(0,0), BETA);
  double d2 = std::pow(pheros.at<double>(1,0), ALPHA) * std::pow(heuris.at<double>(1,0), BETA);
  double d3 = std::pow(pheros.at<double>(2,0), ALPHA) * std::pow(heuris.at<double>(2,0), BETA);
  double d4 = std::pow(pheros.at<double>(0,1), ALPHA) * std::pow(heuris.at<double>(0,1), BETA);
  double d5 = std::pow(pheros.at<double>(2,1), ALPHA) * std::pow(heuris.at<double>(2,1), BETA);
  double d6 = std::pow(pheros.at<double>(0,2), ALPHA) * std::pow(heuris.at<double>(0,2), BETA);
  double d7 = std::pow(pheros.at<double>(1,2), ALPHA) * std::pow(heuris.at<double>(1,2), BETA);
  double d8 = std::pow(pheros.at<double>(2,2), ALPHA) * std::pow(heuris.at<double>(2,2), BETA);
  double total = d1 + d2 + d3 + d4 + d5 + d6 + d7 + d8;

  double tirage = double(std::rand()) / double(RAND_MAX);

  if ( tirage < d1/total )
    return 1;
  else if ( tirage < (d1+d2)/total )
    return 2;
  else if ( tirage < (d1+d2+d3)/total )
    return 3;  
  else if ( tirage < (d1+d2+d3+d4)/total )
    return 4; 
  else if ( tirage < (d1+d2+d3+d4+d5)/total )
    return 5;  
  else if ( tirage < (d1+d2+d3+d4+d5+d6)/total )
    return 6;  
  else if ( tirage < (d1+d2+d3+d4+d5+d6+d7)/total )
    return 7;
  else
    return 8;

}

void Ant::makeMove(int dir) {
  int deltaX, deltaY;

  switch (dir) {
  case 1 : deltaX = -1; deltaY = -1; break;
  case 2 : deltaX = 0; deltaY = -1; break;
  case 3 : deltaX = 1; deltaY = -1; break;
  case 4 : deltaX = -1; deltaY = 0; break;
  case 5 : deltaX = 1; deltaY = 0; break;
  case 6 : deltaX = -1; deltaY = +1; break;
  case 7 : deltaX = 0; deltaY = +1; break;
  case 8 : deltaX = 1; deltaY = +1; break;
  }

  if ( (x + deltaX) < 1 || (x + deltaX) > maxX-1)
    deltaX = 0;

  if ( (y + deltaY) < 1 || (y + deltaY) > maxY-1)
    deltaY = 0;

  x += deltaX;
  y += deltaY;
}
