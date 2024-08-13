#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"
#include <time.h>
#include "stdlib/acl_htable.h"
#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_msg.h"
#include "stdlib/acl_avl.h"
#include "stdlib/acl_cache2.h"

#endif

typedef struct {
	ACL_CACHE2  cache;		/**< ��װ�� ACL_CACHE2 */
	ACL_HTABLE *table;		/**< ��ϣ�����ڰ���ֵ��ѯ */
	acl_avl_tree_t  avl;		/**< ���ڰ�ʱ�������ƽ������� */

	acl_pthread_mutex_t lock;       /**< ������� */
} CACHE;

typedef struct CACHE_INFO CACHE_INFO;

/* ������ͬ����ʱ��ص�Ԫ�ش��������ڵ��� */
typedef struct TREE_NODE {
	CACHE_INFO *head;
	CACHE_INFO *tail;
	acl_avl_node_t  node;

	time_t when_timeout;
} TREE_NODE;

/* ÿ��Ԫ�ص��ڲ���������Ԫ��������һ��ͬʱ������Ԫ�����������ڵ� */
struct CACHE_INFO {
	ACL_CACHE2_INFO info;

	TREE_NODE  *tree_node;
	CACHE_INFO *prev;
	CACHE_INFO *next;
};

static void *cache_iter_head(ACL_ITER *iter, struct ACL_CACHE2 *cache2)
{
	CACHE      *cache = (CACHE*) cache2;
	CACHE_INFO *info;
	TREE_NODE  *node;

	iter->dlen = -1;
	iter->i    = 0;
	iter->size = cache2->size;

	node = acl_avl_first(&cache->avl);
	if (node == NULL) {
		iter->ptr  = NULL;
		iter->data = NULL;
		iter->key  = NULL;
		return iter->ptr;
	}

	iter->ptr = info = node->head;
	acl_assert(info);
	iter->data = ((ACL_CACHE2_INFO *) info)->value;
	iter->key  = ((ACL_CACHE2_INFO *) info)->key;
	return iter->ptr;
}

static void *cache_iter_next(ACL_ITER *iter, struct ACL_CACHE2 *cache2)
{
	CACHE      *cache = (CACHE*) cache2;
	CACHE_INFO *info  = (CACHE_INFO*) iter->ptr;
	TREE_NODE  *node  = info->tree_node;

	info = info->next;
	if (info) {
		iter->ptr  = info;
		iter->data = ((ACL_CACHE2_INFO *) info)->value;
		iter->key  = ((ACL_CACHE2_INFO *) info)->key;
		iter->i++;
		return iter->ptr;
	}

	node = AVL_NEXT(&cache->avl, node);
	if (node == NULL) {
		iter->ptr  = NULL;
		iter->data = NULL;
		iter->key  = NULL;
		return iter->ptr;
	}

	iter->ptr = info = node->head;
	acl_assert(info);
	iter->data = ((ACL_CACHE2_INFO *) info)->value;
	iter->key  = ((ACL_CACHE2_INFO *) info)->key;
	iter->i++;
	return iter->ptr;
}

static void *cache_iter_tail(ACL_ITER *iter, struct ACL_CACHE2 *cache2)
{
	CACHE      *cache = (CACHE*) cache2;
	CACHE_INFO *info;
	TREE_NODE  *node;

	iter->dlen = -1;
	iter->i    = cache2->size - 1;
	iter->size = cache2->size;

	node = acl_avl_last(&cache->avl);
	if (node == NULL) {
		iter->ptr  = NULL;
		iter->data = NULL;
		iter->key  = NULL;
		return iter->ptr;
	}

	iter->ptr = info = node->tail;
	acl_assert(info);
	iter->data = ((ACL_CACHE2_INFO *) info)->value;
	iter->key  = ((ACL_CACHE2_INFO *) info)->key;
	return iter->ptr;
}

static void *cache_iter_prev(ACL_ITER *iter, struct ACL_CACHE2 *cache2)
{
	CACHE      *cache = (CACHE*) cache2;
	CACHE_INFO *info  = (CACHE_INFO*) iter->ptr;
	TREE_NODE  *node = info->tree_node;

	info = info->prev;
	if (info) {
		iter->ptr  = info;
		iter->data = ((ACL_CACHE2_INFO *) info)->value;
		iter->key  = ((ACL_CACHE2_INFO *) info)->key;
		iter->i++;
		return iter->ptr;
	}

	node = AVL_PREV(&cache->avl, node);
	if (node == NULL) {
		iter->ptr  = NULL;
		iter->data = NULL;
		iter->key  = NULL;
		return iter->ptr;
	}

	iter->ptr = info = node->tail;
	acl_assert(info);
	iter->data = ((ACL_CACHE2_INFO *) info)->value;
	iter->key  = ((ACL_CACHE2_INFO *) info)->key;
	iter->i++;
	return iter->ptr;
}

static ACL_CACHE2_INFO *cache_iter_info(ACL_ITER *iter,
	struct ACL_CACHE2 *cache2 acl_unused)
{
	if (iter->ptr == NULL) {
		return NULL;
	}
	return (ACL_CACHE2_INFO*) iter->ptr;
}

/**
 * AVL �õıȽϻص�����
 */
static int avl_cmp_fn(const void *v1, const void *v2)
{
	const TREE_NODE *n1 = (const TREE_NODE*) v1;
	const TREE_NODE *n2 = (const TREE_NODE*) v2;
	time_t ret = n1->when_timeout - n2->when_timeout;

	if (ret < 0) {
		return -1;
	} else if (ret > 0) {
		return 1;
	} else {
		return 0;
	}
}

ACL_CACHE2 *acl_cache2_create(int max_size,
	void (*free_fn)(const ACL_CACHE2_INFO*, void*))
{       
	ACL_CACHE2 *cache2;
	CACHE      *cache;

	cache = (CACHE *) acl_mycalloc(1, sizeof(CACHE));
	cache->table = acl_htable_create(max_size, 0);
	acl_avl_create(&cache->avl, avl_cmp_fn, sizeof(TREE_NODE),
		offsetof(TREE_NODE, node));
	acl_pthread_mutex_init(&cache->lock, NULL);

	cache2            = (ACL_CACHE2*) cache;
	cache2->max_size  = max_size;
	cache2->free_fn   = free_fn;
	cache2->iter_head = cache_iter_head;
	cache2->iter_next = cache_iter_next;
	cache2->iter_tail = cache_iter_tail;
	cache2->iter_prev = cache_iter_prev;
	cache2->iter_info = cache_iter_info;
	return cache2;
}

void acl_cache2_free(ACL_CACHE2 *cache2)
{
	CACHE           *cache = (CACHE*) cache2;
	TREE_NODE       *node;
	ACL_CACHE2_INFO *info2;
	CACHE_INFO      *info;

	if (cache == NULL) {
		return;
	}

	node = (TREE_NODE*) acl_avl_first(&cache->avl);
	while (node) {
		info = node->head;
		node = AVL_NEXT(&cache->avl, node);
		while (info) {
			info2 = (ACL_CACHE2_INFO*) info;
			info  = info->next;
			if (info2->nrefer > 0) {
				acl_msg_warn("%s(%d): del key=%s, nrefer=%d",
					__FUNCTION__, __LINE__,
					info2->key, info2->nrefer);
				info2->nrefer = 0;  /* force to set 0 */
			}
			(void) acl_cache2_delete(cache2, info2);
		}
	}       

	acl_avl_destroy(&cache->avl);
	acl_htable_free(cache->table, NULL);
	acl_pthread_mutex_destroy(&cache->lock);
	acl_myfree(cache);
}

static ACL_CACHE2_INFO *cache2_enter(CACHE *cache, const char *key,
	void *value, int timeout)
{
	CACHE_INFO *info;
	TREE_NODE  *node, iter;
	time_t      when_timeout = time(NULL) + timeout;

	info = (CACHE_INFO*) acl_mycalloc(1, sizeof(CACHE_INFO));

	info->info.value = value;
	info->info.key   = acl_mystrdup(key);
	info->info.cache = &cache->cache;

	/* ����Ԫ�����ӽ���ϣ���У��Ա����ͨ��Ԫ�ؼ�ֵ���в���*/

	if (acl_htable_enter(cache->table, key, info) == NULL) {
		acl_msg_fatal("%s(%d): add key(%s) to htable error(%s)",
			__FUNCTION__, __LINE__, key, acl_last_serror());
	}

	/* �ȴ�ƽ��������в��Ҷ�Ӧʱ��صĽڵ㣬����鵽��Ӧ�ڵ㣬��Ҫ����
	 * Ԫ�ؼ�������ڵ㣬���򴴽��µ����ڵ㲢���Ӹ�Ԫ�������ڵ�
	 */

	iter.when_timeout = timeout > 0 ? when_timeout : 0;
	node = (TREE_NODE*) acl_avl_find(&cache->avl, &iter, NULL);
	if (node == NULL) {
		node = (TREE_NODE*) acl_mycalloc(1, sizeof(TREE_NODE));
		node->when_timeout = iter.when_timeout;
		acl_avl_add(&cache->avl, node);
	}

	/* ��������ͬ����ʱ��ص�Ԫ����˫�������������� */

	if (node->tail == NULL) {
		info->prev = info->next = NULL;
		node->head = node->tail = info;
	} else {
		node->tail->next = info;
		info->prev = node->tail;
		info->next = NULL;
		node->tail = info;
	}

	info->tree_node         = node;
	info->info.when_timeout = node->when_timeout;

	cache->cache.size++;
	return &info->info;
}

ACL_CACHE2_INFO *acl_cache2_enter(ACL_CACHE2 *cache2,
	const char *key, void *value, int timeout)
{
	CACHE      *cache = (CACHE *) cache2;
	CACHE_INFO *info;
	TREE_NODE  *node;

	if (cache == NULL) {
		return NULL;
	}

	info = (CACHE_INFO*) acl_htable_find(cache->table, key);
	if (info != NULL) {
		if (info->info.nrefer > 0) {
			acl_msg_warn("%s(%d): error=exist, key=%s, refer=%d",
				__FUNCTION__, __LINE__, key, info->info.nrefer);
			return NULL;
		}

		if (cache2->free_fn) {
			cache2->free_fn(&info->info, info->info.value);
		}
		info->info.value = value;
		return &info->info;
	}

	/* �����������������Ȳ��ù��ڲ��� */

	if (cache2->max_size > 0 && cache2->size >= cache2->max_size) {
		(void) acl_cache2_timeout(cache2);
	}

	/* ���û���޶����������򻺴���δ��������ֱ�������»������ */
	if (cache2->max_size <= 0 || cache2->size < cache2->max_size) {
		return cache2_enter(cache, key, value, timeout);
	}
	/* else: cache2->max_size > 0 && cache2->size >= cache2->max_size */

	/* ����������Ȼ��������� LRU ����ɾ����ɵ����� */

	node = (TREE_NODE*) acl_avl_first(&cache->avl);
	while (node && cache2->size >= cache2->max_size) {
		if (node->when_timeout == 0) {
			node = AVL_NEXT(&cache->avl, node);
			continue;
		}

		info = node->head;
		node = AVL_NEXT(&cache->avl, node);

		/* ����ɾ��ͬһ����ʱ��ص����ڵ��µĶ��Ԫ�� */

		while (info && cache2->size >= cache2->max_size) {
			ACL_CACHE2_INFO *info2 = (ACL_CACHE2_INFO*) info;

			info = info->next;

			if (info2->nrefer <= 0) {
				(void) acl_cache2_delete(cache2, info2);
			}
		}
	}

	/* �������ػ��Ǵ�����״̬����ֱ�ӷ��ز������������� */
	if (cache2->size >= cache2->max_size) {
		acl_msg_error("%s(%d): size(%d) >= max_size(%d), key=%s",
			__FUNCTION__, __LINE__, cache2->size,
			cache2->max_size, key);
		return NULL;
	}

	return cache2_enter(cache, key, value, timeout);
}

void *acl_cache2_find(ACL_CACHE2 *cache2, const char *key)
{
	CACHE           *cache = (CACHE*) cache2;
	ACL_CACHE2_INFO *info;

	if (cache2 == NULL) {
		return NULL;
	}

	info = (ACL_CACHE2_INFO*) acl_htable_find(cache->table, key);
	if (info != NULL) {
		return info->value;
	} else {
		return NULL;
	}
}

ACL_CACHE2_INFO *acl_cache2_locate(ACL_CACHE2 *cache2, const char *key)
{
	CACHE           *cache = (CACHE*) cache2;
	ACL_CACHE2_INFO *info;

	if (cache2 == NULL) {
		return NULL;
	}

	info = (ACL_CACHE2_INFO*) acl_htable_find(cache->table, key);
	if (info != NULL) {
		return info;
	} else {
		return NULL;
	}
}

int acl_cache2_delete(ACL_CACHE2 *cache2, ACL_CACHE2_INFO *info2)
{
	CACHE_INFO *info  = (CACHE_INFO*) info2;
	TREE_NODE  *node  = info->tree_node;
	CACHE      *cache = (CACHE*) cache2;

	if (cache2 == NULL) {
		return 0;
	}

	if (info2->nrefer > 0) {
		return -1;
	}

	/* �ӹ�ϣ����ɾ����ӦԪ�ؼ��Ķ��������Ԫ�ز����ڣ���ֱ�ӷ��� */
	if (acl_htable_delete(cache->table, info2->key, NULL) < 0) {
		return -1;
	}

	if (info->prev) {
		info->prev->next = info->next;
	} else {
		node->head = info->next;
	}
	if (info->next) {
		info->next->prev = info->prev;
	} else {
		node->tail = info->prev;
	}

	if (cache2->free_fn) {
		cache2->free_fn(info2, info2->value);
	}
	acl_myfree(info2->key);
	acl_myfree(info2);
	cache2->size--;

	/* ��������ͬ����ʱ��صĽڵ����Ԫ��Ϊ��ʱ����ɽ��ýڵ�ɾ�� */
	if (node->head == NULL) {
		acl_avl_remove(&cache->avl, node);
		acl_myfree(node);
	}
	return 0;
}

int acl_cache2_delete2(ACL_CACHE2 *cache2, const char *key)
{
	CACHE           *cache = (CACHE*) cache2;
	ACL_CACHE2_INFO *info2;

	if (cache2 == NULL) {
		return 0;
	}

	info2 = (ACL_CACHE2_INFO*) acl_htable_find(cache->table, key);
	if (info2 == NULL) {
		return -1;
	}
	if (info2->nrefer > 0) {
		return -1;
	}
	return acl_cache2_delete(cache2, info2);
}

int acl_cache2_timeout(ACL_CACHE2 *cache2)
{
	CACHE           *cache = (CACHE*) cache2;
	ACL_CACHE2_INFO *info2;
	CACHE_INFO      *info;
	TREE_NODE       *node, *next;
	time_t now = time(NULL);
	int    n   = 0;

	if (cache2 == NULL) {
		return n;
	}

	node = (TREE_NODE*) acl_avl_first(&cache->avl);
	while (1) {
		if (node == NULL || node->when_timeout > now) {
			break;
		}
		if (node->when_timeout == 0) {
			node = AVL_NEXT(&cache->avl, node);
			continue;
		}
		next = AVL_NEXT(&cache->avl, node);
		info = node->head;
		while (info) {
			info2 = (ACL_CACHE2_INFO*) info;
			info  = info->next;

			if (info2->nrefer > 0) {
				continue;
			}

			if (acl_cache2_delete(cache2, info2) == 0) {
				n++;
			}
		}
		node = next;
	}
	return n;
}

ACL_CACHE2_INFO *acl_cache2_update2(ACL_CACHE2 *cache2,
	ACL_CACHE2_INFO *info2, int timeout)
{
	CACHE      *cache = (CACHE*) cache2;
	CACHE_INFO *info  = (CACHE_INFO*) info2;
	TREE_NODE  *node  = info->tree_node, iter;

	if (cache2 == NULL) {
		acl_msg_error("%s(%d): invalid parameters, max_size=%d",
			__FUNCTION__, __LINE__, cache2 ? cache2->max_size : -1);
		return NULL;
	}

	if (info->prev) {
		info->prev->next = info->next;
	} else {
		node->head = info->next;
	}
	if (info->next) {
		info->next->prev = info->prev;
	} else {
		node->tail = info->prev;
	}

	if (node->head == NULL) {
		acl_avl_remove(&cache->avl, node);
		acl_myfree(node);
	}

	iter.when_timeout = timeout > 0 ? (time(NULL) + timeout) : 0;
	node = (TREE_NODE*) acl_avl_find(&cache->avl, &iter, NULL);
	if (node == NULL) {
		node = (TREE_NODE*) acl_mycalloc(1, sizeof(TREE_NODE));
		node->when_timeout = iter.when_timeout;
		acl_avl_add(&cache->avl, node);
	}

	if (node->tail == NULL) {
		info->prev = info->next = NULL;
		node->head = node->tail = info;
	} else {
		node->tail->next = info;
		info->prev = node->tail;
		info->next = NULL;
		node->tail = info;
	}
	info->tree_node     = node;
	info2->when_timeout = node->when_timeout;
	return info2;
}

ACL_CACHE2_INFO *acl_cache2_update(ACL_CACHE2 *cache2,
	const char *key, int timeout)
{
	CACHE           *cache = (CACHE*) cache2;
	ACL_CACHE2_INFO *info2;

	if (cache2 == NULL) {
		return NULL;
	}

	info2 = (ACL_CACHE2_INFO*) acl_htable_find(cache->table, key);
	if (info2) {
		acl_cache2_update2(cache2, info2, timeout);
		return info2;
	} else {
		return NULL;
	}
}

ACL_CACHE2_INFO *acl_cache2_upsert(ACL_CACHE2 *cache2,
	const char *key, void *value, int timeout, int *exist)
{
	ACL_CACHE2_INFO *info = acl_cache2_update(cache2, key, timeout);

	if (info) {
		if (exist) {
			*exist = 1;
		}
		return info;
	} else {
		if (exist) {
			*exist = 0;
		}
		return acl_cache2_enter(cache2, key, value, timeout);
	}
}

ACL_CACHE2_INFO *acl_cache2_head(ACL_CACHE2 *cache2)
{
	CACHE     *cache = (CACHE *) cache2;
	TREE_NODE *node  = (TREE_NODE *) acl_avl_first(&cache->avl);
	return node ? (ACL_CACHE2_INFO *) node->head : NULL;
}

ACL_CACHE2_INFO *acl_cache2_tail(ACL_CACHE2 *cache2)
{
	CACHE     *cache = (CACHE *) cache2;
	TREE_NODE *node  = (TREE_NODE *) acl_avl_last(&cache->avl);
	return node ? (ACL_CACHE2_INFO *) node->tail : NULL;
}

void acl_cache2_refer(ACL_CACHE2_INFO *info2)
{
	info2->nrefer++;
}

void acl_cache2_refer2(ACL_CACHE2 *cache2, const char *key)
{
	CACHE           *cache = (CACHE*) cache2;
	ACL_CACHE2_INFO *info2;

	if (cache2 == NULL) {
		return;
	}

	info2 = (ACL_CACHE2_INFO*) acl_htable_find(cache->table, key);
	if (info2 != NULL) {
		info2->nrefer++;
	}
}

void acl_cache2_unrefer2(ACL_CACHE2 *cache2, const char *key)
{
	CACHE           *cache = (CACHE*) cache2;
	ACL_CACHE2_INFO *info2;

	if (cache2 == NULL) {
		return;
	}

	info2 = (ACL_CACHE2_INFO*) acl_htable_find(cache->table, key);
	if (info2 != NULL) {
		info2->nrefer--;
	}
}

void acl_cache2_unrefer(ACL_CACHE2_INFO *info2)
{
	info2->nrefer--;
	if (info2->nrefer < 0) {
		acl_msg_warn("%s(%d): key(%s)'s nrefer(%d) invalid",
			__FUNCTION__, __LINE__, info2->key, info2->nrefer);
	}
}

void acl_cache2_lock(ACL_CACHE2 *cache2)
{
	CACHE *cache = (CACHE*) cache2;
	if (cache2 == NULL) {
		return;
	}

	acl_pthread_mutex_lock(&cache->lock);
}

void acl_cache2_unlock(ACL_CACHE2 *cache2)
{
	CACHE *cache = (CACHE*) cache2;
	
	if (cache2 == NULL) {
		return;
	}

	acl_pthread_mutex_unlock(&cache->lock);
}

void acl_cache2_walk(ACL_CACHE2 *cache2,
	void (*walk_fn)(ACL_CACHE2_INFO*, void*), void *arg)
{
	CACHE           *cache = (CACHE*) cache2;
	ACL_CACHE2_INFO *info2;
	CACHE_INFO      *info;
	TREE_NODE       *node;

	if (cache2 == NULL) {
		return;
	}

	node = (TREE_NODE*) acl_avl_first(&cache->avl);
	while (1) {
		if (node == NULL) {
			break;
		}
		info = node->head;
		while (info) {
			info2 = (ACL_CACHE2_INFO*) info;
			walk_fn(info2, arg);
			info  = info->next;
		}
		node = AVL_NEXT(&cache->avl, node);
	}
}

int acl_cache2_clean(ACL_CACHE2 *cache2, int force)
{
	CACHE           *cache = (CACHE*) cache2;
	ACL_CACHE2_INFO *info2;
	CACHE_INFO      *info;
	TREE_NODE       *node;
	int              n = 0;

	if (cache2 == NULL) {
		return 0;
	}

	node = (TREE_NODE*) acl_avl_first(&cache->avl);
	while (node) {
		info = node->head;
		node = AVL_NEXT(&cache->avl, node);
		while (info) {
			info2 = (ACL_CACHE2_INFO*) info;
			info  = info->next;
			if (info2->nrefer > 0 && force == 0) {
				continue;
			}
			if (acl_cache2_delete(cache2, info2) == 0) {
				n++;
			}
		}
	}
	return n;
}

int acl_cache2_size(ACL_CACHE2 *cache2)
{
	if (cache2 == NULL) {
		return 0;
	}
	return cache2->size;
}