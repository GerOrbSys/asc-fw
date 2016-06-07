/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** main.h
** Capture application to control the camera
**
** Author: Dimitri Emmerich (me@dimatosaurus.com)
** -------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))


struct buffer {
	void * start;
    size_t length;
};

struct buffer *         buffers         = NULL;
static unsigned int     n_buffers       = 0;


void set_format(uint32 width, uint32 height) {

}

static int xioctl(int fd, int request, void* argp)
{
	int r;

    do r = ioctl(fd, request, argp);
      while (-1 == r && EINTR == errno);

      return r;
}

static void write_frame(const void *ptr, int size) {
	FILE  * f = fopen("frame.raw", "wb");
        if(f == NULL) {
		printf("Failed to open file");
		return;
	}

	fwrite(ptr, size, 1, f);

	fclose(f);
}

int main(int argc, char **argv) {
	printf("Starting capture test\n");

	int fd = open("/dev/video0", O_RDWR);
	int i;
	if(fd<=-1) {
		printf("could not open video0 device");
		return 0;
	}

	struct v4l2_format fmt;

	int width  = 3856;
	int height = 2048;

	struct v4l2_format fmt;
	CLEAR(fmt);
 	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
 	fmt.fmt.pix.width       = width;
 	fmt.fmt.pix.height      = height;
 	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SBGGR16;
 	fmt.fmt.pix.field       = V4L2_FIELD_NONE;

	if(-1==xioctl(fd, VIDIOC_S_FMT, &fmt)) {
		printf("faild to choose format");
		return 0;
	}

	printf("choosen frame format: %d %d \n", fmt.fmt.pix.width, fmt.fmt.pix.height);

	// Request buffers
	struct v4l2_requestbuffers req;
	CLEAR(req);

	req.count = 2;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

 	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
 	 	printf("device does not support mmap");
 		return -1;
 	}

	printf("allocated buffers %i \n", req.count);

	buffers = calloc(req.count, sizeof(*buffers));
	if(!buffers) {
		printf("out of memory");
		return -1;
	}

	// mmap buffers
	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
	    struct v4l2_buffer buf;

	    CLEAR (buf);

	    buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	    buf.memory      = V4L2_MEMORY_MMAP;
	    buf.index       = n_buffers;

	    if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)) {
	    	printf("failed to query buffer");
	    	return -1;
	    }

	    buffers[n_buffers].length = buf.length;
	    buffers[n_buffers].start =
	    mmap (NULL /* start anywhere */, buf.length, PROT_READ | PROT_WRITE /* required */, MAP_SHARED /* recommended */, fd, buf.m.offset);

			printf("allocated buffer len: %d, pos: %x", buffers[n_buffers].length,
				buffers[n_buffers].start);

	    if (MAP_FAILED == buffers[n_buffers].start) {
	    	printf("failed to mmap");
	    	return -1;
	    }
  	}

  	// Queue buffers
  	for (i = 0; i < 2; ++i) {
        struct v4l2_buffer buf;

        CLEAR (buf);

        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = i;

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf)) {
        	printf("failed to queue buffer");
        	return -1;
        }

    }


  	enum v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == xioctl(fd, VIDIOC_STREAMON, &type)) {
    	printf("failed starting streaming");
    	return -1;
    }

	// Read frame
	struct v4l2_buffer buf;

	CLEAR (buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	printf("dequeing buffer..\n");

	if(-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
		printf("failed to dequeue buffer\n");
		return 0;
	}

	printf("finished dequeing\n");

	enum v4l2_buf_type tt;
	xioctl(fd, VIDIOC_STREAMOFF, &tt);


	printf("start writing file \n");
	printf("frame size: %d \n", buf.bytesused);

	write_frame(buffers[buf.index].start, buf.bytesused);

	printf("finished writing file \n");

	close(fd);

	return 0;
}
