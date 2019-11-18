// stdafx.cpp : 只包括标准包含文件的源文件
// MonTools.pch 将是预编译头
// stdafx.obj 将包含预编译类型信息

/***************************************************************************
frame_memory.cpp  -  description
-------------------
begin                : Mit Jun 25 2003
copyright            : (C) 2003 by Dirk Henrici
email                : henrici@informatik.uni-kl.de
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of license found in "COPYING".                     *
*                                                                         *
***************************************************************************/

#include "frame_memory.h"
#include <iomanip>

/**Konstruktor
*/
tAbstractMemoryPool::tAbstractMemoryPool(bool master, const string keystr, long memsize) : _master(master), _memsize(memsize) {
	if (keystr=="") {}
}

/**Konstruktor
b tSharedMemoryPool::tSharedMemoryPool(bool, std::string, long, long) 

*/
tSharedMemoryPool::tSharedMemoryPool(bool master, const string keystr, long memsize,long shmaddr) : tAbstractMemoryPool(master, keystr, memsize), _shmid(0) {
	//Deskriptor-Key aus Key-String errechnen
	unsigned long key = crc32(keystr);
	//int addr=0x50000000;
	if (keystr=="")
		debugprint('E',"key-string is missing", "tSharedMemoryPool::tSharedMemoryPool");
	//Sollte SharedMemory-Segment beim Master schon bestehen: freigeben
	if (_master)
		while ((_shmid = shmget(key, memsize, 0)) >= 0)
			if (shmctl(_shmid, IPC_RMID, NULL)==-1)
				debugprintunix('E',"error destroying already exisiting shared memory", "tSharedMemoryPool::tSharedMemoryPool");
	//Shared-Memory anlegen/ansprechen
	//int attrib = (_master)?(IPC_CREAT | 0666):0;
	int attrib = (_master)?( IPC_CREAT|IPC_EXCL|S_IRWXU|S_IRWXO):0;
	if ((_shmid = shmget(key, memsize, attrib)) < 0)
		debugprintunix('E',"error allocating shared memory", "tSharedMemoryPool::tSharedMemoryPool");
	debugprint('N', "shared memory keystr:" + keystr + "key:" + int2str(key) + " id is: "+int2str(_shmid), "tSharedMemoryPool::tSharedMemoryPool");
	//Shared-Memory referenzieren
	if ((_memptr = shmat(_shmid, (void *)shmaddr, 0)) == (char *) -1) {
		debugprintunix('E',"error referencing shared memory key: " + int2str(key) + ",address:" + int2str(shmaddr), "tSharedMemoryPool::tSharedMemoryPool");
		_memptr = NULL;
	} else {
		if (_master) {
			*(void**)_memptr = _memptr;
		} else {
			//ist die automatisch generierte Base-Adresse nicht die gleiche wie beim Master, Shared-Memory entsprechend neu referenzieren
			void* newmembase = *(void**)_memptr;    //why do this .ganhh
			if (newmembase != _memptr) {
				debugprint('I',"membase of master and client doesn't match, redefining client's base", "tSharedMemoryPool::tSharedMemoryPool");
				if (shmdt(_memptr) == -1)
					debugprintunix('E',"error detaching from shared memory", "tSharedMemoryPool::tSharedMemoryPool");
				if ((_memptr = shmat(_shmid, newmembase, 0)) == (char *) -1) {
					debugprintunix('E',"error referencing shared memory (base "+ptr2str(newmembase)+")", "tSharedMemoryPool::tSharedMemoryPool");
					_memptr = NULL;
				}
			}
		}
	}
}

/**Destruktor
*/
tSharedMemoryPool::~tSharedMemoryPool() {
	if (_memptr!=NULL) {
		if (shmdt(_memptr) == -1)
			debugprintunix('E',"error detaching from shared memory", "tSharedMemoryPool::~tSharedMemoryPool");
		if (_master)
			if (shmctl(_shmid, IPC_RMID, NULL)==-1)
				debugprintunix('E',"error destroying shared memory", "tSharedMemoryPool::~tSharedMemoryPool");
	}
}

/**Konstruktor
*Erwartet als Parameter einen Memory-Pool und einen Mutex deren Verwaltung komplett 黚ernommen wird (inkl. L鰏chung)
*/
tMemoryAllocator::tMemoryAllocator(bool master, tAbstractMemoryPool* mempool, tSystemVSemaphore* mutex) : _master(master), _mempool(mempool), _mutex(mutex) {
	//Parameter auf Plausibilit鋞 pr黤en, Checks
	if (mempool==NULL)
		debugprint('E',"no memory-pool for memory-allocator specified", "tMemoryAllocator::tMemoryAllocator");
	if (mutex==NULL)
		debugprint('E',"no lock for memory-allocator specified", "tMemoryAllocator::tMemoryAllocator");
	tMemPoolHeader* pMemPoolHeader = (tMemPoolHeader*)(_mempool->getmembase());
	debugprint('N', "memory-pool-header-address is: "+ptr2str(pMemPoolHeader), "tMemoryAllocator::tMemoryAllocator");
	if (pMemPoolHeader!=NULL) {
		if (master)
			memcpy(pMemPoolHeader->identifier, MemPoolHeaderIdentifier, sizeof(MemPoolHeaderIdentifier));
		if (memcmp(pMemPoolHeader->identifier, MemPoolHeaderIdentifier, sizeof(MemPoolHeaderIdentifier))!=0)
			debugprint('E',"invalid mempool identifier", "tMemoryAllocator::tMemoryAllocator");
		reset();
	}
}

/**Destruktor
*/
tMemoryAllocator::~tMemoryAllocator() {
	if (_mempool->getmembase()!=NULL)
		_mutex->remove();
	delete _mutex;
	delete _mempool;
	debugprint('I',"delete _mempool and _mutex", "tMemoryAllocator::~tMemoryAllocator");
}

/**Liefert einen Info-String 黚er den angefragten Block zur點k
*/
string tMemoryAllocator::getinfo(const tBlock block) {
	return getinfo(abs2offset(block.header));
}

/**Liefert einen Info-String 黚er den durch Zeiger auf Blockheader angefragten Block zur點k
*/
string tMemoryAllocator::getinfo(offsettype block) {
	string info;
	tBlockHeader* header = offset2header(block);
	tMemPoolHeader* pMemPoolHeader = (tMemPoolHeader*)(_mempool->getmembase());
	if (block==0) {
		info="Block is NULL";
	} else {
		info="At "+int2str(block)+" = "+ptr2str(header)+": ";
		info+="previous "+int2str(header->memoffset.previous)+" next "+int2str(header->memoffset.next)+": ";
		info+="previous key "+int2str(header->keyoffset.previous)+" next key "+int2str(header->keyoffset.next)+": ";
		info+=" key value "+int2str(header->key) + " datasize "+int2str(header->datasize);
		tBlock block_body =  tBlock(header);
		info += " body size "+int2str(block_body.datasize);
		info += " body buf " + string((char *)block_body.data);
		void *data=(void*)(header==NULL)?NULL:((char*)header+sizeof(tBlockHeader));
		debugprint('I', string(" bock data is: ") + (char*)data, "getinfo");
		//map_zone_t *pMapZoneInfo = static_cast<map_zone_t*>(data);
		//map_zone_t::iterator* iter = static_cast<map_zone_t::iterator*>(data);
		//if(NULL != iter)
		//	info +=" domain " + dbl2str((*iter)->first);
		//if(pMapZoneInfo != NULL && !pMapZoneInfo->empty())
		{
			//map_zone_t::iterator iter = pMapZoneInfo->begin();
			//info += " domain "+ dbl2str(iter->first);
		}
		if (block == pMemPoolHeader->blocks.head)
			info=info+" head";
		if (block == pMemPoolHeader->blocks.tail)
			info=info+" tail";
		if (block == pMemPoolHeader->keys.head)
			info=info+" keys.head";
		if (block == pMemPoolHeader->keys.tail)
			info=info+" keys.tail";
		if (block == pMemPoolHeader->mem.start)
			info=info+" memstart";
		if (block+sizeof(block)+header->datasize == pMemPoolHeader->mem.end)
			info=info+" memfit";
		if (block < pMemPoolHeader->mem.start)
			info=info+" BUFFER UNDERRUN";
		if (block+sizeof(block)+header->datasize > pMemPoolHeader->mem.end)
			info=info+" BUFFER OVERRUN";
	}
	return info;
}

/**Gibt zum Debuggen Informationen aus dem Header des Memory-Segmentes aus
*/
void tMemoryAllocator::dumpmeminfo() {
	tMemPoolHeader* pMemPoolHeader = (tMemPoolHeader*)(_mempool->getmembase());
	string info=" blocks.head "+int2str(pMemPoolHeader->blocks.head)+" blocks.tail "+int2str(pMemPoolHeader->blocks.tail);
	info+=" keys.head "+int2str(pMemPoolHeader->keys.head)+" keys.tail "+int2str(pMemPoolHeader->keys.tail);
	debugprint('I',info+": baseaddr "+ptr2str(pMemPoolHeader)+" blockmem ("+int2str(pMemPoolHeader->mem.start)+"="+ptr2str(offset2abs(pMemPoolHeader->mem.start))+" to "+int2str(pMemPoolHeader->mem.end)+"="+ptr2str(offset2abs(pMemPoolHeader->mem.end))+")", "tMemoryAllocator::dumpmeminfo");
}

/**Gibt zum Debuggen alle Bl鯿ke in der Reihenfolge, in der sie im Speicher liegen, aus
*/
void tMemoryAllocator::dumpblocks() {
	tMemPoolHeader* pMemPoolHeader = (tMemPoolHeader*)(_mempool->getmembase());
	offsettype currentblock = pMemPoolHeader->blocks.head;
	int blockcount = 0;
	int totalsize = 0;
	debugprint('I',"MemoryAllocator-Memory-Chain: membaseaddr "+ptr2str(pMemPoolHeader)+" blocks ("+int2str(pMemPoolHeader->blocks.head)+"-"+int2str(pMemPoolHeader->blocks.tail)+"):", "tMemoryAllocator::dumpblocks");
	if (currentblock==0)
		debugprint('T',"Memory-Chain is empty");
	while ((currentblock!=pMemPoolHeader->mem.end)&&(currentblock!=0)) {
		debugprint('T',getinfo(currentblock));
		blockcount++;
		totalsize+=offset2header(currentblock)->datasize;
		currentblock = offset2header(currentblock)->memoffset.next;
	}
	debugprint('T', "==> "+int2str(blockcount)+" blocks with "+int2str(totalsize)+" bytes of data.");
}

/**Gibt zum Debuggen alle Bl鯿ke anhand ihrer Schl黶sel aus
*/
void tMemoryAllocator::dumpkeys(const tKeyHint keyhint) 
{
	string info;
	tMemPoolHeader* pMemPoolHeader = (tMemPoolHeader*)(_mempool->getmembase());
	offsettype currentblock = pMemPoolHeader->keys.head;
	debugprint('I',"MemoryAllocator-Key-Chain: membaseaddr "+ptr2str(pMemPoolHeader)+" keys ("+int2str(pMemPoolHeader->keys.head)+"-"+int2str(pMemPoolHeader->keys.tail)+"):", "tMemoryAllocator::dumpkeys");
	if (currentblock==0)
		debugprint('T',"Key-Chain is empty");
	while (currentblock!=0) {
		tBlockHeader* header = offset2header(currentblock);
		info="";
		if (header==keyhint.first())
			info+=" keyhint.first";
		if (header==keyhint.last())
			info+=" keyhint.last";
		debugprint('T',getinfo(currentblock)+info);
		currentblock = header->keyoffset.next;
	}
}

string tMemoryAllocator::getmeminfo()
{
    stringstream info;
    char tmp[10] = "";
    tMemPoolHeader* pMemPoolHeader = (tMemPoolHeader*)(_mempool->getmembase());
    if(pMemPoolHeader == NULL) return "NULL";
    
    sprintf(tmp, "%.2f", ((pMemPoolHeader->blocks.tail+0.0) /pMemPoolHeader->mem.end)*100) ;
    info<<"total: "<<setw(11)<<pMemPoolHeader->mem.end \
          <<" used: " <<setw(11)<<pMemPoolHeader->blocks.tail \
          <<"("<<setw(5)<<tmp<<"%)"\
          <<"   free: "<<setw(11)<<pMemPoolHeader->mem.end - pMemPoolHeader->blocks.tail \
          <<"(byte)"<<ends;

    return info.str();
}


/**initialisiert den Memory-Pool-Header und den Mutex
*/
void tMemoryAllocator::reset() {
	char* memptr = _mempool->getmembase();
	offsettype offset = 0;
	tMemPoolHeader* pMemPoolHeader = (tMemPoolHeader*)memptr;
	//Speicher im Memorypool in Header, Mutex und allokierbaren Speicher aufteilen
	offset+=sizeof(tMemPoolHeader);
	_mutex->connect(memptr+offset);
	offset+=_mutex->getmutexsize();
	_mutex->create();
	//damit MemPool-Header initialisieren, wenn master
	if (_master) {
		pMemPoolHeader->mem.start = offset;
		pMemPoolHeader->mem.end = offset + _mempool->getmemsize();
		pMemPoolHeader->blocks.head = pMemPoolHeader->mem.end;
		pMemPoolHeader->blocks.tail = 0;
		pMemPoolHeader->keys.head = 0;
		pMemPoolHeader->keys.tail = 0;
	}
}

/**Sperrt den assoziierten Mutex
*/
/*void tMemoryAllocator::lock() {
	_mutex->acquire();
}
*/
bool tMemoryAllocator::lock() {
	return (_mutex->wait(5) == ERR_OK);
}

/**Entsperrt den assoziierten Mutex
*/
void tMemoryAllocator::unlock() {
	_mutex->release();
}

/**Allokiert einen Speicherblock angegebener Gr鲞e
*/
tMemoryAllocator::tBlock tMemoryAllocator::malloc(int size, bool bWaring) {
	if (size==0)
		return tBlock(NULL);	
    CSemLock lock(_mutex);
    if( !lock.TryLock(/* (unsigned int)*/-1))
    {   
        debugprint('I', "cannt lock the resource for malloc", "tMemoryAllocator::malloc");
        return tBlock(NULL);    //update by ganhh
   }        
	offsettype newblock = 0;
	offsettype currentblock = 0;
	tBlockHeader* newheader = NULL;
	tBlockHeader* currentheader = NULL;      
        tMemPoolHeader* pMemPoolHeader = (tMemPoolHeader*)(_mempool->getmembase());
        if(bWaring && pMemPoolHeader->mem.end - pMemPoolHeader->blocks.tail <200) //free 200 for next time start the program to use.    
            debugprint('I', "Waring: not enough memory to malloc", "tMemoryAllocator::malloc");

	//M鰃lichst weit vorne freien Speicherbereich suchen...
	if (pMemPoolHeader->mem.start+sizeof(tBlockHeader)+size<=pMemPoolHeader->blocks.head) {
		//freier Platz am Anfang vor erstem Block vorhanden bzw. erster Block?
		newblock = pMemPoolHeader->mem.start;
	}
	else if(pMemPoolHeader->blocks.tail+2*sizeof(tBlockHeader)+offset2header(pMemPoolHeader->blocks.tail)->datasize+size
		<=pMemPoolHeader->mem.end-sizeof(tBlockHeader))
	{
		//debugprint('I', "pMemPoolHeader->mem.end-sizeof(tBlockHeader) is : "+int2str(pMemPoolHeader->mem.end-sizeof(tBlockHeader)), "tMemoryAllocator::malloc");
		currentblock=pMemPoolHeader->blocks.tail;
		newblock=currentblock+sizeof(tBlockHeader)+offset2header(currentblock)->datasize;
	} 
	else 
	{
		//sonst weiter hinten bis zum Ende suchen
		currentblock=pMemPoolHeader->blocks.head;
		while ((currentblock!=pMemPoolHeader->blocks.tail)
			&&(currentblock+2*sizeof(tBlockHeader)+offset2header(currentblock)->datasize+size > offset2header(currentblock)->memoffset.next))
		{
			//debugprint('I', string("current block is: ") + int2str(currentblock), "tMemoryAllocator::malloc");
			currentblock=offset2header(currentblock)->memoffset.next;
		}
		if((currentblock+2*sizeof(tBlockHeader)+offset2header(currentblock)->datasize+size
			<=pMemPoolHeader->mem.end-sizeof(tBlockHeader))
			&&(currentblock+2*sizeof(tBlockHeader)+offset2header(currentblock)->datasize+size<=(offset2header(currentblock)->memoffset.next)))
			newblock=currentblock+sizeof(tBlockHeader)+offset2header(currentblock)->datasize;
	}
	newheader = offset2header(newblock);
	currentheader = offset2header(currentblock);
	//neuen Block verlinken
	if (newblock!=0) {
		if (currentblock==0) {
			newheader->memoffset.next = pMemPoolHeader->blocks.head;
			pMemPoolHeader->blocks.head = newblock;
		} else {
			//debugprint('I', "currentheader currentblock: " + int2str(currentblock) +", datasize: "+int2str(currentheader->datasize), "tMemoryAllocator::malloc");
			int tmp = currentheader->memoffset.next;
			newheader->memoffset.next = tmp;
			currentheader->memoffset.next = newblock;
		}
		if (newheader->memoffset.next!=pMemPoolHeader->mem.end) {
			offset2header(newheader->memoffset.next)->memoffset.previous = newblock;
		} else {
			pMemPoolHeader->blocks.tail = newblock;
		}
		newheader->memoffset.previous = currentblock;
	}
	if (newblock!=0) {
		newheader = offset2header(newblock);
		newheader->keyoffset.previous = 0;
		newheader->keyoffset.next = 0;
		newheader->key = 0;
		newheader->datasize = size;
	} else {
		debugprint('W',"no more free space in memory; have to return NULL-block", "tMemoryAllocator::malloc");
                throw CMemException("Error: not enough memory for alloc", __FILE__, __LINE__);
	}
        //unlock();
	return tBlock(newheader);
}

/**Allokiert einen Speicherblock angegebender Gr鲞e und setzt Schl黶sel
*/
tMemoryAllocator::tBlock tMemoryAllocator::malloc_key(int size, long key) {
	tKeyHintStruc keyhint;
	keyhint.used = false;
	return malloc_keyhint(size, key, keyhint);
}

/**Allokiert einen Speicherblock angegebender Gr鲞e, setzt Schl黶sel und aktualisiert Hint zum schnellen Zugriff
*/
tMemoryAllocator::tBlock tMemoryAllocator::malloc_keyhint(int size, long key, tKeyHintStruc &keyhint) {
	 tBlockHeader* newblock = malloc(size).header;
	 CSemLock lock(dynamic_cast<tSystemVSemaphore*>(_mutex));
         if( !lock.TryLock(/* (unsigned int)*/-1))
         {   
             debugprint('I', "cannt lock the resource for malloc", "tMemoryAllocator::malloc");
             return tBlock(NULL);    //update by ganhh
        }  

	//Speicher allokieren	
	if (newblock!=NULL) {
		offsettype newoffset = abs2offset(newblock);
		newblock->key = key;
		//Key-Kette aktualisieren
		if (key!=0) {
			tMemPoolHeader* pMemPoolHeader = (tMemPoolHeader*)(_mempool->getmembase());
			tBlockHeader* currentblock = end(key, tKeyHint(keyhint), false).current().header;
			//am Anfang einzuf ender Block?
			if (currentblock==NULL) {
				if (pMemPoolHeader->keys.head==0) {
					pMemPoolHeader->keys.tail = newoffset;
				} else {
					offset2header(pMemPoolHeader->keys.head)->keyoffset.previous = newoffset;
					newblock->keyoffset.next = pMemPoolHeader->keys.head;
				}
				pMemPoolHeader->keys.head = newoffset;
				keyhint.first = newblock;
			} else {
				//sonst hinter angegebenem Block einf黦en
				if (currentblock->key!=key)
					keyhint.first = newblock;
				newblock->keyoffset.next = currentblock->keyoffset.next;
				currentblock->keyoffset.next = newoffset;
				if (newblock->keyoffset.next!=0) {
					offset2header(newblock->keyoffset.next)->keyoffset.previous = newoffset;
				} else {
					pMemPoolHeader->keys.tail = newoffset;
				}
				newblock->keyoffset.previous = abs2offset(currentblock);
			}
			keyhint.last = newblock;
			keyhint.used = true;
		}
	}
	return newblock;
}

/**Gibt einen Speicherblock frei
*/
void tMemoryAllocator::free(const tBlock &block) {
        CSemLock lock(_mutex);
        if( !lock.TryLock(-1))
         {   
             debugprint('I', "cannt lock the resource for malloc", "tMemoryAllocator::malloc");
             return ;    //update by ganhh
        }  

	tMemPoolHeader* pMemPoolHeader = (tMemPoolHeader*)(_mempool->getmembase());
	tBlockHeader* currentblock = block.header;
	if (currentblock!=NULL) {
		//Key-Kette aktualisieren
		//n鋍hsten Block in Kette bearbeiten
		if (currentblock->keyoffset.next!=0) {
			offset2header(currentblock->keyoffset.next)->keyoffset.previous = currentblock->keyoffset.previous;
		} else {
			if (currentblock->key!=0) {
				pMemPoolHeader->keys.tail = currentblock->keyoffset.previous;
			}
		}
		//vorherigen Block in Kette bearbeiten
		if (currentblock->keyoffset.previous!=0) {
			offset2header(currentblock->keyoffset.previous)->keyoffset.next = currentblock->keyoffset.next;
		} else {
			if (currentblock->key!=0) {
				pMemPoolHeader->keys.head = currentblock->keyoffset.next;
			}
		}
		//Mem-Kette aktualisieren
		if (currentblock->memoffset.previous==0) {
			pMemPoolHeader->blocks.head = currentblock->memoffset.next;
		} else {
			offset2header(currentblock->memoffset.previous)->memoffset.next = currentblock->memoffset.next;
		}
		if (currentblock->memoffset.next==pMemPoolHeader->mem.end) {
			//letzter Block? Tail anpassen
			pMemPoolHeader->blocks.tail = currentblock->memoffset.previous;
		} else {
			//sonst Zeiger auf vorherigen Block des n鋍hsten Blocks anpassen
			offset2header(currentblock->memoffset.next)->memoffset.previous = currentblock->memoffset.previous;
		}
	} else {
		debugprint('W',"tried to free NULL-block", "tMemoryAllocator::free");
	}
}

/**Gibt einen Speicherblock frei, aktualisiert Hint
*/
void tMemoryAllocator::free_keyhint(const tBlock &block, tKeyHintStruc &keyhint) {
	CSemLock lock(dynamic_cast<tSystemVSemaphore*>(_mutex));
        if( !lock.TryLock(/* (unsigned int)*/-1))
        {   
            debugprint('I', "cannt lock the resource for malloc", "tMemoryAllocator::malloc");
            return ;    //update by ganhh
       }  
	//Hint aktualisieren
	tBlockHeader* currentblock = block.header;
	if (currentblock!=NULL) {
		//keyhint.last bearbeiten
		if (keyhint.last==currentblock) {
			keyhint.last = offset2header(currentblock->keyoffset.previous);
			if (keyhint.last!=0)
				if (keyhint.last->key!=currentblock->key)
					keyhint.last = 0;
		}
		//keyhint.first bearbeiten
		if (keyhint.first==currentblock) {
			keyhint.first = offset2header(currentblock->keyoffset.next);
			if (keyhint.first!=0)
				if (keyhint.first->key!=currentblock->key)
					keyhint.first = 0;
		}
	}
	//Block 黚er normale Methode freigeben
	free(block);
}

/**Liefert den ersten Block mit "key" zur點k, der Hint erm鰃licht optional direkten Zugriff
*Ist "match" true, wird NULL zur點kgeliefert, wenn das Element nicht gefunden wird, bei false das n鋍hste Element.
*/
tMemoryAllocator::tBlock tMemoryAllocator::getfirst(const long key, const tKeyHint keyhint, bool match) 
{
	tBlockHeader* current = NULL;
	if(_mempool == NULL) return NULL;//add by ganhh

	tMemPoolHeader* pMemPoolHeader = (tMemPoolHeader*)(_mempool->getmembase());
	if (key!=0) 
        {
		if (keyhint.used()) {
			//ggf. keyhint nutzen
			current = keyhint.first();
			if (current!=NULL)
				if (keyhint.first()->key!=key)
					debugprint('E',"keyhint doesn't match key", "tMemoryAllocator::begin");
		} else {
			//sonst mu?gesucht werden...
			//vorne anfangen
			current = offset2header(pMemPoolHeader->keys.head);
			//alle Bl鯿ke entlang Keykette durchgehen bis Ende oder gefunden
			while ((current!=NULL)&&(current->key < key))
				current = offset2header(current->keyoffset.next);
			//wenn nicht gefunden, bei "match" Iterator nullen
			if (current!=NULL)
				if (match)
					if (current->key!=key)
						current = NULL;
		}
	} 
    else 
        {
		if (pMemPoolHeader->blocks.head != pMemPoolHeader->mem.end)
			current = offset2header(pMemPoolHeader->blocks.head);
	}
	return current;
}

/**Liefert den letzten Block mit "key" zur點k, der Hint erm鰃licht optional direkten Zugriff
*Ist "match" true, wird NULL zur點kgeliefert, wenn das Element nicht gefunden wird, bei false das n鋍hste Element.
*/
tMemoryAllocator::tBlock tMemoryAllocator::getlast(const long key, const tKeyHint keyhint, bool match) {
	tBlockHeader* current = NULL;
	tMemPoolHeader* pMemPoolHeader = (tMemPoolHeader*)(_mempool->getmembase());
	if (key!=0) {
		if (keyhint.used()) {
			//ggf. keyhint nutzen
			current = keyhint.last();
			if (keyhint.last()->key!=key)
				debugprint('E',"keyhint doesn't match key", "tMemoryAllocator::end");
		} else {
			//sonst mu?gesucht werden...
			//hinten anfangen
			current = offset2header(pMemPoolHeader->keys.tail);
			//alle Bl鯿ke entlang Keykette durchgehen bis Ende oder gefunden
			while ((current!=NULL)&&(current->key > key))
				current = offset2header(current->keyoffset.previous);
			//wenn nicht gefunden, bei "match" Iterator nullen
			if (current!=NULL)
				if (match)
					if (current->key!=key)
						current = NULL;
		}
	} else {
		current = offset2header(pMemPoolHeader->blocks.tail);
	}
	return current;
}

/**Liefert einen Iterator zur點k, der auf das erste Element mit "key" verweist, der Hint erm鰃licht optional direkten Zugriff
*Ist "match" true, wird NULL zur點kgeliefert, wenn das Element nicht gefunden wird, bei false das n鋍hste Element.
*/
tMemoryAllocator_Iterator tMemoryAllocator::begin(const long key, const tKeyHint keyhint, bool match) {
	return tMemoryAllocator_Iterator(this, key, getfirst(key, keyhint, match).header);
}

/**Liefert einen Iterator zur點k, der auf das letzte Element mit "key" verweist, der Hint erm鰃licht optional direkten Zugriff
*Ist "match" true, wird NULL zur點kgeliefert, wenn das Element nicht gefunden wird, bei false das n鋍hste Element.
*/
tMemoryAllocator_Iterator tMemoryAllocator::end(const long key, const tKeyHint keyhint, bool match) {
	return tMemoryAllocator_Iterator(this, key, getlast(key, keyhint, match).header);
}

/**Liefert die Daten des ersten Blocks mit "key" zur點k
*/
void* tMemoryAllocator::getdata(const long key) {
	return getfirst(key).data;
}

/**Gibt den Block, auf den der Iterator gerade verweist, zur點k
*/
tMemoryAllocator::tBlock tMemoryAllocator_Iterator::current() {
	return tMemoryAllocator::tBlock(_current);
}

/**Setzt Iterator auf den nachfolgenden Block, sofern vorhanden.
*Ein key-aware Iterator verwendet die Key-Kette, sonst wird die MemBlock-Kette verwendet
*/
bool tMemoryAllocator_Iterator::next() {
	if (_current!=NULL) {
		if (_key!=0) {
			if (_current->keyoffset.next!=0) {
				if (_malloc->offset2header(_current->keyoffset.next)->key==_key) {
					_current = _malloc->offset2header(_current->keyoffset.next);
					return true;
				}
			}
		} else {
			tMemoryAllocator::tMemPoolHeader* pMemPoolHeader = (tMemoryAllocator::tMemPoolHeader*)(_malloc->_mempool->getmembase());
			if (_current->memoffset.next!=pMemPoolHeader->blocks.head) {
				_current = _malloc->offset2header(_current->memoffset.next);
				return true;
			}
		}
	}
	return false;
}

/**Setzt Iterator auf den vorherigen Block, sofern vorhanden.
*Ein key-aware Iterator verwendet die Key-Kette, sonst wird die MemBlock-Kette verwendet
*/
bool tMemoryAllocator_Iterator::previous() {
	if (_current!=NULL) {
		if (_key!=0) {
			if (_current->keyoffset.previous!=0) {
				if (_malloc->offset2header(_current->keyoffset.previous)->key==_key) {
					_current = _malloc->offset2header(_current->keyoffset.previous);
					return true;
				}
			}
		} else {
			if (_current->memoffset.previous!=0) {
				_current = _malloc->offset2header(_current->memoffset.previous);
				return true;
			}
		}
	}
	return false;
}


