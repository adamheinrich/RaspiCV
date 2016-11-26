#ifndef CV_IMG_H
#define CV_IMG_H

#include <stdint.h>
#include "opencv2/imgproc/imgproc.hpp"

class cv_img
{
	cv::Mat m_mat;
	unsigned long m_timestamp;

public:
	cv_img(uint8_t *p_buffer, int width, int height, unsigned long timestamp)
	{
		int len = width * height * sizeof(uint8_t);
		m_timestamp = timestamp;
		m_mat.create(height, width, CV_8UC1);
		memcpy(m_mat.ptr<uchar>(0), p_buffer, len);		
	}

	~cv_img()
	{
		m_mat.release();
	}

	cv::Mat& mat()
	{
		return m_mat;
	}

	unsigned long timestamp()
	{
		return m_timestamp;
	}
};

#endif
