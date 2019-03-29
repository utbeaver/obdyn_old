/*
 * This module implements a hash table class for mapping C/C++ addresses to the
 * corresponding wrapped Python object.
 *
 * Copyright (c) 2004
 * 	Riverbank Computing Limited <info@riverbankcomputing.co.uk>
 * 
 * This file is part of SIP.
 * 
 * This copy of SIP is licensed for use under the terms of the SIP License
 * Agreement.  See the file LICENSE for more details.
 * 
 * SIP is supplied WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */


#include "sip.h"
#include "sipint.h"


#define hash_1(k,s)	(((unsigned long)(k)) % (s))
#define hash_2(k,s)	((s) - 2 - (hash_1((k),(s)) % ((s) - 2)))


/* Prime numbers to use as hash table sizes. */

static unsigned long hash_primes[] = {
	131,        257,        521,        1031,       2053,       4099,
	8209,       16411,      32771,      65537,      131101,     262147,
	524309,     1048583,    2097169,    4194319,    8388617,    16777259,
	33554467,   67108879,   134217757,  268435459,  536870923,  1073741827,
	2147483659U,0
};


static sipHashEntry *newHashTable(unsigned long);
static int findObjectIndex(sipObjectMap *,const void *,unsigned long *);
static void clearValList(sipHashEntry *,int);
static void addVal(sipHashEntry *,sipThisType *);
static void setHashEntry(sipHashEntry *,const void *,sipThisType *);


/*
 * Initialise an object map.
 */

void sipOMInit(sipObjectMap *om)
{
	om -> primeIdx = 0;
	om -> neverUsed = om -> size = hash_primes[om -> primeIdx];
	om -> hash_array = newHashTable(om -> size);
}


/*
 * Finalise an object map.
 */

void sipOMFinalise(sipObjectMap *om)
{
	sipFree((ANY *)om -> hash_array);
}


/*
 * Allocate and initialise a new hash table.
 */

static sipHashEntry *newHashTable(unsigned long size)
{
	sipHashEntry *hashtab, *he;

	hashtab = (sipHashEntry *)sipMalloc(sizeof (sipHashEntry) * size);

	for (he = hashtab; size-- != 0; ++he)
	{
		he -> key = NULL;
		he -> vallist = NULL;
	}

	return hashtab;
}


/*
 * Find the index in the hash table for a C/C++ address.  Return TRUE if it was
 * found.
 */

static int findObjectIndex(sipObjectMap *om,const void *key,unsigned long *idxp)
{
	unsigned long hash, inc;

	hash = hash_1(key,om -> size);
	inc = hash_2(key,om -> size);

	while (om -> hash_array[hash].vallist != NULL)
	{
		if (om -> hash_array[hash].key == key)
		{
			*idxp = hash;
			return TRUE;
		}

		hash = (hash + inc) % om -> size;
	}

	return FALSE;
}


/*
 * Return the wrapped Python object of a specific class for a C/C++ address or
 * NULL if it wasn't found.
 */

sipThisType *sipOMFindObject(sipObjectMap *om,const void *key,PyObject *cls)
{
	unsigned long index;

	if (findObjectIndex(om,key,&index))
	{
		sipThisList *tl;

		/* Go through each wrapped object at this address. */

		for (tl = om -> hash_array[index].vallist; tl != NULL; tl = tl -> next)
		{
			/*
			 * Skip this if the Python instance has gone, but the
			 * wrapped object is still lying around because there
			 * is a reference being held (temporarily) by Python.
			 */

			if (tl -> sipThis -> sipSelf == NULL)
				continue;

			/*
			 * If this wrapped object is of the given class, or a
			 * sub-class of it, or vice versa, then we assume it is
			 * the same C++ object.
			 */

			if (PyClass_IsSubclass(sipGetClass(tl -> sipThis -> sipSelf),cls) ||
			    PyClass_IsSubclass(cls,sipGetClass(tl -> sipThis -> sipSelf)))
				return tl -> sipThis;
		}
	}

	return NULL;
}


/*
 * Return a list of Python objects to the heap and optionally set the C++
 * address in the object to zero.
 */

static void clearValList(sipHashEntry *he,int nullTheCppAddr)
{
	sipThisList *tl = he -> vallist;

	while (tl != NULL)
	{
		sipThisList *next = tl -> next;

		if (nullTheCppAddr)
			tl -> sipThis -> u.cppPtr = NULL;

		if (tl != &he -> first)
			sipFree((ANY *)tl);

		tl = next;
	}

	he -> vallist = NULL;
}


/*
 * Add a Python object to the list for a particular hash table entry.
 */

static void addVal(sipHashEntry *he,sipThisType *val)
{
	sipThisList *tl;

	/*
	 * It should be fairly rare to have more than one element in the list,
	 * so (for efficiency) we don't use the heap for the first one.  We
	 * keep the first one at the head of the list - this is used by the
	 * code that re-sizes the hash table.
	 */

	if (he -> vallist == NULL)
	{
		tl = &he -> first;
		tl -> next = NULL;
		he -> vallist = tl;
	}
	else
	{
		tl = (sipThisList *)sipMalloc(sizeof (sipThisList));
		tl -> next = he -> first.next;
		he -> first.next = tl;
	}

	tl -> sipThis = val;
}


/*
 * Set the contents of an entry in the hash table.
 */

static void setHashEntry(sipHashEntry *he,const void *key,sipThisType *val)
{
	he -> key = key;
	clearValList(he,FALSE);
	addVal(he,val);
}


/*
 * Add a C/C++ address and the corresponding wrapped Python object to the map.
 */

void sipOMAddObject(sipObjectMap *om,const void *key,sipThisType *val)
{
	unsigned long hash, inc, first_free;
	int first_free_invalid;

	hash = hash_1(key,om -> size);
	inc = hash_2(key,om -> size);

	first_free_invalid = TRUE;

	while (om -> hash_array[hash].vallist != NULL)
	{
		/* See if the key is already there. */

		if (om -> hash_array[hash].key == key)
		{
			/*
			 * This can happen for three reasons.  A variable of
			 * one class can be declared at the start of another
			 * class.  Therefore there are two objects, of
			 * different classes, with the same address.  The
			 * second reason that the old C/C++ object has been
			 * deleted by C/C++ but we didn't get to find out for
			 * some reason, and a new C/C++ instance has been
			 * created at the same address.  The third reason is if
			 * we are in the process of deleting a Python object
			 * but the C++ object gets wrapped again because the
			 * C++ dtor called a method that has been
			 * re-implemented in Python.  If we have created this
			 * new C/C++ instance from Python then we know the
			 * second reason is the correct one so we mark the old
			 * pointers as invalid and reuse the entry.  If we are
			 * wrapping an instance created by C/C++ then we don't
			 * know which is the right reason so we guess it is not
			 * the second and just add this one to the existing
			 * list of objects at this address.
			 */

			if (val -> flags & SIP_PY_OWNED)
				clearValList(&om -> hash_array[hash],TRUE);

			addVal(&om -> hash_array[hash],val);

			return;
		}

		/*
		 * Re-use an entry that has been used before rather than use
		 * one that has never been used.
		 */

		if (first_free_invalid && om -> hash_array[hash].key == NULL)
		{
			first_free_invalid = FALSE;
			first_free = hash;
		}

		hash = (hash + inc) % om -> size;
	}

	if (first_free_invalid)
	{
		setHashEntry(&om -> hash_array[hash],key,val);
		om -> neverUsed--;

		/* Grow the hash table if it needs it and we can. */

		if (om -> size >> 3 > om -> neverUsed && hash_primes[om -> primeIdx + 1] != 0)
		{
			unsigned long old_size, i;
			sipHashEntry *ohe, *old_tab;

			old_size = om -> size;
			old_tab = om -> hash_array;

			om -> primeIdx++;
			om -> size = hash_primes[om -> primeIdx];

			om -> hash_array = newHashTable(om -> size);

			om -> neverUsed += om -> size - old_size;

			/*
			 * Transfer the entries from the old table to the new
			 * one.
			 */

			ohe = old_tab;

			for (i = 0; i < old_size; ++i)
			{
				if (ohe -> key != NULL)
				{
					sipHashEntry *nhe;

					/* Find the new entry. */

					hash = hash_1(ohe -> key,om -> size);
					inc = hash_2(ohe -> key,om -> size);

					while (om -> hash_array[hash].key != NULL)
						hash = (hash + inc) % om -> size;

					nhe = &om -> hash_array[hash];

					/* Copy the entry. */

					nhe -> key = ohe -> key;
					nhe -> first = ohe -> first;

					/*
					 * We know that the local storage is at
					 * the head of any list.
					 */

					nhe -> vallist = &nhe -> first;
				}

				++ohe;
			}

			sipFree((ANY *)old_tab);
		}
	}
	else
		setHashEntry(&om -> hash_array[first_free],key,val);
}


/*
 * Remove a C/C++ object from the table.  Return 0 if it was removed
 * successfully.
 */

int sipOMRemoveObject(sipObjectMap *om,const void *key,sipThisType *val)
{
	unsigned long index;

	if (findObjectIndex(om,key,&index))
	{
		sipThisList **tlp, *tl;

		/* See if it stored locally or on the heap. */

		if (om -> hash_array[index].first.sipThis == val)
		{
			/*
			 * If it was the only one then mark the hash entry as
			 * unused.
			 */

			if ((tl = om -> hash_array[index].first.next) == NULL)
				om -> hash_array[index].key = NULL;
			else
			{
				/*
				 * Copy the first entry on the heap to the
				 * local storage and free the heap copy.
				 */

				om -> hash_array[index].first = *tl;
				sipFree((ANY *)tl);
			}

			return 0;
		}

		/* Search the part of the list on the heap. */

		tlp = &om -> hash_array[index].first.next;

		while ((tl = *tlp) != NULL)
		{
			if (tl -> sipThis == val)
			{
				/* Remove it from the list. */

				*tlp = tl -> next;
				sipFree((ANY *)tl);

				return 0;
			}

			tlp = &tl -> next;
		}
	}

	return -1;
}
