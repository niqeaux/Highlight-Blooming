#pragma once

#include "opencv2/opencv.hpp"

using namespace cv;

void bloomifyImage(Mat srcColor, Mat dst, bool bloomingMode);
void integAverage(Mat srcImage, int radius);