#ifndef __SPRDCAMERA_V4L2_HANDLER_H__
#define __SPRDCAMERA_V4L2_HANDLER_H__
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <poll.h>

struct sensor_reg_info {
    int reg_addr;
    int reg_value;
};

class V4l2AdapterInterface {
  public:
    virtual ~V4l2AdapterInterface(){};
    virtual int open(const char *__path, int __flags) = 0;
    virtual long ioctl(int fd_, unsigned long request, void *argp) = 0;
    virtual void *mmap(void *addr, size_t length, int prot, int flags, int fd,
                       off_t offset) = 0;
    virtual int munmap(void *addr, size_t length) = 0;
    virtual int poll(struct pollfd *fds, nfds_t nfds, int timeout) = 0;
    virtual int close(int __flags) = 0;
    virtual int write_i2c_t(int fd_, void *reg, int count) {
        return this->write_i2c_t(fd_, reg, count);
    };
    virtual int read_i2c(int fd_, unsigned short reg_addr) {
        return this->read_i2c(fd_, reg_addr);
    };

    template<class T,int N> int write_i2c(int fd_, T (&array)[N]){
        int count = 0;
        for(int i=0;i<N;i++) {
            if(!array)
              break;
            if(!((struct sensor_reg_info)array[i]).reg_addr &&
               !((struct sensor_reg_info)array[i]).reg_value)
              continue;
            count++;
        }
        return write_i2c_t(fd_, (void *)array, count);
    };
    const char *name_;
};

class V4l2Client {
  public:
    V4l2Client(const char *name);
    virtual ~V4l2Client() {}

    static V4l2AdapterInterface *create();
    static void registerType(V4l2Client *factory);
    const std::string &name() const { return name_; }

  private:
    virtual V4l2AdapterInterface *createInstance();
    std::string name_;
};

#define REGISTER_V4L2_HANDLER(handler)                                         \
    class handler##Factory : public V4l2Client {                               \
      public:                                                                  \
        handler##Factory() : V4l2Client(#handler) {}                           \
                                                                               \
      private:                                                                 \
        V4l2AdapterInterface *createInstance() { return new handler(); }        \
    };                                                                         \
    static handler##Factory global_##handler##Factory;

#endif /* __LIBCAMERA_PIPELINE_HANDLER_H__ */
