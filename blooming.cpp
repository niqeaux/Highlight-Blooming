#include "blooming.h"


void bloomifyImage(Mat srcColor, Mat dst, bool bloomingMode) {

	// Convert from BGR to Grayscale
	Mat srcGray;
	cvtColor(srcColor, srcGray, COLOR_BGR2GRAY);

	if (bloomingMode) {
		Mat tmp = Mat(srcGray.size(), CV_8UC1);

		int radius = 10;

		// add mirror padding
		copyMakeBorder(srcGray, tmp, radius, radius, radius, radius, BORDER_REFLECT_101);
		Mat tmp2 = Mat(tmp.size(), CV_32FC1);

		// darken low intensity pixels with power function
		tmp.convertTo(tmp, CV_32F, 1.f / 255);
		pow(tmp, 10, tmp);
		tmp = tmp * 255;
		tmp.convertTo(tmp, CV_8U);

		// create integral image
		integral(tmp, tmp2, CV_32F);

		// approximate sigma
		float sigma = 3.1;
		int n = 6;
		float wIdeal = sqrt(12 * (sigma * sigma) / n + 1); // Ideal averaging filter width
		// wl is first odd valued integer less than wIdeal
		int wl = floor(wIdeal);
		if (wl % 2 == 0) {
			wl = wl - 1;
		}
		// wu is the next odd value > wl
		int wu = wl + 2;
		// Compute m.Refer to the tech note for derivation of this formula
		float mIdeal = (12 * sigma * sigma - n * wl * wl - 4 * n * wl - 3 * n) / (-4 * wl - 4);
		int m = round(mIdeal);
		// Compute actual sigma that will be achieved
		float sigmaActual = sqrt((m * wl * wl + (n - m) * wu * wu - n) / 12);
		int radl = (wl - 1) / 2;
		int radu = (wu - 1) / 2;

		// filter integral image
		for (int i = 0; i < m; ++i) {
			integAverage(tmp, radl);
		}
		for (int j = 0; j < n - m; ++j) {
			integAverage(tmp, radu);
		}

		// crop to original size
		tmp = tmp(Rect(radius, radius, srcGray.cols, srcGray.rows));

		// convert from grayscale to bgr
		cvtColor(tmp, tmp, COLOR_GRAY2BGR);

		// add to image
		dst = srcColor + tmp;

	}
	else {
		srcColor.copyTo(dst);
		//dst.setTo(0);
	}
}

void integAverage(Mat image, int radius) {

	Mat tmpImage = Mat(image.size(), CV_32FC1);

	integral(image, tmpImage, CV_32F); // create integral image

	int down = radius;
	int up = down + 1;
	int r, c;
	uchar* p;
	for (int r = up; r < image.rows - down; ++r) {
		p = image.ptr<uchar>(r);
		for (int c = up; c < image.cols - down; ++c) {
			p[c] = round((tmpImage.at<float>(r + down, c + down) - tmpImage.at<float>(r - up, c + down)
				- tmpImage.at<float>(r + down, c - up) + tmpImage.at<float>(r - up, c - up)) / ((up + down) * (up + down)));
		}
	}
}