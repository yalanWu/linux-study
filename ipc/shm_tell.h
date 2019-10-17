#ifndef __SHM_TELL_H__
#define __SHM_TELL_H__

//管道初始化
extern int init_pipe();

//利用管道进行等待
extern int wait_pipe();

//利用管道通知
extern int notify_pipe();

//销毁管道
extern void destory_pipe();

//关闭管道一端
extern void close_pipe(int flag);

#endif