/*
    scconfig - hash tables
    Copyright (C) 2007, 2008, 2009 by Szabolcs Nagy

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

		Project page: http://repo.hu/projects/scconfig
		Contact via email: scconfig [at] igor2.repo.hu
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include "libs.h"
#include "ht.h"

#define HT_MINSIZE 8
#define HT_MAXSIZE ((UINT_MAX >> 1U) + 1U)

#define JUMP(i, j) i += j++
#define JUMP_FIRST(i, j) j = 1, i += j++

/* portable str hash */
#define HASH_INIT 0xbabeface
static unsigned int keyhash(const char *key) {
	unsigned int a = HASH_INIT;

	while (*key)
		a += (a << 5) + *key++;
	return a;
}

/* fill threshold = 3/4 */
#define HT_LOG_THRES 2
static void checkfill(ht_t *ht)
{
	if (ht->fill > ht->mask - (ht->mask >> HT_LOG_THRES) || ht->fill > ht->used << 2)
		ht_resize(ht, ht->used << (ht->used > 1U << 15 ? 1 : 2));
}

static ht_t *ht_init(ht_t *ht, int isstr)
{
	ht->mask = HT_MINSIZE - 1;
	ht->fill = 0;
	ht->used = 0;
	ht->isstr = isstr;
	ht->table = calloc(ht->mask + 1, sizeof(ht_entry_t));
	ht->refcount = 1;
	return ht;
}

static ht_t *ht_uninit(ht_t *ht)
{
	ht_entry_t *entry;

	for (entry = ht->table; entry != ht->table + ht->mask + 1; entry++)
		if (ht_isused(entry)) {
			if (ht->isstr)
				free(entry->value);
			free(entry->key);
		}
	free(ht->table);
	return ht;
}

ht_t *ht_alloc(int isstr)
{
	ht_t *ht;

	ht = malloc(sizeof(ht_t));
	return ht_init(ht, isstr);
}

void ht_free(ht_t *ht)
{
	ht_uninit(ht);
	free(ht);
}

ht_t *ht_clear(ht_t *ht)
{
	ht_uninit(ht);
	return ht_init(ht, ht->isstr);
}

/* one lookup function to rule them all */
static ht_entry_t *ht_lookup(ht_t *ht, const char *key, unsigned int hash)
{
	unsigned int mask;
	unsigned int i;
	unsigned int j;
	ht_entry_t *table;
	ht_entry_t *entry;
	ht_entry_t *free_entry;

	mask = ht->mask;
	i = hash;
	table = ht->table;
	assert(ht->table);
	entry = table + (i & mask);
	if (ht_isempty(entry) || !strcmp(entry->key, key))
		return entry;
	/* free_entry: first deleted entry for insert */
	free_entry = ht_isdeleted(entry) ? entry : NULL;
	assert(ht->fill <= ht->mask);
	for (JUMP_FIRST(i, j); ; JUMP(i, j)) {
		entry = table + (i & mask);
		if (ht_isempty(entry))
			return (free_entry == NULL) ? entry : free_entry;
		if (entry->hash == hash && !strcmp(entry->key, key))
			return entry;
		if (ht_isdeleted(entry) && free_entry == NULL)
			free_entry = entry;
	}
}

/* for resize: no deleted entries in ht, entry->key is not in ht, no strdup */
static void cleaninsert(ht_t *ht, const ht_entry_t *entry)
{
	unsigned int i;
	unsigned int j;
	ht_entry_t *newentry;

	i = entry->hash;
	newentry = ht->table + (i & ht->mask);
	if (!ht_isempty(newentry))
		for (JUMP_FIRST(i, j); !ht_isempty(newentry); JUMP(i, j))
			newentry = ht->table + (i & ht->mask);
	++ht->fill;
	++ht->used;
	memcpy(newentry, entry, sizeof(ht_entry_t));
}

ht_t *ht_resize(ht_t *ht, unsigned int hint)
{
	unsigned int newsize;
	unsigned int oldused;
	ht_entry_t *oldtable, *newtable, *entry;

	oldused = ht->used;
	if (hint < oldused << 1)
		hint = oldused << 1;
	assert(hint <= HT_MAXSIZE && hint > oldused);
	for (newsize = HT_MINSIZE; newsize < hint; newsize <<= 1);
	newtable = calloc(newsize, sizeof(ht_entry_t));
	oldtable = ht->table;
	ht->mask = newsize - 1;
	ht->fill = 0;
	ht->used = 0;
	ht->table = newtable;
	for (entry = oldtable; oldused > 0; ++entry)
		if (ht_isused(entry)) {
			--oldused;
			cleaninsert(ht, entry);
		}
	free(oldtable);
	return ht;
}

void *ht_get(ht_t *ht, const char *key)
{
	ht_entry_t *entry;

	entry = ht_lookup(ht, key, keyhash(key));
	return ht_isused(entry) ? entry->value : NULL;
}

void *ht_insert(ht_t *ht, const char *key, void *value)
{
	unsigned int hash;
	ht_entry_t *entry;

	hash = keyhash(key);
	entry = ht_lookup(ht, key, hash);
	if (ht_isused(entry))
		return entry->value;
	if (ht_isempty(entry))
		++ht->fill;
	++ht->used;
	entry->hash = hash;
	entry->key = strclone(key);
	entry->value = ht->isstr ? strclone(value) : value;
	checkfill(ht);
	return NULL;
}

const char *ht_set(ht_t *ht, const char *key, void *value)
{
	unsigned int hash;
	ht_entry_t *entry;
	char *k;

	hash = keyhash(key);
	entry = ht_lookup(ht, key, hash);
	if (ht_isused(entry)) {
		if (ht->isstr) {
			free(entry->value);
			entry->value = strclone(value);
		} else
			entry->value = value;
		return entry->key;
	}
	if (ht_isempty(entry))
		++ht->fill;
	++ht->used;
	entry->hash = hash;
	entry->key = k = strclone(key);
	entry->value = ht->isstr ? strclone(value) : value;
	checkfill(ht);
	return k;
}

const char *ht_del(ht_t *ht, const char *key)
{
	ht_entry_t *entry;

	entry = ht_lookup(ht, key, keyhash(key));
	if (!ht_isused(entry))
		return NULL;
	--ht->used;
	free(entry->key);
	if (ht->isstr)
		free(entry->value);
	entry->key = ht_deleted_key;
	return ht_deleted_key;
}

ht_entry_t *ht_first(const ht_t *ht)
{
	ht_entry_t *entry = 0;

	if (ht->used)
		for (entry = ht->table; !ht_isused(entry); ++entry);
	return entry;
}

ht_entry_t *ht_next(const ht_t *ht, ht_entry_t *entry)
{
	while (++entry != ht->table + ht->mask + 1)
		if (ht_isused(entry))
			return entry;
	return 0;
}
