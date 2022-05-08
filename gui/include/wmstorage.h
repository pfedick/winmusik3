/*
 * This file is part of WinMusik 3 by Patrick Fedick
 *
 * Copyright (c) 2022 Patrick Fedick
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WMSTORAGE_H_
#define WMSTORAGE_H_

#ifndef WINMUSIK3_H_
#include "winmusik3.h"
#endif
#include <set>

 /*******************************************************
  * Storage Classes                                     *
  *******************************************************/
class CStorage;
class CWMFileChunk;
class CTrackStore;
class CTitleStore;
class DataTitle;
class CTrackList;
class CCallback;
class DataOimp;


class CStorageItem
{
	friend class CStorage;
private:
	uint32_t filepos;
	uint32_t lastchange;
	uint32_t version;
protected:
	uint8_t  formatversion;

public:
	CStorageItem();
	CStorageItem(const CStorageItem& other);
	~CStorageItem();
	void Clear();
	void ClearStorageData();
	void CopyStorageFrom(const CStorageItem& other);
	void CopyStorageFrom(const CWMFileChunk& chunk);
	uint32_t GetVersion();
	uint32_t GetLastChange();
	uint8_t GetFormatVersion();
	void PrintStorageData();
};

class CStorageBase
{
	friend class CStorage;
	CStorage* Storage;
public:
	CStorageBase();
	CStorage& getStorage() const;
	virtual void Clear()=0;
	virtual const char* GetChunkName()=0;
	virtual void LoadChunk(const CWMFileChunk& chunk)=0;
};


class CStorage
{
private:
	ppl7::Mutex Mutex;
	ppl7::String DataPath;
	std::map<int, CStorageBase*> StorageClasses;
	bool	bLoadDatabaseRunning;
	CWMFile	StorageFile;

	void ClearWithoutMutex();

public:
	CStorage();
	virtual ~CStorage();
	void Clear();
	void Init(const ppl7::String& path);
	void RegisterStorageClass(CStorageBase* storageclass);
	CStorageBase* FindStorageClass(const char* name);
	void Save(CStorageBase* type, CStorageItem* item, const ppl7::ByteArrayPtr& bin);
	void Delete(CStorageBase* type, CStorageItem* item);

	void LoadDatabase(CCallback* c=NULL);
	void DeleteDatabase();
	bool isDatabaseLoading() { return bLoadDatabaseRunning; }

	//int CreateDatabase();
};


/*******************************************************
 * TITLES                                              *
 *******************************************************/



class CTitleStore : public CStorageBase
{
private:
	ppl7::Mutex Mutex;
	DataTitle** TitleIndex;
	uint32_t max;
	uint32_t highestId;

	void Increase(uint32_t maxid);
	void SaveToStorage(DataTitle& t);
	DataTitle* SaveToMemory(const DataTitle& t);

public:
	CTitleStore();
	virtual ~CTitleStore();
	virtual const char* GetChunkName();
	virtual void Clear();
	virtual void LoadChunk(const CWMFileChunk& chunk);

	std::set<ppl7::String> Artists;

	uint32_t Put(const DataTitle& title);
	bool Exists(uint32_t id) const;
	void Delete(uint32_t id);
	const DataTitle& Get(uint32_t id) const;
	const DataTitle* GetPtr(uint32_t id) const;
	uint32_t MaxId() const;
};



class DataTitle : public CStorageItem
{
public:
	ppl7::String	Artist;
	ppl7::String	Title;
	ppl7::String	Remarks;
	ppl7::String	Album;
	ppl7::String	Tags;
	ppl7::ByteArray	CoverPreview;
	uint32_t		TitleId;
	uint32_t		DeviceId;
	uint32_t		Length;
	uint32_t		VersionId;
	uint32_t		LabelId;
	uint32_t		BPM;
	uint32_t		RecordDate;
	uint32_t		ReleaseDate;
	uint32_t		ImportData;
	uint32_t		Size;
	uint16_t		RecordSourceId;
	uint16_t		Track;
	uint16_t		Bitrate;
	uint16_t		GenreId;
	uint16_t		RecordDeviceId;
	uint8_t		DeviceType;
	uint8_t		Page;
	uint8_t		Channels;
	uint8_t		Quality;
	uint8_t		Rating;
	uint8_t		Flags;
	uint8_t		Key;
	uint8_t		EnergyLevel;

	DataTitle();
	DataTitle(const DataTitle& other);
	~DataTitle();
	void Clear();

	void CopyFrom(const DataTitle& other);
	void CopyDataFrom(const DataTitle& other);
	void SetTitle(const char* title);
	void SetArtist(const char* artist);
	void SetRemarks(const char* remarks);
	void SetTags(const char* tags);
	void SetAlbum(const char* album);

	void SetTitle(const ppl7::String& title);
	void SetArtist(const ppl7::String& artist);
	void SetRemarks(const ppl7::String& remarks);
	void SetTags(const ppl7::String& tags);
	void SetAlbum(const ppl7::String& album);
	void SetKey(const ppl7::String& key);
	ppl7::String getKeyName(MusicKeyType type) const;

	void Export(ppl7::ByteArray& bin);
	void Import(const ppl7::ByteArrayPtr& bin, int version);

	DataTitle& operator=(const DataTitle& other);
	bool operator==(const DataTitle& other) const;
	bool operator!=(const DataTitle& other) const;

	static ppl7::String keyName(uint8_t id, MusicKeyType type);
	static uint8_t keyId(const ppl7::String& name);
};


/*******************************************************
 * DEVICES                                             *
 *******************************************************/


typedef struct {
	uint32_t		DeviceId;
	uint8_t		DeviceType;
} DATADEVICE;

class DataDevice : public CStorageItem, public ppl6::CTreeItem
{
private:
public:
	ppl7::String	Title;
	ppl7::String	SubTitle;
	uint32_t		DeviceId;
	uint32_t		Length;
	uint32_t		Recorded;
	uint32_t		LabelId;
	uint32_t		PurchaseDate;
	uint32_t		DateCreated;
	uint32_t		FirstDate;
	uint32_t		LastDate;
	uint32_t		NumTracks;
	float			PurchasePrice;
	uint16_t		PurchaseId;
	uint8_t		DeviceType;
	uint8_t		Pages;

	DataDevice();
	virtual ~DataDevice();
	void Clear();

	void CopyFrom(const DataDevice& other);
	void CopyDataFrom(const DataDevice& other);
	void SetTitle(const ppl7::String& title);
	void SetSubTitle(const ppl7::String& subtitle);
	const ppl7::String& GetTitle() const;
	const ppl7::String& GetSubTitle() const;

	void Export(ppl7::ByteArray& bin) const;
	void Import(const ppl7::ByteArrayPtr& bin, int version);
};

class CDeviceStore : public CStorageBase
{
private:
	ppl7::Mutex Mutex;
	std::map<uint64_t, DataDevice>Tree;

	void SaveToStorage(DataDevice& t);
	DataTitle* SaveToMemory(const DataDevice& t);

	uint32_t highest[MAX_DEVICE_TYPES + 1];

public:
	CDeviceStore();
	virtual ~CDeviceStore();
	virtual const char* GetChunkName();
	virtual void Clear();
	virtual void LoadChunk(const CWMFileChunk& chunk);
	int Renumber(uint8_t DeviceType, uint32_t oldId, uint32_t newId);
	uint32_t GetHighestDevice(uint8_t DeviceType) const;
	int Update(uint8_t DeviceType, uint32_t DeviceId);

	uint32_t Put(const DataDevice& entry);
	bool Exists(uint8_t DeviceType, uint32_t DeviceId) const;
	//void Delete(uint8_t DeviceType, uint32_t DeviceId);
	const DataDevice& Get(uint8_t DeviceType, uint32_t DeviceId) const;
	const DataDevice* GetPtr(uint8_t DeviceType, uint32_t DeviceId) const;
	uint32_t MaxId(uint8_t) const;

};

/*******************************************************
 * TRACKS                                              *
 *******************************************************/


class DataTrack : public CStorageItem, public ppl6::CTreeItem
{
private:
	//ppluint64	Key;
public:
	uint32_t	DeviceId;
	uint32_t	TitleId;
	uint16_t	Track;
	uint8_t	Device;
	uint8_t	Page;

	DataTrack();
	virtual ~DataTrack();
	virtual int CompareNode(CTreeItem* item);
	virtual int CompareValue(void* value);

	void Clear();
	int CopyFrom(DataTrack* t);
	int SetValue(uint8_t Device, uint32_t DeviceId, uint8_t Page, uint16_t Track, uint32_t TitleId);
	ppl6::CBinary* Export();
	int Import(ppl6::CBinary* bin, int version);
};


class DataTrackHint : public ppl6::CTreeItem
{
public:
	uint32_t	DeviceId;
	uint8_t	Device;
	uint8_t	Page;
	int			min, max;

	DataTrackHint();
	virtual ~DataTrackHint();
	virtual int CompareNode(CTreeItem* item);
	virtual int CompareValue(void* value);
};

class CTrackStore : public CStorageBase
{
private:
	ppl6::CMutex Mutex;
	ppl6::CTree Tree;
	ppl6::CTree TrackHints;
	int Save(DataTrack* t);
	int UpdateHints(DataTrack* track);
	DataTrackHint* GetHints(uint8_t Device, uint32_t DeviceId, uint8_t Page);

public:
	CTrackStore();
	virtual ~CTrackStore();
	virtual const char* GetChunkName();
	virtual void Clear();
	virtual void LoadChunk(const CWMFileChunk& chunk);
	int Put(DataTrack* entry);
	int Delete(DataTrack* entry);
	DataTrack* Get(uint8_t Device, uint32_t DeviceId, uint8_t Page, uint16_t Track);
	int GetCopy(uint8_t Device, uint32_t DeviceId, uint8_t Page, uint16_t Track, DataTrack* t);
	CTrackList* GetTracklist(uint8_t Device, uint32_t DeviceId, uint8_t Page);

};

/*******************************************************
 * SHORTCUTS                                           *
 *******************************************************/

class DataShortcut : public CStorageItem, public ppl6::CTreeItem
{
	friend class CShortcutStore;
private:
	char* shortcut;
	char* artist;


public:
	DataShortcut();
	DataShortcut(const DataShortcut& other);
	DataShortcut& operator = (const DataShortcut& other);

	virtual ~DataShortcut();
	virtual int CompareNode(CTreeItem* item);
	virtual int CompareValue(void* value);

	void Clear();
	int CopyFrom(const DataShortcut* t);
	int CopyFrom(const DataShortcut& t);
	int SetValue(const char* shortcut, const char* artist);
	const char* GetShortcut();
	const char* GetArtist();
	ppl6::CBinary* Export();
	int Import(ppl6::CBinary* bin, int version);
};

class CShortcutStore : public CStorageBase
{
private:
	ppl6::CMutex Mutex;
	ppl6::CTree Tree;
	int Save(DataShortcut* t);

public:
	CShortcutStore();
	virtual ~CShortcutStore();
	virtual const char* GetChunkName();
	virtual void Clear();
	virtual void LoadChunk(const CWMFileChunk& chunk);
	int Put(DataShortcut* entry);
	DataShortcut* Get(const char* shortcut);
	int Delete(const char* shortcut);
	int GetCopy(const char* shortcut, DataShortcut* t);
	void List();
	void Reset();
	DataShortcut* GetFirst();
	DataShortcut* GetNext();
};


/*******************************************************
 * TABLES                                              *
 *******************************************************/


class CSimpleTable : public CStorageItem, public ppl6::CTreeItem
{
public:
	uint32_t		Id;
	uint32_t		References;
	char* Value;
	CSimpleTable();
	CSimpleTable(const CSimpleTable& other);
	CSimpleTable& operator = (const CSimpleTable& other);
	virtual ~CSimpleTable();
	virtual int CompareNode(CTreeItem* item);
	virtual int CompareValue(void* value);

	void Clear();
	int CopyFrom(const CSimpleTable* t);
	int CopyFrom(const CSimpleTable& other);
	int SetValue(const char* value);
	ppl6::CBinary* Export();
	int Import(ppl6::CBinary* bin, int version);
};

class DataVersion : public CSimpleTable {};
class DataRecordSource : public CSimpleTable {};
class DataLabel : public CSimpleTable {};
class DataPurchaseSource : public CSimpleTable {};
class DataRecordDevice : public CSimpleTable {};
class DataGenre : public CSimpleTable {};

typedef struct {
	CSimpleTable* t;
} TABLE;

class CTableStore : public CStorageBase
{
public:
	typedef std::set<uint32_t> IndexTree;
private:
	ppl6::CMutex Mutex;
	ppl6::CTree Tree;
	TABLE* TableIndex;
	uint32_t max;
	uint32_t highestId;
	int Increase(uint32_t maxid);
	int Save(CSimpleTable* t);
	typedef std::map<ppl7::String, IndexTree >	WordTree;

	void removeFromWordTree(uint32_t id);
	void addToWordTree(uint32_t id);
	void makeUnion(IndexTree& Result, const IndexTree& Tree1, const IndexTree& Tree2);
	void copy(IndexTree& Result, const IndexTree& src);

	WordTree Words;

public:
	CTableStore();
	virtual ~CTableStore();
	virtual const char* GetChunkName();
	virtual void Clear();
	virtual void LoadChunk(const CWMFileChunk& chunk);
	int Put(CSimpleTable* entry);
	CSimpleTable* Get(uint32_t id);
	CSimpleTable* Find(const char* value);
	int GetId(const char* value);
	int FindOrAdd(const char* value);
	int FindAll(ppl6::CWString& value, ppl6::CTree& Result);
	int GetCopy(uint32_t id, CSimpleTable* t);

	uint32_t findWords(IndexTree& tree, const ppl7::String& words);
};

class CVersionStore : public CTableStore
{
public:
	virtual const char* GetChunkName();
};

class CRecordSourceStore : public CTableStore
{
public:
	virtual const char* GetChunkName();
};

class CLabelStore : public CTableStore
{
public:
	virtual const char* GetChunkName();
};

class CPurchaseSourceStore : public CTableStore
{
public:
	virtual const char* GetChunkName();
};

class CRecordDeviceStore : public CTableStore
{
public:
	virtual const char* GetChunkName();
};

class CGenreStore : public CTableStore
{
public:
	virtual const char* GetChunkName();
};

/*******************************************************
 * Original Import Daten                               *
 *******************************************************/

class DataOimp : public CStorageItem
{
public:
	uint32_t			Id;
	ppl7::String		Filename;
	ppl7::AssocArray	ID3v1;
	ppl7::AssocArray	ID3v2;


	DataOimp();
	virtual ~DataOimp();

	void Clear();
	int CopyFrom(const DataOimp* t);
	ppl6::CBinary* Export();
	int Import(ppl6::CBinary* bin, int version);
	int ImportId(ppl6::CBinary* bin, int version);
};


typedef struct {
	DataOimp* t;
} OIMPDATA;

class COimpDataStore : public CStorageBase
{
private:
	ppl6::CMutex Mutex;
	OIMPDATA* TableIndex;
	uint32_t max;
	uint32_t highestId;
	int Increase(uint32_t maxid);
	int Save(DataOimp* t);

public:
	COimpDataStore();
	virtual ~COimpDataStore();
	virtual const char* GetChunkName();
	virtual void Clear();
	virtual void LoadChunk(const CWMFileChunk& chunk);
	int Put(DataOimp* entry);

	int GetCopy(uint32_t id, DataOimp* t);

	void SetHighestImportDataId(uint32_t id);

	void List();
};


#endif	// WMSTORAGE
