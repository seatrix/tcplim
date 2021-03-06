#include "main.h"

void print_connection(int fd, const char* prologue, const char* epilogue) {
    int peerfd = fdinfo[fd].peerfd;
    
    printf("%s%s:%d -> ",
	   prologue,
	   inet_ntoa(fdinfo[fd].address.sin_addr),
	   ntohs(fdinfo[fd].address.sin_port)
	  );
    printf("%s:%d [%d->%d]",
	   inet_ntoa(fdinfo[peerfd].address.sin_addr),
	   ntohs(fdinfo[peerfd].address.sin_port), 
	   fd, 
	   peerfd);


    if (fdinfo[fd].total_read || fdinfo[peerfd].total_read) {
	printf("%s:%d [%d->%d] %lld:%lld limit=%d:%d nice=%d:%d rate=%d:%d",
	       inet_ntoa(fdinfo[peerfd].address.sin_addr),
	       ntohs(fdinfo[peerfd].address.sin_port), 
	       fd, 
	       peerfd,
	       fdinfo[fd].total_read,
	       fdinfo[peerfd].total_read,
	       fdinfo[fd].speed_limit,
	       fdinfo[peerfd].speed_limit,
	       fdinfo[fd].nice,
	       fdinfo[peerfd].nice,
	       fdinfo[fd].rate,
	       fdinfo[peerfd].rate
	       );
    }
    printf("%s", epilogue);
}

void list_connections() {
    int fd;
    for(fd=0; fd<MAXFD; ++fd) {
	if(fdinfo[fd].status == 0 || fdinfo[fd].status == '.') {
	    continue;
	}
	if(fdinfo[fd].group != 'c') {
	    continue;
	}

	print_connection(fd, "    ", "\n");
    }
}

void process_stdin() {
    dpf("processing stdin\n");
    char cmd[256]={0};
    char arg[256]={0};
    char arg2[256]={0};
    scanf("%256s%256s%256s", cmd, arg, arg2); 
    dpf("    c=\"%s\" arg=\"%s\"\n", cmd, arg);
    switch(cmd[0]) {
	case 'q':
	    exit(0);
	    break;
	case 'l':
	    list_connections();
	    break;
	case 'k':
	    if(!arg[0]) {
                printf("k[ill] fd_number\n");
	    } else {
		close_fd(atoi(arg));
	    }
	    break;
	case 't':
	    if(!arg[0]) {
                printf("timetick number\n");
	    } else {
		timetick = (atoi(arg));
	    }
	    break;
	case 'i':
	    if(!arg[0] || !arg2[0]) {
                printf("i fd_number new_limit\n");
	    } else {
		int fd = (atoi(arg));
		int new_limit = atoi(arg2);
		fdinfo[fd].speed_limit = new_limit;
		quotas_are_full=0;
	    }
	    break;
	case 'd':
	    if(!arg[0] || !arg2[0]) {
                printf("d new_upload_limit new_download_limit\n");
	    } else {
		int upl = atoi(arg);
		int dll = atoi(arg2);
		fd_upload_limit = upl;
		fd_download_limit = dll;
		quotas_are_full=0;
	    }
	    break;
	case 'o':
	    if(!arg[0] || !arg2[0]) {
                printf("o new_upload_total_limit new_download_total_limit\n");
	    } else {
		int upl = atoi(arg);
		int dll = atoi(arg2);
		total_upload_limit = upl;
		total_download_limit = dll;
		quotas_are_full=0;
	    }
	    break;
	case 'r': 
	    {
		int fd;
		for(fd=0; fd<MAXFD; ++fd) {
		    fdinfo[fd].speed_limit = ((fdinfo[fd].group=='c') ? fd_upload_limit : fd_download_limit);
		}
	    }
	    break;
	case 'n':
	    if(!arg[0] || !arg2[0]) {
                printf("nice fd nice\n");
	    } else {
		int fd = atoi(arg);
		int nice = atoi(arg2);
		fdinfo[fd].nice = nice;
		quotas_are_full=0;
	    }
	    break;
	case 'D':
	    debug_output = !debug_output;
	    break;
	case '\0':
	    break;
	default:
	    printf("Commands: Quit List Kill Timetick lImit Default_limits tOtal_limits Reset_all_fds_to_default Nice Debug\n");
    }
}
