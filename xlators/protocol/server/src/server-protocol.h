/*
  Copyright (c) 2006, 2007, 2008 Z RESEARCH, Inc. <http://www.zresearch.com>
  This file is part of GlusterFS.

  GlusterFS is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; either version 3 of the License,
  or (at your option) any later version.

  GlusterFS is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see
  <http://www.gnu.org/licenses/>.
*/

#ifndef _SERVER_PROTOCOL_H_
#define _SERVER_PROTOCOL_H_

#ifndef _CONFIG_H
#define _CONFIG_H
#include "config.h"
#endif


#include "glusterfs.h"
#include "xlator.h"
#include "logging.h"
#include "call-stub.h"
#include <pthread.h>
#include "authenticate.h"
#include "fd.h"
#include "byte-order.h"

#define DEFAULT_BLOCK_SIZE     4194304   /* 4MB */
#define GLUSTERFSD_SPEC_PATH   CONFDIR "/glusterfs-client.vol"

typedef struct _server_state server_state_t;

struct _server_reply {
	struct list_head list;
	call_frame_t *frame;
	dict_t *reply;
	dict_t *refs;
	int32_t op;
	int32_t type;
};
typedef struct _server_reply server_reply_t;

struct _server_reply_queue {
	struct list_head list;
	pthread_t thread;
	pthread_mutex_t lock;
	pthread_cond_t cond;
	uint64_t pending;
};
typedef struct _server_reply_queue server_reply_queue_t;

struct _locker {
	struct list_head lockers;
	loc_t loc;
	fd_t *fd;
	pid_t pid;
};

struct _lock_table {
	struct list_head file_lockers;
	struct list_head dir_lockers;
	gf_lock_t lock;
	size_t count;
};

/* private structure per connection (transport object)
 * used as transport_t->xl_private
 */
struct _server_connection_private {
	pthread_mutex_t lock;
	char disconnected;    /* represents a disconnected object, if set */
	fdtable_t *fdtable; 
	struct _lock_table *ltable;
	xlator_t *bound_xl;   /* to be set after an authenticated SETVOLUME */
};

typedef struct {
	server_reply_queue_t *queue;
	int32_t max_block_size;
	int32_t inode_lru_limit;
} server_conf_t;

struct _server_state {
	transport_t *trans;
	xlator_t *bound_xl;
	loc_t loc;
	loc_t loc2;
	int flags;
	fd_t *fd;
	size_t size;
	off_t offset;
	mode_t mode;
	dev_t dev;
	uid_t uid;
	gid_t gid;
	size_t nr_count;
	int cmd;
	int type;
	char *name;
	int name_len;
	inode_table_t *itable;
	ino_t ino;
	ino_t par;
	ino_t ino2;
	ino_t par2;
	char *path;
	char *path2;
	char *bname;
	char *bname2;
	int mask;
	char is_revalidate;
	char need_xattr;
	struct flock flock;
	struct timespec tv[2];
	char *resolved;
};


typedef struct {
	dict_t *auth_modules;
	transport_t *trans;
} server_private_t;

typedef struct _server_connection_private server_connection_private_t;



int32_t
server_stub_resume (call_stub_t *stub,
		    int32_t op_ret,
		    int32_t op_errno,
		    inode_t *inode,
		    inode_t *parent);

int32_t
do_path_lookup (call_stub_t *stub,
		const loc_t *loc);

#endif
