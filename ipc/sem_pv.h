#ifndef __SEM_PV_H__
#define __SEM_PV_H__


//创建并初始化信号量集
//nums 信号量的个数
//value 初始话的值
//成功：返回信号集id 失败：返回-1
extern int i_sem(int nsems, int value);

//对semid信号量集中的索引为semnum的信号量，做取得操作 取得数量为value
extern int p_sem(int semid, int semnum, int value);

//对semid信号量集中的索引为semnum的信号量，做归还操作 归还数量为value
extern int v_sem(int semid, int semnum, int value);

//销毁创建的信号量 否则会一直存在系统中 使用ipcs可查看
extern void d_sem(int semid);

#endif