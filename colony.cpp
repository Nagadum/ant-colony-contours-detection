#include "colony.h"

#include <cmath>
#include <cstdlib>
#include <omp.h>

#include <opencv2/imgproc/imgproc.hpp>


#include "consts.h"

Colony::Colony(cv::Mat _img) {

  img = _img;
  pheros = cv::Mat(_img.rows, _img.cols, CV_64F, TAU_0);
  pheros2 = cv::Mat(_img.rows, _img.cols, CV_64F, TAU_0);
  heuris = cv::Mat(_img.rows, _img.cols, CV_64F, 0.001);

  int maxX = _img.rows;
  int maxY = _img.cols;
  unsigned int K = std::sqrt(maxX*maxY);

  for (unsigned int k = 0; k < K; k++) {
    unsigned int x = (std::rand() % (maxX-1))+1;
    unsigned int y = (std::rand() % (maxY-1))+1;
    m_vAntToMove.push_back(new Ant(x,y, maxX, maxY));
  }

  std::cout << m_vAntToMove.size() << " ants created" << std::endl;

  computeHeuris(_img);
  
}

Colony::~Colony() {

  int n = m_vAntToMove.size() + m_vAntMoved.size();

  for (std::vector<Ant*>::iterator it = m_vAntToMove.begin(); it != m_vAntToMove.end(); it++) {
    delete *it;
  }

  for (std::vector<Ant*>::iterator it = m_vAntMoved.begin(); it != m_vAntMoved.end(); it++) {
    delete *it;
  }

  std::cout << n << " ants killed" << std::endl;

}

void Colony::run() {

  //Construction proccess
  for (int n = 0; n < N_MAX; n++) {

    Ant* l_ant;

#pragma OMP parallel shared(m_vAntToMove, m_vAntMoved, pheros, heuris) private (l_ant)
    {
      while(m_vAntToMove.size() > 0) {

	unsigned int k = std::rand() % m_vAntToMove.size();
	l_ant = m_vAntToMove[k];
	m_vAntToMove.erase(m_vAntToMove.begin()+k);
	
	for (int i = 0; i < NB_STEP; i++) {
	  moveAnt(l_ant);
	}
	
	m_vAntMoved.push_back(l_ant);

      }
    }

    m_vAntToMove.swap(m_vAntMoved);

    /*if ((100*n)/N_MAX % 10 == 0)
      std::cout << (100*n/N_MAX) << "%" <<  std::endl;*/

    //Evaporation
    for (int y = 0; y < pheros.cols; y++) {
      for (int x = 0; x < pheros.rows; x++) {
	pheros.at<double>(x,y) = (1 - PSI)*pheros.at<double>(x,y) + PSI * TAU_0;
      }
    }
  }

  //Threshold
  std::cout << "Decision process" << std::endl;

  double T = computeThreshold();

  for (int y = 0; y < pheros.cols; y++) {
    for (int x = 0; x < pheros.rows; x++) {
      if (pheros.at<double>(x,y) > T)
	pheros.at<double>(x,y) = 1;
      else
	pheros.at<double>(x,y) = TAU_0;
    }
  }

  cv::GaussianBlur( pheros, pheros2, cv::Size( 5, 5 ), 0, 0 );

}

#if F_INDEX == 1
double Colony::f(double _d) {
  return LAMBDA * _d;
}
#endif

#if F_INDEX == 2
double Colony::f(double _d) {
  return LAMBDA * _d * _d;
}
#endif

#if F_INDEX == 3
double Colony::f(double _d) {
  if (_d <= LAMBDA)
    return std::sin(M_PI * _d / (2*LAMBDA));
  else
    return 0;
}
#endif

#if F_INDEX == 4
double Colony::f(double _d) {
  if (_d <= LAMBDA)
    return (M_PI * _d * std::sin(M_PI * _d / LAMBDA))/LAMBDA;
  else
    return 0;
}
#endif

void Colony::computeHeuris(cv::Mat& _img) {

  for (int y = 2; y < _img.cols-2; y++) {
    for (int x = 2; x < _img.rows-2; x++) {
      double value = 0;
      value += std::abs(img.at<char>(x-2,y-1) - img.at<char>(x+2,y+1));
      value += std::abs(img.at<char>(x-2,y+1) - img.at<char>(x+2,y-1));
      value += std::abs(img.at<char>(x-1,y-2) - img.at<char>(x+1,y+2));
      value += std::abs(img.at<char>(x-1,y-1) - img.at<char>(x+1,y+1));
      value += std::abs(img.at<char>(x-1,y+0) - img.at<char>(x+1,y+0));
      value += std::abs(img.at<char>(x-1,y+1) - img.at<char>(x-1,y-1));
      value += std::abs(img.at<char>(x-1,y+2) - img.at<char>(x-1,y-2));
      value += std::abs(img.at<char>(x+0,y-1) - img.at<char>(x+0,y+1));
      heuris.at<double>(x,y) = f(value/(8*255));
    }
  }
}

void Colony::moveAnt(Ant* _ant) {

  unsigned int x0 = _ant->getX();
  unsigned int y0 = _ant->getY();

  cv::Mat localPheros = pheros(cv::Range(x0-1, x0+1), cv::Range(y0-1, y0+1));
  cv::Mat localHeuris = heuris(cv::Range(x0-1, x0+1), cv::Range(y0-1, y0+1));

  _ant->move( localHeuris, localPheros );

  unsigned int x1 = _ant->getX();
  unsigned int y1 = _ant->getY();
  
  double oldPhero = pheros.at<double>(x1,y1);
  double pheroDelta = heuris.at<double>(x1,y1);
  
  double newPhero = (1 - RO) * oldPhero + RO * pheroDelta;
  pheros.at<double>(x1,y1) = newPhero;

}

double Colony::computeThreshold() {

  //Initialisation
  double psum = 0;
  for (int y = 0; y < pheros.cols; y++) {
    for (int x = 0; x < pheros.rows; x++) {
      psum += pheros.at<double>(x,y);
    }
  }

  double T = psum / (pheros.cols * pheros.rows);

  unsigned int nbIter = 0;

  while(nbIter < MAX_ITER) {
    double psumL = 0;
    double psumU = 0;
    double nbL = 0;
    double nbU = 0;
    for (int y = 0; y < pheros.cols; y++) {
      for (int x = 0; x < pheros.rows; x++) {
	double p = pheros.at<double>(x,y);
	if (p >= T) {
	  psumL += p;
	  nbL++;
	} else {
	  psumU += p;
	  nbU++;
	}
      }
    }

    double oldT = T;
    T = ((psumL/nbL)+(psumU/nbU))/2;

    if (std::abs(T-oldT) < EPSILON) 
      break;

  }

  std::cout << "Threshold : " << T << std::endl;

  nbIter++;

  //T = 10 * TAU_0;

  return T;

}
