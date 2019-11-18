/***************************************************************************
frame_memory.h  -  description
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

#ifndef FRAME_MEMORY_H
#define FRAME_MEMORY_H

#include <sys/shm.h>
//#include <fcntl.h>              // for shm_open()
//#include <sys/mman.h>           // for shm_open()
#include <unistd.h>
#include <map>
#include <limits>
#include <stdio.h>
#include "definitions.h"
#include "frame_utils.h"
#include "frame_mutex.h"
#include "thread.h"
const char MemPoolHeaderIdentifier[] = "MemPoolHeader";
extern pid_t mem_id;

class tAbstractMemoryPool {
protected:
	//!Gibt an, ob Memory-Pool von aktuellem Prozess verwaltet wird
	bool _master;
	//!Gr鲞e des Memory-Pools
	long _memsize;
	//!Zeiger auf Start des Memory-Pools
	void* _memptr;
public:
	tAbstractMemoryPool(bool master = false, const string keystr = "", long memsize = CFG_MemPoolStdSize);
	virtual ~tAbstractMemoryPool() {};
	virtual void getmem(void*& memptr, long& memsize) {memptr=_memptr; memsize=_memsize;};
	virtual char* getmembase() {return (char*)_memptr;};
	virtual long getmemsize() {return _memsize;};
};

/**Memory-Pool im Shared-Memory (Wrapper zu shm.h)
*/
class tSharedMemoryPool : public tAbstractMemoryPool 
{
	//!Identifier f黵 Shared-Memory
	int _shmid;
public:
	tSharedMemoryPool(bool master = false, const string keystr = "", long size = CFG_MemPoolStdSize,long shmaddr = 0x0);
	virtual ~tSharedMemoryPool();
};


class tMemoryAllocator_Iterator;
//Forward-Deklarationen
class tMemoryAllocator_LIFO_Iterator;
class tMemoryAllocator_FIFO_Iterator;

/**Klasse zur Verwaltung von Speicherbl鯿ken in einem Memory-Pool
*/
class tMemoryAllocator {
protected:
	typedef unsigned long offsettype;

	//!Header f黵 einen Datenblock im Memory-Pool
	struct tBlockHeader {
		struct {
			offsettype previous;
			offsettype next;
		} memoffset;
		struct {
			offsettype previous;
			offsettype next;
		} keyoffset;
		long key;
		int datasize;
	};

	//!Header f黵 einen Memory-Pool
	struct tMemPoolHeader {
		//!Am Anfang Platz f黵 Memory-Pool-Baseaddress lassen
		void* membase;
		//!Identifikation eines MemoryPools (nur zur Fehlerpr黤ung)
		char identifier[sizeof(MemPoolHeaderIdentifier)];
		struct {
			//!Start des f黵 Blocks zu verwendenen Speicherbereichs
			offsettype start;
			//!Ende des f黵 Blocks zu verwendenen Speicherbereichs
			offsettype end;
		} mem;
		struct {
			//!Erster allokierter Block im Speicher
			offsettype head;
			//!Letzter allokierter Block im Speicher
			offsettype tail;
		} blocks;
		struct {
			//!Erster allokierter Block nach Schl黶seln sortiert
			offsettype head;
			//!Letzter allokierter Block nach Schl黶seln sortiert
			offsettype tail;
		} keys;
	};

	//!Gibt an, ob Speicher von aktuellem Prozess verwaltet wird (f黵 unterliegenden Memory-Pool)
	bool _master;
	//!Memory-Pool, in dem die Bl鯿ke gespeichert werden
	tAbstractMemoryPool* _mempool;
	//!Mutex zur Absicherung gleichzeitigen Zugriffs
	tSystemVSemaphore* _mutex;

public:

	//!Daten f黵 Klasse tKeyHint
	struct tKeyHintStruc {
		tBlockHeader* first;
		tBlockHeader* last;
		bool used;
	};

	//!Klasse zur Beschleunigung des Zugriffs auf Bl鯿ke mit bestimmtem Schl黶sel
	class tKeyHint {
		tKeyHintStruc _data;
	public:
		tKeyHint() {_data.first=NULL; _data.last=NULL; _data.used=false;};
		tKeyHint(tKeyHintStruc keyhintstruc) : _data(keyhintstruc) {};
		tKeyHint(tBlockHeader* f, tBlockHeader* l) {_data.first=f; _data.last=l; _data.used=true;};
		void setfirst(tBlockHeader* first) {_data.first = first;};
		void setlast(tBlockHeader* last) {_data.last = last;};
		tBlockHeader* first() const {return _data.first;};
		tBlockHeader* last() const {return _data.last;};
		bool used() const {return _data.used;};
		tKeyHintStruc value() const {return _data;};
	};

	//!Kurzfassung eines tBlockHeaders mit f黵 Anwendung relevanten Informationen
	class tBlock {
	public:
		tBlockHeader* header;
		void* data;
		long datasize;
		tBlock() : header(NULL), data(NULL), datasize(0) {};
		tBlock(tBlockHeader* _hdr, void* _data = NULL) : header(_hdr), data(_data) {
			if ((data!=NULL)&&(header==NULL)) header=(tBlockHeader*)((char*)data-sizeof(tBlockHeader));
			data=(header==NULL)?NULL:((char*)header+sizeof(tBlockHeader));
			datasize=(header==NULL)?0:header->datasize;
		}
	};

	tMemoryAllocator(bool master = false, tAbstractMemoryPool* mempool = NULL, tSystemVSemaphore* mutex = NULL);
	~tMemoryAllocator();
	string getinfo(const tBlock block);
	string getinfo(offsettype block);
	void dumpmeminfo();
	void dumpblocks();
	string getmeminfo();
	void dumpkeys(const tKeyHint keyhint = tKeyHint());
	void reset();
	//void lock();
	bool lock();
	void unlock();
	tAbstractMutex* getmutex() {return _mutex;};
	bool mempool_isvalid() {return (_mempool->getmembase()!=NULL);};
	offsettype abs2offset(void* pointer) {return (pointer==NULL)?0:((char*)pointer - _mempool->getmembase());};
	void* offset2abs(offsettype offset) {return (offset==0)?NULL:(_mempool->getmembase() + offset);};
	tBlockHeader* offset2header(offsettype offset) {return (offset==0)?NULL:((tBlockHeader*)(_mempool->getmembase() + offset));};
	tBlock malloc(int size, bool bWaring=true);
	tBlock malloc_key(int size, long key);
	tBlock malloc_keyhint(int size, long key, tKeyHintStruc &keyhint);
	void free(const tBlock &block);
	void free_keyhint(const tBlock &block, tKeyHintStruc &keyhint);
	friend class tMemoryAllocator_Iterator;
	tBlock getfirst(const long key, const tKeyHint keyhint = tKeyHint(), bool match = true);
	tBlock getlast(const long key, const tKeyHint keyhint = tKeyHint(), bool match = true);
	tMemoryAllocator_Iterator begin(const long key = 0, const tKeyHint keyhint = tKeyHint(), bool match = true);
	tMemoryAllocator_Iterator end(const long key = 0, const tKeyHint keyhint = tKeyHint(), bool match = true);
	void* getdata(const long key);
};

/**Iterator 黚er Bl鯿ke im Speicher
*/
class tMemoryAllocator_Iterator {
private:
	tMemoryAllocator* _malloc;
	long _key;
	tMemoryAllocator::tBlockHeader* _current;
public:
	tMemoryAllocator_Iterator(tMemoryAllocator* malloc, const long key = 0, tMemoryAllocator::tBlockHeader* current = NULL) : _malloc(malloc), _key(key), _current(current) {_malloc->lock();};
	~tMemoryAllocator_Iterator() {_malloc->unlock();};
	tMemoryAllocator::tBlock current();
	bool next();
	bool previous();
};

/**Adapter zu tMemoryAllocator zur Verwendung mit der STL-Library
*/
template <class T>
class tSTLAllocator {
public:
	//!zu verwendender Memory-Allocator
	tMemoryAllocator* _malloc;
	// Typendefinitionen
	typedef size_t    size_type;
	typedef ptrdiff_t difference_type;
	typedef T*        pointer;
	typedef const T*  const_pointer;
	typedef T&        reference;
	typedef const T&  const_reference;
	typedef T         value_type;
	//!rebind allocator to type U
	template <class U>
	struct rebind {
		typedef tSTLAllocator<U> other;
	};
	//!return address of values
	pointer address(reference value) const {
		return &value;
	}
	//!return address of values (const)
	const_pointer address(const_reference value) const {
		return &value;
	}
	//!standard constructor
	tSTLAllocator() throw() : _malloc(NULL) {};
	//!copy constructor
	tSTLAllocator(const tSTLAllocator& allocator) throw() {setmalloc(allocator.getmalloc());};
	template<class U> tSTLAllocator(const tSTLAllocator<U>& allocator) throw() {_malloc = allocator._malloc;};
	//!destructor
	~tSTLAllocator() throw() {};
	//!return maximum number of elements that can be allocated
	size_type max_size () const throw() {
		return numeric_limits<size_t>::max() / sizeof(T);
	}
	//!allocate but don't initialize num elements of type T
	pointer allocate(size_type num, const_pointer hint = 0) {
		if (hint==NULL) {}
		return (pointer)(_malloc->malloc(num*sizeof(T)).data);
		//original:  return (pointer)(::operator new(num*sizeof(T)));
	}
	//!initialize elements of allocated storage p with value value
	void construct (pointer p, const T& value) {
		//initialize memory with placement new
		new((void*)p)T(value);
	}
	//!destroy elements of initialized storage p
	void destroy(pointer p) {
		//destroy objects by calling their destructor
		p->~T();
	}
	//!deallocate storage p of deleted elements
	void deallocate (pointer p, size_type num) {
		num=0; // damit keine Compilerwarnung
		_malloc->free(tMemoryAllocator::tBlock((tMemoryAllocator::tBlockHeader*)((char*)p - sizeof(tMemoryAllocator::tBlockHeader))));
		//original:  ::operator delete((void*)p);
	}
	//!Setzt Adresse des assoziierten Memory-Allocators; muss vor Verwendung aufgerufen werden
	void setmalloc(tMemoryAllocator* malloc) {_malloc=malloc;};
	//!Gibt Adresse des assoziierten Memory-Allocators zur點k
	tMemoryAllocator* getmalloc() const {return _malloc;};
};


//!return that all specializations of this allocator are interchangeable if underlying allocator matches
template <class T1, class T2>
bool operator== (const tSTLAllocator<T1>& allocator1,
				 const tSTLAllocator<T2>& allocator2) throw() 
{
	return (allocator1._malloc==allocator2._malloc);
}
template <class T1, class T2>
bool operator!= (const tSTLAllocator<T1>& allocator1,
				 const tSTLAllocator<T2>& allocator2) throw() 
{
	return (allocator1._malloc!=allocator2._malloc);
}

//!Vergleichsoperator f黵 Sortierung innerhalb STL-Map etc.
// Darf nicht in Deklaration f黵 Template stehen, weil f黵 alle Allokatoren gleich
struct pid_less : public binary_function<pid_t,pid_t,bool>
{
	bool operator()(const pid_t& __x, const pid_t& __y) const {
		return __x < __y;
	}
};

/**Adapter zu tMemoryAllocator zur Verwendung mit der STL-Library im SharedMemory.
*/
template <class T>
class tSTLShMemAllocator {
public:
	//!STL-Map: Proze?-> Memory-Allocator des Prozesses
	typedef map<pid_t, tMemoryAllocator*, pid_less/*less<pid_t>*/, tSTLShMemAllocator<pid_t> > tProcessMAllocMap;
	typedef tProcessMAllocMap* pProcessMAllocMap;
	//!zu verwendender Memory-Allocator (Map, da abh鋘gig vom Proze?
	pProcessMAllocMap* _mallocmapptr;
	bool _master;
	// Typendefinitionen
	typedef size_t    size_type;
	typedef ptrdiff_t difference_type;
	typedef T*        pointer;
	typedef const T*  const_pointer;
	typedef T&        reference;
	typedef const T&  const_reference;
	typedef T         value_type;
	//!Rebind Allokator zu Typ U
	template<class U>
	struct rebind {
		typedef tSTLShMemAllocator<U> other;
	};
	//!Zur點kliefern der Adresse von Werten
	pointer address(reference value) const {
		return &value;
	}
	//!Zur點kliefern der Adresse von Werten (const)
	const_pointer address(const_reference value) const {
		return &value;
	}
	//add by ganhh
	void ShowAllocator(pProcessMAllocMap *p)
	{	

	}
			
		
	//!Standard Konstruktor
	tSTLShMemAllocator() throw() : _mallocmapptr(NULL), _master(false) {
	};
	//!Copy-Konstruktor
	tSTLShMemAllocator(const tSTLShMemAllocator& allocator) throw() {
		_mallocmapptr = allocator._mallocmapptr;
		_master = false;
	};
	//!Copy-Kontruktor f黵 Allokator f黵 Typ U
	template<class U> tSTLShMemAllocator(const tSTLShMemAllocator<U>& allocator) throw() {
		_mallocmapptr = allocator._mallocmapptr;
		_master = false;
	};
	//!Destruktor
	~tSTLShMemAllocator() throw() {
		if (_master) {
			tProcessMAllocMap* currentmap;
			currentmap = *_mallocmapptr;
			tProcessMAllocMap tempmap;
			//tempor鋜es Mapping im normalen Speicher anlegen
			tempmap = *currentmap;
			*_mallocmapptr = &tempmap;
			//Map im Shared-Memory freigeben
			currentmap->~tProcessMAllocMap();
			getmalloc()->free(tMemoryAllocator::tBlock(NULL, currentmap));
			//Block mit Zeiger im SharedMemory freigeben
			getmalloc()->free(tMemoryAllocator::tBlock(NULL, _mallocmapptr));
		}
	};
	//!Zur點kliefern der maximalen Anzahl von Elementen, die allokiert werden k鰊nen
	size_type max_size () const throw() {
		return numeric_limits<size_t>::max() / sizeof(T);
	}
	//!Allokieren von "num" Elementen vom Typ T, ohne die Elemente zu initialisieren
	pointer allocate(size_type num, const_pointer hint = 0) {
		if (hint==NULL) {}
		if (_mallocmapptr==NULL) {
			pointer p = (pointer)(::operator new(num*sizeof(T)));
			if(NULL==p)
				debugprintunix('E', "operator new erro: ", "tSTLShMemAllocator::allocate");
			//debugprint('I', "operator new memory size:" + int2str(num*sizeof(T))/* + ", type is: " + typeid(T).name() 
			//	*/+ ", pointer addess is: " + ptr2str(p), "tSTLShMemAllocator::allocate");
			return p ;
		} else {
			if (getmalloc()==NULL) {
				debugprint('E', " no memory-allocator for current process found", "tSTLShMemAllocator::allocate");
				return NULL;
			} else {
				return (pointer)(getmalloc()->malloc(num*sizeof(T)).data);
			}
		}
	}
	//!Initialisieren von Elementen, f黵 die bereits der Speicherplatz "p" allokiert worden ist
	void construct (pointer p, const T& value) {
		//Initialisiere mit "placement new"
		new((void*)p)T(value);
	}
	//!Deinitialisieren von Elementen
	void destroy(pointer p) {
		//Aufrufen des Destruktors
		p->~T();
	}
	//!Speicherplatz "p" gel鰏chter Elemente freigeben
	void deallocate(pointer p, size_type num) {
		num=0; //damit keine Compilerwarnung
		if (_mallocmapptr==NULL) {
			//debugprint('I', "operator delete  pointer addess is: " + ptr2str(p), "tSTLShMemAllocator::deallocate");			
			::operator delete((void*)p);
		} else {
			if (getmalloc()==NULL) {
				debugprint('E', " no memory-allocator for current process found", "tSTLShMemAllocator::deallocate");
			} else {
				getmalloc()->free(tMemoryAllocator::tBlock(NULL, p));
			}
		}
	}
	//!Setzt Adresse des assoziierten Memory-Allocators; muss vor Verwendung aufgerufen werden
	void setmalloc(tMemoryAllocator* malloc) {
		//Shared-Memory vor Zugriff durch andere sch黷zen
		//tGuardCurrentScope guard = tGuardCurrentScope(malloc->getmutex());
		tProcessMAllocMap* currentmap=NULL;
		tProcessMAllocMap tempmap;
		if (_mallocmapptr!=NULL) {
			currentmap = *_mallocmapptr;
			//tempmap = **_mallocmapptr;
		} else {
			currentmap = NULL;
		}
	
		debugprint('I', " adding process["+int2str(mem_id)+"] to process map using memory-allocator "+ptr2str(malloc), "tSTLShMemAllocator::setmalloc");
		//neues Paar PID-MAlloc einf黦en
		/*if(currentmap)
		{
			//如果里面的map有问题，用这段代码重建这个map
			(*currentmap).clear();
			delete currentmap;
			currentmap = NULL;
		}*/
		if (currentmap==NULL) 
		{
			for(int i=0; i<15; ++i)
				tempmap[i] = NULL; 

  			  tempmap[mem_id] = malloc;
                        tMemoryAllocator::tBlock memblock = malloc->malloc(sizeof(pProcessMAllocMap), false);
                        _mallocmapptr = (pProcessMAllocMap*)(memblock.data);
                        *_mallocmapptr = &tempmap;
                        //Map im Shared-Memory/Memory-Allocator-Bereich anlegen
                        tProcessMAllocMap* newmap;
                        memblock = malloc->malloc(sizeof(tProcessMAllocMap), false);
                        newmap = new((memblock.data))tProcessMAllocMap(pid_less(), *this);
                        //Daten kopieren und neues Mapping verwenden
                        *newmap = **_mallocmapptr;
                        *_mallocmapptr = newmap;
						
                        _master = true;
                        debugprint('I', "_master = true", __FUNCTION__);	
		} else {

			CSemLock lock((tSystemVSemaphore*)malloc->getmutex());
		        if( !lock.TryLock(-1))
		        {   
		            debugprint('I', " cann't lock the resource for malloc", "tSTLShMemAllocator::setmalloc");
		            return;    //update by ganhh
		       }   
			
			tempmap[mem_id] = malloc;
			*_mallocmapptr = &tempmap;
			(*currentmap)[mem_id] = malloc;
			*_mallocmapptr = currentmap;
		}
		ShowAllocator(_mallocmapptr);
	};
	//!Gibt Adresse des assoziierten Memory-Allocator-Mappings zur點k
	inline tProcessMAllocMap* getmallocmap() const {return *_mallocmapptr;};
	//!Gibt Adresse des assoziierten Memory-Allocators f黵 den aktuellen Proze?zur點k
	inline tMemoryAllocator* getmalloc()// const 
	{
		//ShowAllocator(_mallocmapptr);
		//debugprint('I', " malloc address is : " + ptr2str( (*_mallocmapptr)->find(mem_id)->second ), "tMemoryAllocator::getmalloc");
		return (*_mallocmapptr)->find(mem_id)->second;
	};
};

//!Zur點kliefern, da?alle spezialisierten Allokatoren austauschbar sind, so lange sie die gleiche Memory-Allokator-Map verwenden
template <class T1, class T2>
bool operator== (const tSTLShMemAllocator<T1>& allocator1,
				 const tSTLShMemAllocator<T2>& allocator2) throw() 
{
	return (allocator1._mallocmapptr==allocator2._mallocmapptr);
}

template <class T1, class T2>
bool operator!= (const tSTLShMemAllocator<T1>& allocator1,
				 const tSTLShMemAllocator<T2>& allocator2) throw() 
{
	return (allocator1._mallocmapptr!=allocator2._mallocmapptr);
}

typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef std::map<ULONG, TFileInfo, std::less<ULONG>, tSTLShMemAllocator<TFileInfo> > tFileInfoMap;

#endif

