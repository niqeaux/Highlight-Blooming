#include "blooming.h"


void bloomifyImage(Mat srcColor, Mat dst, bool bloomingMode) {

	// Convert from BGR to Grayscale
	Mat srcGray;
	cvtColor(srcColor, srcGray, COLOR_BGR2GRAY);

	if (bloomingMode) {
		Mat tmp = Mat(srcGray.size(), CV_8UC1);
		//Mat tmp3 = Mat(srcGray.size(), CV_32FC1);
		int radius = 100;

		// add mirror padding
		copyMakeBorder(srcGray, tmp, radius, radius, radius, radius, BORDER_REFLECT_101);
		Mat tmp2 = Mat(tmp.size(), CV_32FC1);

		// darken low intensity pixels with power function
		tmp.convertTo(tmp, CV_32F, 1.f/255);
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
		int r, c;
		uchar* p;
		for (int r = radius; r < tmp.rows - radius; ++r) {
			p = tmp.ptr<uchar>(r);
			for (int c = radius; c < tmp.cols - radius; ++c) {
				p[c] = round((tmp2.at<float>(r + radius, c + radius) - tmp2.at<float>(r - radius, c + radius)
					- tmp2.at<float>(r + radius, c - radius) + tmp2.at<float>(r - radius, c - radius)) / (radius * radius));
			}
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

