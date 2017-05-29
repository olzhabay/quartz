#include <numa.h>
#include <stddef.h>
#include <wchar.h>
#include "malloc.h"
#include "thread.h"
#include "debug.h"
#include "topology.h"


static int64_t nvram_size = 0;
static int64_t total_size = 0;
pthread_mutex_t mutex;

int get_node_id(size_t size, int node_id)
{
    int p = 0;
    pthread_mutex_lock(&mutex);
    if (nvram_size/total_size < hybrid_malloc.rate) {
        p = 1;
    }
    nvram_size += size*p;
    total_size += size;
    pthread_mutex_unlock(&mutex);
    return node_id + p;
}

void *malloc(size_t size)
{
    thread_t *thread = thread_self();
    if (thread == NULL) {
        register_self();
        thread = thread_self();
    }
    if (!thread) {
        DBG_LOG(ERROR, "malloc")
        return NULL;
    }
    int node_id = get_node_id(size, thread->virtual_node->nvram_node->node_id);
    void *ptr = numa_alloc_onnode(size + sizeof(size_t), node_id);
    if (ptr == 0) return 0;
    *(size_t *)ptr = size;
    return ptr + sizeof(size_t);
}

void free(void *ptr)
{
    numa_free(ptr - sizeof(size_t), *(size_t *)(ptr - sizeof(size_t)));
}
