/******************
IMemoryBlock.h
******************/

/*!\file IMemoryBlock.h
 * \brief Contains the "IMemoryBlock" Class-defintion.
 *        This is the (abstract) interface for the actual MemoryPool-Class.
 */


#ifndef __INC_IMemoryBlock_h__
#define __INC_IMemoryBlock_h__
#include <string>
#include "BasicIncludes.h"
#define DEFAULT_MEMORY_POOL_SIZE       1000                          //!< Initial MemoryPool size (in Bytes)
#define DEFAULT_MEMORY_CHUNK_SIZE       128                            //!< Default MemoryChunkSize (in Bytes)
#define DEFAULT_MEMORY_SIZE_TO_ALLOCATE ( DEFAULT_MEMORY_CHUNK_SIZE * 2) //!< Default Minimal Memory-Size (in Bytes) to Allocate. 
using namespace std ;
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
typedef unsigned char TByte ;

/*!\class IMemoryBlock
 * \brief Interface Class (pure Virtual) for the MemoryPool
 *
 * Abstract Base-Class (interface) for the MemoryPool.
 * Introduces Basic Operations like Geting/Freeing Memory.
 */
class IMemoryBlock
{
  public :
    virtual ~IMemoryBlock() {} ;
	virtual bool InintMemoryPool(const std::size_t &sInitialMemoryPoolSize = DEFAULT_MEMORY_POOL_SIZE,
		const std::size_t &sMemoryChunkSize = DEFAULT_MEMORY_CHUNK_SIZE,
		const std::size_t &sMinimalMemorySizeToAllocate = DEFAULT_MEMORY_SIZE_TO_ALLOCATE,
		bool bSetMemoryData = false) = 0;

	/*!
	Get "sMemorySize" Bytes from the Memory Pool.
	\param [IN] sMemorySize       Sizes (in Bytes) of Memory.
	\return                       Pointer to a Memory-Block of "sMemorySize" Bytes, or NULL if an error occured.
	*/
	virtual void *GetMemory(const std::size_t &sMemorySize) = 0;

	/*!
	Free the allocated memory again....
	\param [IN] ptrMemoryBlock    Pointer to a Block of Memory, which is to be freed (previoulsy allocated via "GetMemory()").
	\param [IN] sMemorySize       Sizes (in Bytes) of Memory.
	*/
	virtual void FreeMemory(void *ptrMemoryBlock, const std::size_t &sMemoryBlockSize) = 0;

	/*!
	Writes the contents of the MemoryPool to a File.
	Note : This file can be quite large (several MB).
	\param [IN] strFileName      FileName of the MemoryDump.
	\return                      true on success, false otherwise
	*/
	virtual bool WriteMemoryDumpToFile(const std::string &strFileName) = 0;
	/*!
	Check, if a Pointer is in the Memory-Pool.
	Note : This Checks only if a pointer is inside the Memory-Pool,
	and <b>not</b> if the Memory contains meaningfull data.
	\param [IN] ptrPointer       Pointer to a Memory-Block which is to be checked.
	\return                      true, if the Pointer could be found in the Memory-Pool, false otherwise.
	*/
	virtual bool IsValidPointer(void *ptrPointer) = 0;
	virtual string GetLastError() = 0;
protected:
	//CCriticalSectionFactory m_CSFactory;
   
} ;


#endif /* __INC_IMemoryBlock_h__ */
