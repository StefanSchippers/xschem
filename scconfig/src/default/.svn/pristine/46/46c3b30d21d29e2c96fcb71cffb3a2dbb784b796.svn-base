#ifndef STR_HT_H
#define STR_HT_H

/* char * -> void *  open addressing hashtable */
/* keys and values are strdupped (strcloned) */

#define ht_deleted_key ((char *)1)
#define ht_isused(e) ((e)->key && (e)->key != ht_deleted_key)
#define ht_isempty(e) (((e)->key == NULL) || (e)->key == ht_deleted_key)
#define ht_isdeleted(e) ((e)->key == ht_deleted_key)

typedef struct {
	unsigned int hash;
	char *key;
	void *value;
} ht_entry_t;

typedef struct {
	unsigned int mask;
	unsigned int fill;
	unsigned int used;
	int isstr;
	ht_entry_t *table;
	int refcount;
} ht_t;

ht_t *ht_alloc(int isstr);
void ht_free(ht_t *ht);
ht_t *ht_clear(ht_t *ht);
ht_t *ht_resize(ht_t *ht, unsigned int hint);

/* value of ht[key], NULL if key is empty or deleted */
void *ht_get(ht_t *ht, const char *key);
/* ht[key] = value and return NULL or return ht[key] if key is already used */
void *ht_insert(ht_t *ht, const char *key, void *value);
/* ht[key] = value and return a pointer to the strdupped key */
const char *ht_set(ht_t *ht, const char *key, void *value);
/* delete key and return ht_deleted_key or NULL if key was not used */
const char *ht_del(ht_t *ht, const char *key);

/* iteration */
#define foreach(ht, e) \
	for (e = (ht)->table; e != (ht)->table + (ht)->mask + 1; e++) \
		if (ht_isused(e))

/* first used (useful for iteration) NULL if empty */
ht_entry_t *ht_first(const ht_t *ht);
/* next used (useful for iteration) NULL if there is no more used */
ht_entry_t *ht_next(const ht_t *ht, ht_entry_t *entry);

#endif
