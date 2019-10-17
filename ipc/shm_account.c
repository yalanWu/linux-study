#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <assert.h>
#include "shm_account.h"

//取款
double withdraw(SHMACOUNT* a, double amt)
{
	assert(a != NULL);
	double balance = a->balance;
	sleep(1);
	if (balance >= amt)
	{
		balance -= amt;
		a->balance = balance;
		return amt;
	}
	
	return 0;
}

//存款
double deposit(SHMACOUNT* a, double amt)
{
	assert(a != NULL);
	double balance = a->balance;
	sleep(1);
	balance += amt;
	a->balance = balance;
	return balance;
}

//查看余额
double get_balance(const SHMACOUNT* a)
{
	assert(a != NULL);
	double balance = a->balance;
	return balance;
}