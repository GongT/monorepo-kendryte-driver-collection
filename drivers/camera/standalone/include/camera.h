#ifndef _CAMERA_H_
#define _CAMERA_H_

#if OV5640 + OV2640 != 1
#error ov sensor choose and only choose one
#endif

void camera_init();

#endif
