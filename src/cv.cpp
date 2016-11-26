#include "cv.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctime>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>

#include "opencv2/core/utility.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "cv_queue.h"
#include "cv_img.h"
#include "cv_imv.h"
#include "debug.h"

using namespace cv;
using namespace std;

static struct {
	int width;
	int height;
	int mbx;
	int mby;
} m_img;

static volatile bool m_initialized;
static pthread_t m_thread;

static cv_queue<cv_img*> m_img_queue;
static cv_queue<cv_imv*> m_imv_queue;

static suseconds_t microseconds()
{
	static struct timeval tv;
	gettimeofday(&tv, NULL);

	return ((tv.tv_sec * 1000000UL) + tv.tv_usec);
}

static void draw_imv(Mat& image, cv_imv_t *p_imv, Mat& colormap, int sad_limit)
{
	suseconds_t t1, t2;
	int i, j;

	t1 = microseconds();

	for (j = 0; j < m_img.mby; j++) {
		for (i = 0; i < m_img.mbx; i++) {
			cv_imv_t *p_vec = p_imv + (i+(m_img.mbx+1)*j);

			if (p_vec->x == 0 && p_vec->y == 0)
				continue;

			if (p_vec->sad > sad_limit)
				continue;

			int x = i*16 + 8;
			int y = j*16 + 8;

			float intensity = p_vec->sad;
			intensity = round(255 * intensity / sad_limit);

			if (intensity > 255)
				intensity = 255;

			uint8_t *ptr = colormap.ptr<uchar>(0);
			uint8_t idx = 3*(uint8_t)intensity;

			arrowedLine(image, Point(x+p_vec->x, y+p_vec->y),
				    Point(x, y),
				    Scalar(ptr[idx], ptr[idx+1], ptr[idx+2]));
		}
	}

	t2 = microseconds();

	DBG("draw_imv(): " << (t2-t1) << " us");
}

static void *process_thread(void *ptr)
{
	suseconds_t t, prev_t;
	int cnt = 0;
	int sad_limit = 2000;
	Mat colormap_img;
	Mat img_rgb;

	const char *img_win_title = "RaspiCV";
	const char *colormap_win_title = "SAD Colormap";
	const char *sad_trackbar_title = "SAD Threshold";

	img_rgb.create(m_img.height, m_img.width, CV_8UC3);

	/* Create colormap: */
	Mat colormap_in(64, 256, CV_8UC1);

	for (int i = 0; i < colormap_in.rows; i++)
		for (int j = 0; j < colormap_in.cols; j++)
			colormap_in.at<uchar>(i, j) = (uint8_t)j;

	applyColorMap(colormap_in, colormap_img, COLORMAP_JET);

	/* Initialize UI: */
	namedWindow(colormap_win_title, WINDOW_AUTOSIZE);
	imshow(colormap_win_title, colormap_img);

	namedWindow(img_win_title, WINDOW_AUTOSIZE);
	createTrackbar(sad_trackbar_title, img_win_title, &sad_limit, sad_limit);

	waitKey(1000);

	/* Run! */
	prev_t = microseconds();

	while (1) {
		cv_img *img = m_img_queue.remove();
		cv_imv *imv = m_imv_queue.remove();

		t = microseconds();

		unsigned long dt = imv->timestamp() - img->timestamp();

		DBG("[process] buffer dt: " << dt << ", thread dt: "
		    << (t - prev_t) << " us");

		cnt++;
		if (cnt == 10) {
			cvtColor(img->mat(), img_rgb, CV_GRAY2RGB);
			draw_imv(img_rgb, imv->imv(), colormap_img, sad_limit);
			imshow(img_win_title, img_rgb);
			cnt = 0;
		}

		waitKey(1);

		delete img;
		delete imv;

		prev_t = t;
	}
}

void cv_init(int width, int height, int fps, int fmt)
{
	DBG("cv_init(" << width << ", " << height << ", " << fps << ")");

	int imv_size = sizeof(cv_imv_t);
	if (imv_size != 4) {
		ERR("Error: sizeof(cv_imv_t)=" << imv_size << " instead of 4");
		return;
	}

	if (fmt != 3) {
		ERR("Format " << fmt << " is not grayscale");
		return;
	}

	m_img.width = width;
	m_img.height = height;

	if (m_img.width % 16 != 0) {
		ERR("Width " << m_img.width << " is not a multiple of 16");
		return;
	}

	if (m_img.height % 16 != 0) {
		ERR("Height " << m_img.height << " is not a multiple of 16");
		return;
	}

	m_img.mbx = m_img.width/16;
	m_img.mby = m_img.height/16;

	/* Start thread: */
	int rc = pthread_create(&m_thread, NULL, process_thread, NULL);
	if (rc) {
		ERR("Unable to create thread: " << rc);
		return;
	}

	m_initialized = true;
}

void cv_process_img(uint8_t *p_buffer, int length, int64_t timestamp)
{
	static int64_t prev_timestamp;
	suseconds_t t1, t2;

	if (!m_initialized)
		return;

	if (length != m_img.width*m_img.height) {
		ERR("Wrong img length: " << length);
		m_initialized = false;
		return;
	}

	t1 = microseconds();
	cv_img *img = new cv_img(p_buffer, m_img.width, m_img.height, t1);
	m_img_queue.add(img);
	t2 = microseconds();

	DBG("cv_process_img(p_buffer, " << length << ") [dts "
	    << (timestamp-prev_timestamp) << "]: " << (t2-t1) << " us");

	prev_timestamp = timestamp;
}

void cv_process_imv(uint8_t *p_buffer, int length, int64_t timestamp)
{
	static int64_t prev_timestamp;
	suseconds_t t1, t2;

	if (!m_initialized)
		return;

	if (length != (m_img.mbx+1)*(m_img.mby)*sizeof(cv_imv_t)) {
		ERR("Wrong imv length: " << length);
		m_initialized = false;
		return;
	}

	t1 = microseconds();
	cv_imv *imv = new cv_imv(p_buffer, m_img.mbx, m_img.mby, t1);
	m_imv_queue.add(imv);
	t2 = microseconds();

	DBG("cv_process_imv(p_buffer, " << length << ") [dts "
	    << (timestamp-prev_timestamp) << "]: " << (t2-t1) << " us");

	prev_timestamp = timestamp;
}

void cv_close(void)
{
	DBG("cv_close()");
	m_initialized = false;
}
