This minimal package passed `rhub::check_for_cran()`. The package borrow the structure of `yixuan/miniGPU`. 

However, to decieve the Linux environment provided by `rhub::check_for_cran()` (which, I hope, has the same configuration as the real CRAN server), several changes have been made:

1. In `/src/loader/icd_dispatch.h` lines 66 and 68, `#include <GL/gl.h>` and `#include <GLES/gl.h>` have been modified to `#include "GL/gl.h"` and `#include "GLES/gl.h"`.

2. Correspondingly, two directories `GL` and `GLES` are created in `/src/loader`. Those two directories hold the `gl.h` files found online.

If the above changes are not made, `rhub::check_for_cran()` would freak out over the failure of finding `gl.h` files which are not on the system paths.

The package function runs correctly under Windows. I can observe surge in GPU consumption. Currently I don't care whether the package function would run correctly under Linux.

