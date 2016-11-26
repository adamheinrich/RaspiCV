# RaspiCV

[RaspiVid][1] modified to provide image and motion vector data for processing
in [OpenCV][2].

The original [RaspiVid.c][3] is modified (see [f564a2f][4]) to pass image and
motion vector buffers to callbacks defined in `cv.h`. File `cv.cpp` implements
these callbacks and handles simple processing (display image and motion
vectors).

The motion vectors displayed by the application are colored according to their
SAD (sum of absolute differences) value. SAD value of 0 corresponds to blue,
SAD Threshold corresponds to red. Vectors with SAD above the threshold are not
displayed:

![screenshot][5]

## Building

First [build][6] the OpenCV library and make sure it is registered by
`pkg-config`:

```
pkg-config --libs opencv
```

Then make the `USERLAND_DIR` variable in `Makefile` point to Raspberry Pi's
[userland][7] repository. `Makefile` references some files required by
`RaspiVidCv.c`.

Finally, build the application by `make` and run it by `make run` or:

```
raspicv -v -w 640 -h 480 -fps 30 -t 0 -o /dev/null -x /dev/null -r /dev/null -rf gray
```

Currently the `cv.cpp` is limited to 640x480 grayscale image. This can be easily
modified (see function `cv_init()`).

[1]: https://www.raspberrypi.org/documentation/usage/camera/raspicam/raspivid.md
[2]: http://opencv.org/
[3]: https://github.com/raspberrypi/userland/blob/master/host_applications/linux/apps/raspicam/RaspiVid.c
[4]: https://github.com/adamheinrich/RaspiCV/commit/f564a2fd334ad1d6a0a0f4fea5f3e6aa749851d8
[5]: https://raw.githubusercontent.com/adamheinrich/RaspiCV/master/raspicv.png
[6]: http://www.pyimagesearch.com/2016/04/18/install-guide-raspberry-pi-3-raspbian-jessie-opencv-3/
[7]: https://github.com/raspberrypi/userland/
