/*
 * select.c
 *
 *  Created on: 2015Äê10ÔÂ13ÈÕ
 *      Author: zqf
 */

#include <os.h>

#include <sys/socket.h>

int FD_SET(int fd, fd_set *sets)
{
	int i = 0;
	int result = 0;
	if(NULL != sets)
	{
		for(i =0;i<CN_SELECT_MAXNUM;i++)
		{
			if(sets->fd[i] == CN_INVALID_FD)
			{
				sets->fd[i] = fd;
				result = 1;
				break;
			}
		}
	}

	return result;
}
int FD_CLR(int fd, fd_set *sets)
{

	int i = 0;
	int result = 0;
	if(NULL != sets)
	{
		for(i =0;i<CN_SELECT_MAXNUM;i++)
		{
			if(sets->fd[i] == fd)
			{
				sets->fd[i] = CN_INVALID_FD;
				result = 1;
			}
		}
	}

	return result;
}


int select(int maxfd, fd_set *reads,fd_set *writes, fd_set *exps, \
		   struct timeval *timeout)
{
	int mode = 0;
	int i = 0;
	int fd = -1;
	int result = 0;
	int waittime = 0;


	if(NULL != reads)
	{
		reads->mode |= CN_SOCKET_IOREAD;
		mode |= CN_SOCKET_IOREAD;
	}
	if(NULL != writes)
	{
		writes->mode |= CN_SOCKET_IOWRITE;
		mode |= CN_SOCKET_IOWRITE;
	}
	if(NULL != exps)
	{
		writes->mode |= CN_SOCKET_IOERR;
		mode |= CN_SOCKET_IOERR;
	}

	if(NULL != timeout)
	{
		waittime = timeout->tv_sec*1000 + timeout->tv_usec/1000;
	}
	else
	{
		waittime = CN_SELECT_TIMEDEFAULT;
	}

	while(result == 0)
	{
		if(NULL != reads)
		{
			for(i = 0;i < CN_SELECT_MAXNUM;i++)
			{
				fd = reads->fd[i];
				if(CN_INVALID_FD != fd)
				{
					if(issocketactive(fd,mode))
					{
						result++;
					}
				}
			}
		}
		if(NULL != writes)
		{
			for(i = 0;i < CN_SELECT_MAXNUM;i++)
			{
				fd = writes->fd[i];
				if(-1 != fd)
				{
					if(issocketactive(fd,mode))
					{
						result++;
					}
				}
			}
		}
		if(NULL != exps)
		{
			for(i = 0;i < CN_SELECT_MAXNUM;i++)
			{
				fd = exps->fd[i];
				if(-1 != fd)
				{
					if(issocketactive(fd,mode))
					{
						result++;
					}
				}
			}
		}
		if((result == 0)&&(0 != waittime))//do the delay
		{
			Djy_EventDelay(1*mS);
			waittime--;
		}
		else
		{
			break;
		}
	}
	return result;
}

int FD_ISSET(int fd, fd_set *sets)
{
	int mode;
	int result = 0;

	if(NULL != sets)
	{
		mode = sets->mode;
		result = issocketactive(fd,mode);
	}

	return result;
}

int FD_ZERO(fd_set *sets)
{
	int result = 0;
	int i = 0;
	int fd = CN_INVALID_FD;

	if(NULL != sets)
	{
		sets->mode = 0;
		for (i =0;i < CN_SELECT_MAXNUM ; i++)
		{
			sets->fd[i] = fd;
		}
		result = 1;
	}

	return result;;
}

