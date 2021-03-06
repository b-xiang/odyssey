#ifndef ODYSSEY_SERVER_H
#define ODYSSEY_SERVER_H

/*
 * Odyssey.
 *
 * Scalable PostgreSQL connection pooler.
*/

typedef struct od_server od_server_t;

typedef enum
{
	OD_SERVER_UNDEF,
	OD_SERVER_IDLE,
	OD_SERVER_ACTIVE,
	OD_SERVER_EXPIRE
} od_server_state_t;

struct od_server
{
	od_server_state_t  state;
	od_id_t            id;
	machine_io_t      *io;
	machine_tls_t     *tls;
	od_packet_t        packet_reader;
	int                is_allocated;
	int                is_transaction;
	int                is_copy;
	int                deploy_sync;
	od_stat_state_t    stats_state;
	uint64_t           sync_request;
	uint64_t           sync_reply;
	int                idle_time;
	kiwi_key_t         key;
	kiwi_key_t         key_client;
	od_id_t            last_client_id;
	machine_msg_t     *error_connect;
	void              *client;
	void              *route;
	od_global_t       *global;
	od_list_t          link;
};

static inline void
od_server_init(od_server_t *server)
{
	server->state          = OD_SERVER_UNDEF;
	server->route          = NULL;
	server->client         = NULL;
	server->global         = NULL;
	server->io             = NULL;
	server->tls            = NULL;
	server->idle_time      = 0;
	server->is_allocated   = 0;
	server->is_transaction = 0;
	server->is_copy        = 0;
	server->deploy_sync    = 0;
	server->sync_request   = 0;
	server->sync_reply     = 0;
	server->error_connect  = NULL;
	od_stat_state_init(&server->stats_state);
	kiwi_key_init(&server->key);
	kiwi_key_init(&server->key_client);
	od_packet_init(&server->packet_reader);
	od_list_init(&server->link);
	memset(&server->id, 0, sizeof(server->id));
	memset(&server->last_client_id, 0, sizeof(server->last_client_id));
}

static inline od_server_t*
od_server_allocate(void)
{
	od_server_t *server = malloc(sizeof(*server));
	if (server == NULL)
		return NULL;
	od_server_init(server);
	server->is_allocated = 1;
	return server;
}

static inline void
od_server_free(od_server_t *server)
{
	if (server->is_allocated)
		free(server);
}

static inline void
od_server_sync_request(od_server_t *server, uint64_t count)
{
	server->sync_request += count;
}

static inline void
od_server_sync_reply(od_server_t *server)
{
	server->sync_reply++;;
}

static inline int
od_server_synchronized(od_server_t *server)
{
	return server->sync_request == server->sync_reply;
}

#endif /* ODYSSEY_SERVER_H */
