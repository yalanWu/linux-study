#ifndef __SHM_ACCOUNT__
#define __SHM_ACCOUNT__


//创建账户结构体
typedef struct 
{
	int code;//账户编号
	double balance;//账户余额
}SHMACOUNT;

//取款
extern double withdraw(SHMACOUNT* a, double amt);

//存款
extern double deposit(SHMACOUNT* a, double amt);

//查看余额
extern double get_balance(const SHMACOUNT* a);

#endif