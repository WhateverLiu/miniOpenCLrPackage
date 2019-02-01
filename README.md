This minimal package passed `rhub::check_for_cran()`. The package borrows contents in `yixuan/miniGPU`. 

However, to decieve the Linux building environment provided by `rhub::check_for_cran()` (which, merely based on my experience on one CRAN publication, has the same configuration as the real CRAN server), several modifications are needed:

1. In `/src/loader/icd_dispatch.h` lines 66 and 68, `#include <GL/gl.h>` and `#include <GLES/gl.h>` are changed to `#include "GL/gl.h"` and `#include "GLES/gl.h"`.

2. Respectively, two subdirectories `GL` and `GLES` are created in `/src/loader`. Those two hold two versions of `gl.h` found online.

If the above changes are not made, `rhub::check_for_cran()` would freak out over the failure of finding `gl.h`.

The package function runs correctly under Windows. I can observe surge in GPU consumption. Currently I don't care whether the function would run correctly under Linux,

..and it doesn't. Just tested it on Ubuntu 16.04 on a virtual machine installed on Windows. Lots of explanations can be made, like did I install my Nvidia driver on Ubuntu?

