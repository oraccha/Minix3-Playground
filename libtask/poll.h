#ifndef _POLL_H
#define _POLL_H

#define POLLIN	0x0001
#define POLLPRI	0x0002
#define POLLOUT	0x0004

struct pollfd {
	int	fd;		/* file descriptor */
	short	events;		/* events to look for */
	short	revents;	/* events returned */
};

typedef unsigned int nfds_t;

extern int poll(struct pollfd fds[], nfds_t nfds, int timeout);

#endif /* _POLL_H */
