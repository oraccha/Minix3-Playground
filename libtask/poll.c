/* Poll(2) emulation functions. */
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/select.h>
#include "poll.h"

int poll(struct pollfd fds[], nfds_t nfds, int timeout)
{
	int i, cc;
	struct timeval tv, *ptv;
	fd_set readfds, writefds, errorfds;
	int maxfd = -1;

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&errorfds);

	/* map the file descriptor sets. */
	for (i = 0; i < nfds; i++) {
		if (fds[i].fd < 0)
			continue;

		if (fds[i].events & POLLIN)
			FD_SET(fds[i].fd, &readfds);
		if (fds[i].events & POLLOUT)
			FD_SET(fds[i].fd, &writefds);
		if (fds[i].events & POLLPRI)
			FD_SET(fds[i].fd, &errorfds);

		if (maxfd < fds[i].fd)
			maxfd = fds[i].fd;
	}

	/* map the timeout. */
	ptv = &tv;
	switch (timeout) {
	case -1: /* block indefinitely. */
		ptv = 0;
		break;
	case 0:  /* return without blocking. */
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		break;
	default:
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;
		break;
	}

	/* do select(). */
	cc = select(maxfd + 1, &readfds, &writefds, &errorfds, ptv);
	if (cc == 0) {
		return 0;
	} else if (cc < 0) {
		if (errno == EAGAIN || errno == EINTR)
			return 0;
		else
			return -1;
	} else {
		for (i = 0; i < nfds; i++) {
			if (fds[i].fd < 0)
				continue;

			fds[i].revents = 0;
			if (fds[i].events & POLLIN &&
			    FD_ISSET(fds[i].fd, &readfds)) {
				fds[i].revents |= POLLIN;
			}
			if (fds[i].events & POLLOUT &&
			    FD_ISSET(fds[i].fd, &writefds)) {
				fds[i].revents |= POLLOUT;
			}
			if (fds[i].events & POLLPRI &&
			    FD_ISSET(fds[i].fd, &errorfds)) {
				fds[i].revents |= POLLPRI;
			}
		}
	}

	return cc;
}

