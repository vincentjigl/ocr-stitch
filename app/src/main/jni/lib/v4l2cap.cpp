#include "v4l2_client.h"
#include <linux/videodev2.h>
#include "cmr_log.h"
#include "cmr_types.h"
#include "cmr_common.h"
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define PARAM_NUM 7
#define MAX_FPS 120
#define MIN_FPS 5
#define WIDTH_MAX 4640
#define HEIGHT_MAX 3488
#define FRAME_MAX 1000
#define FRAME_MIN 60

typedef int(*imgProcCall)(int, int, char*, void*);

bool bPlay = true;
imgProcCall gipc;

#define TEST_LOGE(fmt, args...)
#define TEST_LOGD(fmt, args...)
#define TEST_LOGV(fmt, args...)

typedef struct buffer_type_r {
  char *start;
  int length;
} buffer_type;

buffer_type *user_buffer = NULL;

struct context_test {
  V4l2AdapterInterface *camera_handle;
  int fd;
  unsigned int width;
  unsigned int height;
  unsigned int fps;
  unsigned int frame_count;
  unsigned int dump_count;
};

static void usage_test(void) {
  fprintf(stderr, "Usage:\n\n"
                  "Get usage help:\n"
                  "\thighfpscamera -help\n\n"

                  "Get supported fps:\n"
                  "\thighfpscamera -getfps\n\n"

                  "Test command:\n"
                  "\thighfpscamera -w width -h height -fps framerate -frame_cnt "
                  "frame_count -dump_cnt dump_count\n\n"

                  "Start high fps mode when framerate>=90\n\n"

                  "For example:\n"
                  "\thighfpscamera -w 640 -h 480 -fps 90\n"
                  "\thighfpscamera -w 640 -h 480 -fps 90 -frame_cnt 100\n"
                  "\thighfpscamera -w 640 -h 480 -fps 90 -frame_cnt 100 -dump_cnt 20\n\n");
}

static int Query_FrameInterval(int fd, V4l2AdapterInterface *v4l2handle,
                               struct v4l2_frmsizeenum *fsize) {
  int ret = -CMR_CAMERA_FAIL;
  struct v4l2_frmivalenum fival = {0};

  if (!fsize) {
    printf("struct v4l2_frmsizeenum param is NULL, query FrameInterval fail\n");
    CMR_LOGE("struct v4l2_frmsizeenum param is NULL\n");
    ret = CMR_CAMERA_INVALID_PARAM;
    goto FAILED;
  } else {
    fival.width = fsize->discrete.width;
    fival.height = fsize->discrete.height;
  }
  while ((ret = v4l2handle->ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &fival)) ==
         CMR_CAMERA_SUCCESS) {
    printf("\t%s frame rate %d fps:\t[denominator:%d  numerator:%d]\n",
           fival.index == 0 ? "Min" : fival.index == 1 ? "Max" : "Other",
           fival.discrete.denominator, fival.discrete.denominator,
           fival.discrete.numerator);
    if (fival.index == 1)
      break;
    fival.index++;
  }
FAILED:
  return ret;
}

static int Enum_FrameSizeIvals(int fd, V4l2AdapterInterface *v4l2handle) {
  int ret = -CMR_CAMERA_FAIL;
  struct v4l2_frmsizeenum fsize = {0};

  while ((ret = v4l2handle->ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &fsize)) ==
         CMR_CAMERA_SUCCESS) {
    printf("\nFind fmt:\t[w:%d x h:%d]\n", fsize.discrete.width,
           fsize.discrete.height);
    ret = Query_FrameInterval(fd, v4l2handle, &fsize);
    if (ret != CMR_CAMERA_SUCCESS) {
      break;
    }
    fsize.index++;
  }

  CMR_LOGD("fsize.index = %d\n", fsize.index);
  if (ret != CMR_CAMERA_SUCCESS) {
    if (!fsize.index) {
      ret = -CMR_CAMERA_FAIL;
      TEST_LOGE("query FrameSizeIntervals fail\n");
    } else if (fsize.index) {
      ret = CMR_CAMERA_SUCCESS;
    }
  }

  return ret;
}

static int parse_param_test(struct context_test *cxt, int argc, char **argv) {
  int i = 0;
  int num = 0;

  if (!cxt) {
    TEST_LOGE("failed: input cxt is null");
    goto exit;
  }

  cxt->width = 640;
  cxt->height = 480;
  cxt->fps = 30;
  cxt->frame_count = FRAME_MIN;
  cxt->dump_count = 0;

  if(argc == 1) {
    TEST_LOGD("highcamera will use default parameters");
  }else if (argc < PARAM_NUM) {
    if (argc > 1 && strcmp(argv[1], "-help") == 0) {
      usage_test();
    } else if (argc > 1 && strcmp(argv[1], "-getfps") == 0){
      Enum_FrameSizeIvals(cxt->fd, cxt->camera_handle);
    } else {
      TEST_LOGE("Input parameter error, please input:[highfpscamera -help] to get help");
    }
    goto exit;
  }

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-w") == 0 && (i < argc - 1)) {
      cxt->width = atoi(argv[++i]);
      if (cxt->width > WIDTH_MAX || (cxt->width % 2)) {
        TEST_LOGE("set width failed, or Out of range");
        goto exit;
      }
    } else if (strcmp(argv[i], "-h") == 0 && (i < argc - 1)) {
      cxt->height = atoi(argv[++i]);
      if (cxt->height > HEIGHT_MAX || (cxt->height % 2)) {
        TEST_LOGE("set height failed, or Out of range");
        goto exit;
      }
    } else if (strcmp(argv[i], "-fps") == 0 && (i < argc - 1)) {
      cxt->fps = atoi(argv[++i]);
      if (cxt->fps > MAX_FPS || cxt->fps < MIN_FPS) {
        TEST_LOGE("set fps failed, Out of range");
        goto exit;
      }
    // } else if (strcmp(argv[i], "-frame_cnt") == 0 && (i < argc - 1)) {
    //   cxt->frame_count = atoi(argv[++i]);
    //   if (cxt->frame_count <= 0 || cxt->frame_count >= FRAME_MAX) {
    //     TEST_LOGE("set frame_count failed, Out of range");
    //     goto exit;
    //   }
    } else if (strcmp(argv[i], "-dump_cnt") == 0 && (i < argc - 1)) {
      cxt->dump_count = atoi(argv[++i]);
      if (cxt->dump_count < 0 || cxt->dump_count >= FRAME_MAX) {
        TEST_LOGE("set dump_count failed, Out of range");
        goto exit;
      }
    }
  }

  if (!cxt->width || !cxt->height || !cxt->fps) {
    TEST_LOGE("parse parameter error");
    goto exit;
  } else {
    TEST_LOGD("parse parameter right");
  }

  // fprintf(stderr, "parameter:\n"
  //                 "\twidth:%d\n"
  //                 "\theight:%d\n"
  //                 "\tfps:%d\n"
  //                 "\tframe_cnt:%d\n"
  //                 "\tdump_cnt:%d\n\n",
  //         cxt->width, cxt->height, cxt->fps, cxt->frame_count, cxt->dump_count);

  return CMR_CAMERA_SUCCESS;

exit:
  return -CMR_CAMERA_FAIL;
}

int main2(int argc, char **argv, int dev, int maxFrame, void *arg) {
  int fd;
  // int dev = 1; // camera id
  char dev_name[11] = "/dev/video";
  char dev_tmp_str[10];
  struct v4l2_fmtdesc fmtdesc;
  struct v4l2_format fmt;
  struct v4l2_streamparm stream_para;
  struct v4l2_capability cap;
  struct v4l2_requestbuffers tV4L2_reqbuf;
  struct pollfd event;
  int i = 0;
  double dt = 0;
  clock_t t0 = 0;
  struct timeval tv11, tv12;

  int ret = CMR_CAMERA_SUCCESS;
  cmr_s64 timestamp = 0, last_timestamp = 0, diff_timestamp;

  char file_name[0x40];
  char tmp_str[10];

  struct context_test cxt;
  struct v4l2_format fmt_test;
  enum v4l2_buf_type v4l2type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  V4l2AdapterInterface *v4l2handle = V4l2Client::create();
  //  std::cout << v4l2handle->name_ << std::endl;

  {
    sprintf(dev_tmp_str, "%d", dev);
    strcat(dev_name, dev_tmp_str);
    CMR_LOGD("dev_name %s.\n", dev_name);
    fd = v4l2handle->open(dev_name, 3);

    // parse param
    memset((void *)&cxt, 0, sizeof(cxt));
    cxt.camera_handle = v4l2handle;
    cxt.fd = fd;
    if (parse_param_test(&cxt, argc, argv) != CMR_CAMERA_SUCCESS) {
      ret = -CMR_CAMERA_FAIL;
      goto EXIT_;
    }

    memset(&tV4L2_reqbuf, 0, sizeof(struct v4l2_requestbuffers));
    if (v4l2handle->ioctl(fd, VIDIOC_QUERYCAP, &cap)) {
      CMR_LOGD("Error opening device : unable to query device.\n");
      ret = -CMR_CAMERA_FAIL;
      goto EXIT_;
    } else {
      CMR_LOGD("driver:\t\t%s\n", cap.driver);
      CMR_LOGD("card:\t\t%s\n", cap.card);
      CMR_LOGD("bus_info:\t%s\n", cap.bus_info);
      CMR_LOGD("version:\t%d\n", cap.version);
      CMR_LOGD("capabilities:\t%x\n", cap.capabilities);

      if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) ==
          V4L2_CAP_VIDEO_CAPTURE) {
        CMR_LOGD("Device : supports capture.\n");
      }

      if ((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING) {
        CMR_LOGD("Device supports streaming.\n");
      }
    }

    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    CMR_LOGD("Support format:\n");
    while (v4l2handle->ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1) {
      CMR_LOGD("\t%d.%s\n", fmtdesc.index + 1, fmtdesc.description);
      fmtdesc.index++;
    }

    fmt_test.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt_test.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
    if (v4l2handle->ioctl(fd, VIDIOC_TRY_FMT, &fmt_test) == -1) {
      CMR_LOGD("not support format RGB32!\n");
    } else {
      CMR_LOGD("support format RGB32\n");
    }

    CMR_LOGD("set fmt...\n");
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    fmt.fmt.pix.pixelformat = IMG_PIX_FMT_NV21;
    // fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;

    // set parameter
    fmt.fmt.pix.width = cxt.width;
    fmt.fmt.pix.height = cxt.height;

    stream_para.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    stream_para.parm.capture.timeperframe.numerator = 1;
    stream_para.parm.capture.timeperframe.denominator = cxt.fps;
    CMR_LOGD("fps:\t\t%d\n", stream_para.parm.capture.timeperframe.denominator);
    if (ret = v4l2handle->ioctl(fd, VIDIOC_S_PARM, &stream_para)) {
      TEST_LOGD("Unable to set parameter\n");
      goto EXIT_;
    }

    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    CMR_LOGD("fmt.type:\t\t%d\n", fmt.type);
    CMR_LOGD("pix.pixelformat:\t%c%c%c%c\n", fmt.fmt.pix.pixelformat & 0xFF,
             (fmt.fmt.pix.pixelformat >> 8) & 0xFF,
             (fmt.fmt.pix.pixelformat >> 16) & 0xFF,
             (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
    CMR_LOGD("pix.height:\t\t%d\n", fmt.fmt.pix.height);
    CMR_LOGD("pix.width:\t\t%d\n", fmt.fmt.pix.width);
    CMR_LOGD("pix.field:\t\t%d\n", fmt.fmt.pix.field);
    if (v4l2handle->ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
      TEST_LOGD("Unable to set format\n");
      ret = -CMR_CAMERA_FAIL;
      goto EXIT_;
    }

    CMR_LOGD("get fmt...\n");
    if (v4l2handle->ioctl(fd, VIDIOC_G_FMT, &fmt) == -1) {
      CMR_LOGD("Unable to get format\n");
      ret = -CMR_CAMERA_FAIL;
      goto EXIT_;
    }
    {
      CMR_LOGD("fmt.type:\t\t%d\n", fmt.type);
      CMR_LOGD("pix.pixelformat:\t%c%c%c%c\n", fmt.fmt.pix.pixelformat & 0xFF,
               (fmt.fmt.pix.pixelformat >> 8) & 0xFF,
               (fmt.fmt.pix.pixelformat >> 16) & 0xFF,
               (fmt.fmt.pix.pixelformat >> 24) & 0xFF);
      CMR_LOGD("pix.height:\t\t%d\n", fmt.fmt.pix.height);
      CMR_LOGD("pix.width:\t\t%d\n", fmt.fmt.pix.width);
      CMR_LOGD("pix.field:\t\t%d\n", fmt.fmt.pix.field);
    }

    tV4L2_reqbuf.count = 8;
    user_buffer = (buffer_type *)calloc(tV4L2_reqbuf.count, sizeof(*user_buffer));

    if (v4l2handle->ioctl(fd, VIDIOC_REQBUFS, &tV4L2_reqbuf)) { //
      CMR_LOGD("alloc reqbuf");
    }

    for (i = 0; i < tV4L2_reqbuf.count; i++) {
      struct v4l2_buffer tV4L2buf;
      memset(&tV4L2buf, 0, sizeof(struct v4l2_buffer));
      tV4L2buf.index = i;

      if (v4l2handle->ioctl(fd, VIDIOC_QUERYBUF, &tV4L2buf)) {
        perror("search");
      }

      user_buffer[i].start = (char *)v4l2handle->mmap(NULL, tV4L2buf.length,
                                                      0, /* access privilege */
                                                      0, /* recommended */
                                                      fd, tV4L2buf.m.offset);

      user_buffer[i].length = tV4L2buf.length;
      CMR_LOGD("succeful user_buffer[%d].start = %p\n", i,
               user_buffer[i].start);
    }

    if (v4l2handle->ioctl(fd, VIDIOC_STREAMON, &v4l2type)) {
      CMR_LOGD("VIDIOC_STREAMON");
    }

    for (i = 0; i < tV4L2_reqbuf.count; i++) {
      struct v4l2_buffer tV4L2buf;
      memset(&tV4L2buf, 0, sizeof(struct v4l2_buffer));
      tV4L2buf.index = i;

      if (v4l2handle->ioctl(fd, VIDIOC_QBUF, &tV4L2buf)) {
        CMR_LOGD("VIDIOC_QBUF");
      }
    }
    event.fd = fd;
    event.events = POLLIN;

    gettimeofday(&tv11,NULL);

    // start stream
    i = 0;
    for (; i < maxFrame && bPlay; i++) {
      	if (v4l2handle->poll(&event, 1, 5000) > 0) {
        	struct v4l2_buffer buf;
        	memset(&buf, 0, sizeof(struct v4l2_buffer));

	        if (v4l2handle->ioctl(fd, VIDIOC_DQBUF, &buf) != 0) {
	          perror("VIDIOC_DQBUF");
	          exit(1);
	        }

	        gipc(cxt.height, cxt.width, (char*)user_buffer[buf.index].start, arg);

	        if (v4l2handle->ioctl(fd, VIDIOC_QBUF, &buf)) {
	          CMR_LOGD("VIDIOC_QBUF");
	        }
      	}
    }

    gettimeofday(&tv12,NULL);
    dt = (tv12.tv_sec*1000 + tv12.tv_usec/1000) - (tv11.tv_sec*1000 + tv11.tv_usec/1000);
    printf("[mbh]V4L2-8541e capture thread end. total frame num = %d, time= %.1f ms, fps= %.1f\n", i, dt, i*1000/dt);

    if (v4l2handle->ioctl(fd, VIDIOC_STREAMOFF, &v4l2type)){
      CMR_LOGD("VIDIOC_STREAMOFF");
    }
    free(user_buffer);
    user_buffer = NULL;

  EXIT_:
    if (v4l2handle->close(fd)) {
      TEST_LOGE("close fail");
    } else {
      CMR_LOGD("close Camera");
    }
  }
  delete v4l2handle;
  return ret;
}

extern "C" int v4l2cap_main(const char* videoName, int maxFrame, imgProcCall ipc, void *arg)
{
	const int argc = 11;
	char  * argv[argc] = {"./highfpscamera", "-w", "640", "-h", "480", "-fps", "120", "-frame_cnt", "0", "-dump_cnt", "0"};

	bPlay = true;
	gipc = ipc;
	
	main2(argc, argv, atoi(videoName), maxFrame, arg);

	return 0;
}

extern "C" int v4l2cap_stop()
{
	bPlay = false;
	return 0;
}