/******************
CMemoryPool.cpp
******************/

/*!\file CMemoryPool.cpp
 * \brief CMemoryPool implementation.
 */

#include "BasicIncludes.h"
#include "SMemoryChunk.h"
#include "CMemoryPool.h"
#include "../../../Global/Global.h"
#include "AVX/AVXAllocate/IAVXAllocate.h"

/******************
Constructor
******************/
bool CMemoryPool::InitMemoryPool(const std::size_t &sInitialMemoryPoolSize,
	const std::size_t &sMemoryChunkSize,
	const std::size_t &sMinimalMemorySizeToAllocate,
	bool bSetMemoryData)
{
	
	return true;
}
CMemoryPool::CMemoryPool()
{

}
/******************
Destructor
******************/
CMemoryPool::~CMemoryPool()
{

}
string CMemoryPool::GetLastError()
{
	return m_StrError;
}
/******************
GetMemory
******************/
void *CMemoryPool::GetMemory(const std::size_t &sMemorySize)
{
	void *p = CAVXMemAllocate::New(sMemorySize);
	
	return p;
}

/******************
FreeMemory
******************/
void CMemoryPool::FreeMemory(void *ptrMemoryBlock, const std::size_t &sMemoryBlockSize)
{
	CAVXMemAllocate::Delete(ptrMemoryBlock);
}

/******************
AllocateMemory
******************/
bool CMemoryPool::AllocateMemory(const std::size_t &sMemorySize)
{
	// This function will allocate *at least* "sMemorySize"-Bytes from the Operating-System.

	// How it works :
	// Calculate the amount of "SMemoryChunks" needed to manage the requested MemorySize.
	// Every MemoryChunk can manage only a certain amount of Memory
	// (set by the "m_sMemoryChunkSize"-Member of the Memory-Pool).
	//
	// Also, calculate the "Best" Memory-Block size to allocate from the 
	// Operating-System, so that all allocated Memory can be assigned to a
	// Memory Chunk.
	// Example : 
	//	You want to Allocate 120 Bytes, but every "SMemoryChunk" can only handle
	//    50 Bytes ("m_sMemoryChunkSize = 50").
	//    So, "CalculateNeededChunks()" will return the Number of Chunks needed to
	//    manage 120 Bytes. Since it is not possible to divide 120 Bytes in to
	//    50 Byte Chunks, "CalculateNeededChunks()" returns 3.
	//    ==> 3 Chunks can Manage 150 Bytes of data (50 * 3 = 150), so
	//        the requested 120 Bytes will fit into this Block.
	//    "CalculateBestMemoryBlockSize()" will return the amount of memory needed
	//    to *perfectly* subdivide the allocated Memory into "m_sMemoryChunkSize" (= 50) Byte
	//    pieces. -> "CalculateBestMemoryBlockSize()" returns 150.
	//    So, 150 Bytes of memory are allocated from the Operating-System and
	//    subdivided into 3 Memory-Chunks (each holding a Pointer to 50 Bytes of the allocated memory).
	//    Since only 120 Bytes are requested, we have a Memory-Overhead of 
	//    150 - 120 = 30 Bytes. 
	//    Note, that the Memory-overhead is not a bad thing, because we can use 
	//    that memory later (it remains in the Memory-Pool).
	//

	unsigned int uiNeededChunks = CalculateNeededChunks(sMemorySize);
	std::size_t sBestMemBlockSize = CalculateBestMemoryBlockSize(sMemorySize);

	TByte *ptrNewMemBlock = (TByte *)malloc(sBestMemBlockSize); // allocate from Operating System
	SMemoryChunk *ptrNewChunks = (SMemoryChunk *)malloc((uiNeededChunks * sizeof(SMemoryChunk))); // allocate Chunk-Array to Manage the Memory
	//nzh mark
	if (!((NULL !=ptrNewMemBlock) && (NULL !=ptrNewChunks)))
	{
		m_StrError = "Warning : System ran out of Memory";
		m_pMemoryPoolErroInfoMethod->Warning("MemoryPool",__FILE__,__LINE__,__FUNCTION__,m_StrError.c_str());
		return false;
	}//nzh Mark end

	// Adjust internal Values (Total/Free Memory, etc.)
	m_sTotalMemoryPoolSize += sBestMemBlockSize;
	m_sFreeMemoryPoolSize += sBestMemBlockSize;
	m_uiMemoryChunkCount += uiNeededChunks;

	// Usefull for Debugging : Set the Memory-Content to a defined Value
	if (m_bSetMemoryData)
	{
		memset(((void *)ptrNewMemBlock), NEW_ALLOCATED_MEMORY_CONTENT, sBestMemBlockSize);
	}

	// Associate the allocated Memory-Block with the Linked-List of MemoryChunks
	return LinkChunksToData(ptrNewChunks, uiNeededChunks, ptrNewMemBlock);;
}

/******************
CalculateNeededChunks
******************/
unsigned int CMemoryPool::CalculateNeededChunks(const std::size_t &sMemorySize)
{
	float f = (float)(((float)sMemorySize) / ((float)m_sMemoryChunkSize));
	return ((unsigned int)ceil(f));
}

/******************
CalculateBestMemoryBlockSize
******************/
std::size_t CMemoryPool::CalculateBestMemoryBlockSize(const std::size_t &sRequestedMemoryBlockSize)
{
	unsigned int uiNeededChunks = CalculateNeededChunks(sRequestedMemoryBlockSize);
	return std::size_t((uiNeededChunks * m_sMemoryChunkSize));
}

/******************
FreeChunks
******************/
void CMemoryPool::FreeChunks(SMemoryChunk *ptrChunk)
{
	// Make the Used Memory of the given Chunk available
	// to the Memory Pool again.
   
	SMemoryChunk *ptrCurrentChunk = ptrChunk;
	unsigned int uiChunkCount = CalculateNeededChunks(ptrCurrentChunk->UsedSize);
	for (unsigned int i = 0; i < uiChunkCount; i++)
	{
		if (ptrCurrentChunk)
		{
			// Step 1 : Set the allocated Memory to 'FREEED_MEMORY_CONTENT'
			// Note : This is fully Optional, but usefull for debugging
			if (m_bSetMemoryData)
			{
				memset(((void *)ptrCurrentChunk->Data), FREEED_MEMORY_CONTENT, m_sMemoryChunkSize);
			}

			// Step 2 : Set the Used-Size to Zero
			ptrCurrentChunk->UsedSize = 0;

			// Step 3 : Adjust Memory-Pool Values and goto next Chunk
			m_sUsedMemoryPoolSize -= m_sMemoryChunkSize;
			ptrCurrentChunk = ptrCurrentChunk->Next;
		}
	}
   
}


/******************
FindChunkSuitableToHoldMemory
******************/
SMemoryChunk *CMemoryPool::FindChunkSuitableToHoldMemory(const std::size_t &sMemorySize)
{
	// Find a Chunk to hold *at least* "sMemorySize" Bytes.
	unsigned int uiChunksToSkip = 0;
	bool bContinueSearch = true;
	SMemoryChunk *ptrChunk = m_ptrCursorChunk; // Start search at Cursor-Pos.
	for (unsigned int i = 0; i < m_uiMemoryChunkCount; ++i)
	{
		if (ptrChunk)
		{
			if (ptrChunk == m_ptrLastChunk) // End of List reached : Start over from the beginning
			{
				ptrChunk = m_ptrFirstChunk;
			}

			if (ptrChunk->DataSize >= sMemorySize)
			{
				if (ptrChunk->UsedSize == 0)
				{
					m_ptrCursorChunk = ptrChunk;
					return ptrChunk;
				}
			}
			uiChunksToSkip = CalculateNeededChunks(ptrChunk->UsedSize);
			if (uiChunksToSkip == 0)
			{
				uiChunksToSkip = 1;
			}
			ptrChunk = SkipChunks(ptrChunk, uiChunksToSkip);
		}
		else
		{
			bContinueSearch = false;
		}
	}
	return NULL;
}

/******************
SkipChunks
******************/
SMemoryChunk *CMemoryPool::SkipChunks(SMemoryChunk *ptrStartChunk, unsigned int uiChunksToSkip)
{
	SMemoryChunk *ptrCurrentChunk = ptrStartChunk;
	for (unsigned int i = 0; i < uiChunksToSkip; i++)
	{
		if (ptrCurrentChunk)
		{
			ptrCurrentChunk = ptrCurrentChunk->Next;
		}
		else
		{
			// Will occur, if you try to Skip more Chunks than actually available
			// from your "ptrStartChunk" 
			m_StrError = "Warning : Chunk == NULL was not expected.";
			m_pMemoryPoolErroInfoMethod->Warning("MemoryPool",__FILE__,__LINE__,__FUNCTION__,m_StrError.c_str());
			break;
		}
	}
	return ptrCurrentChunk;
}

/******************
SetMemoryChunkValues
******************/
void CMemoryPool::SetMemoryChunkValues(SMemoryChunk *ptrChunk, const std::size_t &sMemBlockSize)
{
	if ((ptrChunk)) // && (ptrChunk != m_ptrLastChunk))
	{
		ptrChunk->UsedSize = sMemBlockSize;
	}
	else
	{
		m_StrError = "Warning : Invalid NULL-Pointer passed";
		m_pMemoryPoolErroInfoMethod->Warning("MemoryPool",__FILE__,__LINE__,__FUNCTION__,m_StrError.c_str());
	}
}

/******************
WriteMemoryDumpToFile
******************/
bool CMemoryPool::WriteMemoryDumpToFile(const std::string &strFileName)
{

	return true;
}

/******************
LinkChunksToData
******************/
bool CMemoryPool::LinkChunksToData(SMemoryChunk *ptrNewChunks, unsigned int uiChunkCount, TByte *ptrNewMemBlock)
{
	SMemoryChunk *ptrNewChunk = NULL;
	unsigned int uiMemOffSet = 0;
	bool bAllocationChunkAssigned = false;
	for (unsigned int i = 0; i < uiChunkCount; i++)
	{
		if (!m_ptrFirstChunk)
		{
			m_ptrFirstChunk = SetChunkDefaults(&(ptrNewChunks[0]));
			m_ptrLastChunk = m_ptrFirstChunk;
			m_ptrCursorChunk = m_ptrFirstChunk;
		}
		else
		{
			ptrNewChunk = SetChunkDefaults(&(ptrNewChunks[i]));
			m_ptrLastChunk->Next = ptrNewChunk;
			m_ptrLastChunk = ptrNewChunk;
		}

		uiMemOffSet = (i * ((unsigned int)m_sMemoryChunkSize));
		m_ptrLastChunk->Data = &(ptrNewMemBlock[uiMemOffSet]);

		// The first Chunk assigned to the new Memory-Block will be 
		// a "AllocationChunk". This means, this Chunks stores the
		// "original" Pointer to the MemBlock and is responsible for
		// "free()"ing the Memory later....
		if (!bAllocationChunkAssigned)
		{
			m_ptrLastChunk->IsAllocationChunk = true;
			bAllocationChunkAssigned = true;
		}
	}
	return RecalcChunkMemorySize(m_ptrFirstChunk, m_uiMemoryChunkCount);
}

/******************
RecalcChunkMemorySize
******************/
bool CMemoryPool::RecalcChunkMemorySize(SMemoryChunk *ptrChunk, unsigned int uiChunkCount)
{
	unsigned int uiMemOffSet = 0;
	for (unsigned int i = 0; i < uiChunkCount; i++)
	{
		if (ptrChunk)
		{
			uiMemOffSet = (i * ((unsigned int)m_sMemoryChunkSize));
			ptrChunk->DataSize = (((unsigned int)m_sTotalMemoryPoolSize) - uiMemOffSet);
			ptrChunk = ptrChunk->Next;
		}
		else
		{
			m_StrError = "Warning : ptrChunk == NULL";
			m_pMemoryPoolErroInfoMethod->Warning("MemoryPool",__FILE__,__LINE__,__FUNCTION__,m_StrError.c_str());
			return false;
		}
	}
	return true;
}

/******************
SetChunkDefaults
******************/
SMemoryChunk *CMemoryPool::SetChunkDefaults(SMemoryChunk *ptrChunk)
{
	if (ptrChunk)
	{
		ptrChunk->Data = NULL;
		ptrChunk->DataSize = 0;
		ptrChunk->UsedSize = 0;
		ptrChunk->IsAllocationChunk = false;
		ptrChunk->Next = NULL;
	}
	return ptrChunk;
}

/******************
FindChunkHoldingPointerTo
******************/
SMemoryChunk *CMemoryPool::FindChunkHoldingPointerTo(void *ptrMemoryBlock)
{
	SMemoryChunk *ptrTempChunk = m_ptrFirstChunk;
	while (ptrTempChunk)
	{
		if (ptrTempChunk->Data == ((TByte *)ptrMemoryBlock))
		{
			break;
		}
		ptrTempChunk = ptrTempChunk->Next;
	}
	return ptrTempChunk;
}

/******************
FreeAllAllocatedMemory
******************/
void CMemoryPool::FreeAllAllocatedMemory()
{
	SMemoryChunk *ptrChunk = m_ptrFirstChunk;
	while (ptrChunk)
	{
		if (ptrChunk->IsAllocationChunk)
		{
			free(((void *)(ptrChunk->Data)));
		}
		ptrChunk = ptrChunk->Next;
	}
}

/******************
DeallocateAllChunks
******************/
void CMemoryPool::DeallocateAllChunks()
{
	SMemoryChunk *ptrChunk = m_ptrFirstChunk;
	SMemoryChunk *ptrChunkToDelete = NULL;
	while (ptrChunk)
	{
		if (ptrChunk->IsAllocationChunk)
		{
			if (ptrChunkToDelete)
			{
				free(((void *)ptrChunkToDelete));
			}
			ptrChunkToDelete = ptrChunk;
		}
		ptrChunk = ptrChunk->Next;
	}
}

/******************
IsValidPointer
******************/
bool CMemoryPool::IsValidPointer(void *ptrPointer)
{

	return true;
}

/******************
MaxValue
******************/
std::size_t CMemoryPool::MaxValue(const std::size_t &sValueA, const std::size_t &sValueB) const
{
	if (sValueA > sValueB)
	{
		return sValueA;
	}
	return sValueB;
}

bool CMemoryPool::InitErroInfoMethod(void* pMemoryPoolErroInfoMethod)
{

    return true;
}

