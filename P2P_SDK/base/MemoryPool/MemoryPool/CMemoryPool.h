/******************
CMemoryPool.h
******************/

/*!\file CMemoryPool.h
 * \brief Contains the "CMemoryPool" Class-defintion.
 */

#ifndef __INC_CMemoryPool_h__
#define __INC_CMemoryPool_h__

#include "SMemoryChunk.h"
#include <string.h>
#include <string>
#include "BasicIncludes.h"
#include <mutex>
#include "Config.h"
#include "../../../Global/Global.h"
//#include "../../CriticalSession/CriticalSession/CriticalSectionFactory.h"
#define DEFAULT_MEMORY_POOL_SIZE       1000                          //!< Initial MemoryPool size (in Bytes)
#define DEFAULT_MEMORY_CHUNK_SIZE       128                            //!< Default MemoryChunkSize (in Bytes)
#define DEFAULT_MEMORY_SIZE_TO_ALLOCATE ( DEFAULT_MEMORY_CHUNK_SIZE * 2) //!< Default Minimal Memory-Size (in Bytes) to Allocate. 

#define FREEED_MEMORY_CONTENT         0xAA  //!< Value for freed memory  
#define NEW_ALLOCATED_MEMORY_CONTENT  0xFF  //!< Initial Value for new allocated memory 

using namespace std;
/*!\namespace MemPool
* \brief MemoryPool Namespace
*
* This Namespace contains all classes and typedefs needed by
* the MemoryPool implementation.
* The MemoryPool has its own namespace because some typedefs
* (e.g. TByte) may intefer with other toolkits if the
* MemoryPool would be in the global namespace.
*/

/*!\typedef unsigned char TByte ;
* \brief Byte (= 8 Bit) Typedefinition.
*/
typedef unsigned char TByte;

/*!\class IMemoryBlock
* \brief Interface Class (pure Virtual) for the MemoryPool
*
* Abstract Base-Class (interface) for the MemoryPool.
* Introduces Basic Operations like Geting/Freeing Memory.
*/



/*!\class CMemoryPool
 * \brief MemoryPool-Class
 *
 * This class is the actual implementation of the IMemoryBlock - Interface.
 * It is responsible for all MemoryRequests (GetMemory() / FreeMemory()) and
 * manages the allocation/deallocation of Memory from the Operating-System.
 */

class CErrorInfo;

class CMemoryPool 
{
  public :
	  CMemoryPool();
    /*!
	Contructor
	\param [IN] sInitialMemoryPoolSize The Initial Size (in Bytes) of the Memory Pool
	\param [IN] sMemoryChunkSize The Size (in Bytes) each MemoryChunk can Manage. 
	            A low sMemoryChunkSize increases the MemoryPool runtime (bad), but decreases the Memory-overhead/fragmentation (good)
    \param [IN] sMinimalMemorySizeToAllocate The Minimal amount of Memory which is allocated (in Bytes).
	            That means, every time you have to allocate more Memory from the Operating-System,
				<b>at least</b> sMinimalMemorySizeToAllocate Bytes are allocated.
				When you have to request small amount of Memory very often, this will speed up
				the MemoryPool, beacause when you allocate a new Memory from the OS,
				you will allocate a small "Buffer" automatically, wich will prevent you from
				requesting OS-memory too often.
	\param [IN] bSetMemoryData Set to true, if you want to set all allocated/freed Memory to a specific
	            Value. Very usefull for debugging, but has a negativ impact on the runtime.
	*/
	  virtual  bool InitMemoryPool(const std::size_t &sInitialMemoryPoolSize = DEFAULT_MEMORY_POOL_SIZE,
		        const std::size_t &sMemoryChunkSize = DEFAULT_MEMORY_CHUNK_SIZE,
				const std::size_t &sMinimalMemorySizeToAllocate = DEFAULT_MEMORY_SIZE_TO_ALLOCATE,
				bool bSetMemoryData = false) ;
    virtual ~CMemoryPool() ; //!< Destructor

	/*!
	  Get "sMemorySize" Bytes from the Memory Pool.
	  \param [IN] sMemorySize       Sizes (in Bytes) of Memory.
	  \return                       Pointer to a Memory-Block of "sMemorySize" Bytes, or NULL if an error occured. 
	*/
    virtual void *GetMemory(const std::size_t &sMemorySize) ;
    
    /*!
    Get "sMemorySize" Bytes from the Memory Pool.
    \param [IN] sMemorySize       Sizes (in Bytes) of Memory.
    \param [IN] tClassType        Template of class.
    \return                       Pointer to a Memory-Block of "sMemorySize" Bytes, or NULL if an error occured.
    */
//     template <class CLASSTYPE>
//     inline CLASSTYPE* GetClassMemory(const std::size_t & sMemorySize, CLASSTYPE* tClassType);
	/*!
	 Get "sMemorySize" Bytes from the Memory Pool.
	  \param [IN] ptrMemoryBlock    Pointer to a Block of Memory, which is to be freed (previoulsy allocated via "GetMemory()").
	  \param [IN] sMemorySize       Sizes (in Bytes) of Memory.
	*/
	virtual void FreeMemory(void *ptrMemoryBlock, const std::size_t &sMemoryBlockSize = 0) ;

//     template <class CLASSTYPE>
//     inline void FreeClassMemory( CLASSTYPE* ptrMemoryBlock);

	/*!
	  Writes the contents of the MemoryPool to a File.
	  Note : This file can be quite large (several MB).
	  \param [IN] strFileName      FileName of the MemoryDump.
	  \return                      true on success, false otherwise 
	*/
	virtual bool WriteMemoryDumpToFile(const std::string &strFileName);
	/*!
	  Check, if a Pointer is in the Memory-Pool.
	  Note : This Checks only if a pointer is inside the Memory-Pool,
	         and <b>not</b> if the Memory contains meaningfull data.
	  \param [IN] ptrPointer       Pointer to a Memory-Block which is to be checked.
	  \return                      true, if the Pointer could be found in the Memory-Pool, false otherwise.
	*/
	virtual bool IsValidPointer(void *ptrPointer);
	virtual string GetLastError();

    bool InitErroInfoMethod(void* pMemoryPoolErroInfoMethod);
  private :
    /*!
	  Will Allocate <b>at least</b> "sMemorySize" Bytes of Memory from the Operating-System.
	  The Memory will be cut into Pieces and Managed by the MemoryChunk-Linked-List.
	  (See LinkChunksToData() for details).
	  \param [IN] sMemorySize      The Memory-Size (in Bytes) to allocate
	  \return                      true, if the Memory could be allocated, false otherwise (e.g. System is out of Memory, etc.)
	*/
    bool AllocateMemory(const std::size_t &sMemorySize) ;

	void FreeAllAllocatedMemory() ; //!< Will free all Aloocated Memory to the Operating-System again.

    unsigned int CalculateNeededChunks(const std::size_t &sMemorySize) ; //!< \return the Number of MemoryChunks needed to Manage "sMemorySize" Bytes.
    std::size_t CalculateBestMemoryBlockSize(const std::size_t &sRequestedMemoryBlockSize) ; //!< return the amount of Memory which is best Managed by the MemoryChunks.

    SMemoryChunk *FindChunkSuitableToHoldMemory(const std::size_t &sMemorySize) ; //!< \return a Chunk which can hold the requested amount of memory, or NULL, if none was found.
    SMemoryChunk *FindChunkHoldingPointerTo(void *ptrMemoryBlock) ; //!< Find a Chunk which "Data"-Member is Pointing to the given "ptrMemoryBlock", or NULL if none was found.

	SMemoryChunk *SkipChunks(SMemoryChunk *ptrStartChunk, unsigned int uiChunksToSkip) ; //!< Skip the given amount of Chunks, starting from the given ptrStartChunk. \return the Chunk at the "skipping"-Position.
    SMemoryChunk *SetChunkDefaults(SMemoryChunk *ptrChunk) ; //!< Set "Default"-Values to the given Chunk

    void FreeChunks(SMemoryChunk *ptrChunk) ; //!< Makes the memory linked to the given Chunk available in the MemoryPool again (by setting the "UsedSize"-Member to 0).
	void DeallocateAllChunks() ; //!< Deallocates all Memory needed by the Chunks back to the Operating-System.

    bool LinkChunksToData(SMemoryChunk *ptrNewChunk, unsigned int uiChunkCount, TByte *ptrNewMemBlock) ; //!< Link the given Memory-Block to the Linked-List of MemoryChunks...
	void SetMemoryChunkValues(SMemoryChunk *ptrChunk, const std::size_t &sMemBlockSize) ; //!< Set the "UsedSize"-Member of the given "ptrChunk" to "sMemBlockSize".
	bool RecalcChunkMemorySize(SMemoryChunk *ptrChunks, unsigned int uiChunkCount) ; //!< Recalcs the "DataSize"-Member of all Chunks whe the Memory-Pool grows (via "AllocateMemory()")

	std::size_t MaxValue(const std::size_t &sValueA, const std::size_t &sValueB) const ; //!< \return the greatest of the two input values (A or B)
	
    SMemoryChunk *m_ptrFirstChunk ;  //!< Pointer to the first Chunk in the Linked-List of Memory Chunks
    SMemoryChunk *m_ptrLastChunk ;   //!< Pointer to the last Chunk in the Linked-List of Memory Chunks
    SMemoryChunk *m_ptrCursorChunk ; //!< Cursor-Chunk. Used to speed up the navigation in the linked-List.

    std::size_t m_sTotalMemoryPoolSize ;  //!< Total Memory-Pool size in Bytes
    std::size_t m_sUsedMemoryPoolSize ;   //!< amount of used Memory in Bytes
    std::size_t m_sFreeMemoryPoolSize ;   //!< amount of free Memory in Bytes

    std::size_t m_sMemoryChunkSize ;     //!< amount of Memory which can be Managed by a single MemoryChunk.
    unsigned int m_uiMemoryChunkCount ;  //!< Total amount of "SMemoryChunk"-Objects in the Memory-Pool.
	unsigned int m_uiObjectCount ;       //!< Counter for "GetMemory()" / "FreeMemory()"-Operation. Counts (indirectly) the number of "Objects" inside the mem-Pool.

	bool m_bSetMemoryData ;                      //!< Set to "true", if you want to set all (de)allocated Memory to a predefined Value (via "memset()"). Usefull for debugging.
	std::size_t m_sMinimalMemorySizeToAllocate ; //!< The minimal amount of Memory which can be allocated via "AllocateMemory()".
	string m_StrError;
protected:
    //CCriticalSectionFactory m_CSFactory;
    mutex m_CS;
    CErrorInfo *m_pMemoryPoolErroInfoMethod;
};
// 
// template <class CLASSTYPE>
// void CMemoryPool::FreeClassMemory(CLASSTYPE *ptrMemoryBlock)
// {
//     m_pCS.lock();
//     SMemoryChunk *ptrChunk = FindChunkHoldingPointerTo(ptrMemoryBlock);
//     if (nullptr != ptrChunk)
//     {
//        
//         ptrMemoryBlock->~CLASSTYPE();
// 		delete ptrChunk->Data;
//         //std::cerr << "Freed Chunks OK (Used memPool Size : " << m_sUsedMemoryPoolSize << ")" << std::endl ;
// 		FreeChunks(ptrChunk);
// 		if (m_uiObjectCount < 0)
// 		{
// 			ScreenPrintf("memory", "memory free over lenght\n");
// 		}
// 		--m_uiObjectCount;		
// 		m_pCS.unlock();
//     }
//     else
//     {
//         m_StrError = "Warning : Requested Pointer not in Memory Pool";
//         m_pMemoryPoolErroInfoMethod->Warning("MemoryPool",__FILE__,__LINE__,__FUNCTION__,m_StrError.c_str());
//     }
// 	m_pCS.unlock();
//    
// }
// 
// /******************
// GetClassMemory
// ******************/
// template <class CLASSTYPE>
// CLASSTYPE* CMemoryPool::GetClassMemory(const std::size_t & sMemorySize, CLASSTYPE* tClassType)
// {
//     m_pCS.lock();
//     std::size_t sBestMemBlockSize = CalculateBestMemoryBlockSize(sMemorySize);
//     SMemoryChunk *ptrChunk = NULL;
// 	int AllocateCount = 0;
//     while (!ptrChunk)
//     {
//         // Is a Chunks available to hold the requested amount of Memory ?		
//         ptrChunk = FindChunkSuitableToHoldMemory(sBestMemBlockSize);
//         if (nullptr == ptrChunk)
//         {
// 			m_pMemoryPoolErroInfoMethod->Warning("memory", __FILE__, __LINE__, __FUNCTION__, "os have not enought memory!");
// 			m_pCS.unlock();
//         }
// 		else
// 		{
// 			tClassType = new(sizeof(CLASSTYPE));
// 			new(tClassType)CLASSTYPE();
// 			ptrChunk->Data = (TByte*)tClassType;
// 			if (nullptr == ptrChunk->Data)
// 			{
// 				m_pMemoryPoolErroInfoMethod->Warning("memory", __FILE__, __LINE__, __FUNCTION__, "os have not memory,new classtype Faliled\n");
// 				ptrChunk->Data = nullptr;
// 				FreeChunks(ptrChunk);
// 				m_pCS.unlock();
// 				return NULL;
// 
// 			}
// 		}
//     }
// 
//     // Finally, a suitable Chunk was found.
//     // Adjust the Values of the internal "TotalSize"/"UsedSize" Members and 
//     // the Values of the MemoryChunk itself.
//     m_sUsedMemoryPoolSize += sBestMemBlockSize;
//     m_sFreeMemoryPoolSize -= sBestMemBlockSize;
//     m_uiObjectCount++;
//     SetMemoryChunkValues(ptrChunk, sBestMemBlockSize);
//     m_pCS.unlock();
//     // eventually, return the Pointer to the User
//     return ((CLASSTYPE *)ptrChunk->Data);
// 
// }

#endif /* __INC_CMemoryPool_h__ */
