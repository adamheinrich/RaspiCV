#ifndef CV_H
#define CV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void cv_init(int width, int height, int fps, int fmt);
void cv_process_img(uint8_t *p_buffer, int length, int64_t timestamp);
void cv_process_imv(uint8_t *p_buffer, int length, int64_t timestamp);
void cv_close();

#ifdef __cplusplus
}
#endif

#endif