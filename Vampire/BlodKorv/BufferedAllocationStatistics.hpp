//
// File:         BufferedAllocationStatistics.hpp
// Description:  Raw memory allocation.
// Created:      Mar 2, 2000, Alexandre Riazanov, riazanov@cs.man.ac.uk
//=====================================================================
#ifndef BUFFERED_ALLOCATION_STATISTICS_H
#define BUFFERED_ALLOCATION_STATISTICS_H
//=====================================================================
#include <climits> 
#include "jargon.hpp"
#include "BlodKorvDebugFlags.hpp"
#ifndef NO_DEBUG_VIS
#include <iostream>
#endif
//=====================================================================
#undef DEBUG_NAMESPACE
#ifdef DEBUG_BUFFERED_ALLOCATION_STATISTICS
#define DEBUG_NAMESPACE "BufferedAllocationStatistics<MaxSmallObjectSize>"
#endif 
#include "debugMacros.hpp"
//=====================================================================
namespace BK 
{

template <ulong MaxSmallObjectSize>
class BufferedAllocationStatistics
{
 public:
  BufferedAllocationStatistics() 
    {
      reset();
    };
  
  ~BufferedAllocationStatistics()
  { 
    BK_CORRUPT(*this);
  };

  void reset()
    {  
      CALL("reset()");
      _bufferIsBeingUsed = false;
      _sizeOfRemainingNonbufferMemory = LONG_MAX;
      _sizeOfFreeLarge = 0L;
      _sizeOfPersistentLarge = 0L;
      _sizeOfActiveBufferDeficitInLarge = 0L;
      _sizeOfSuspendedBufferDeficitInLarge = 0L;
      _totalSizeOfPersistent = 0L;
      _totalSizeOfActiveBufferDeficit = 0L;

      for (ulong size = 0UL; size <= MaxSmallObjectSize; ++size)
	{
	  _free[size] = 0L;
	  _persistent[size] = 0L;
	  _takenFromBuffer[size] = 0L;
	  _activeBufferDeficit[size] = 0L;
	  _suspendedBufferDeficit[size] = 0L;
	};      
    }; // void reset()

  void setSizeOfNonbufferMemory(long size) 
    {
      _sizeOfRemainingNonbufferMemory = size;
    };

  void suspendActiveBufferDeficit() 
    {
      CALL("suspendActiveBufferDeficit()");
      _totalSizeOfActiveBufferDeficit = 0L;
      _sizeOfSuspendedBufferDeficitInLarge += _sizeOfActiveBufferDeficitInLarge;
      _sizeOfActiveBufferDeficitInLarge = 0L;
      for (ulong size = 1UL; size <= MaxSmallObjectSize; ++size)
	{	  
          _suspendedBufferDeficit[size] += _activeBufferDeficit[size];
	  _activeBufferDeficit[size] = 0L;
	};
    }; // void suspendActiveBufferDeficit() 

  long free(ulong size) const
    {
      return _free[size];
    };

  long persistent(ulong size) const
    {
      return _persistent[size];
    };
  
  long sizeOfFreeLarge() const
    {
      return _sizeOfFreeLarge;
    };
  long sizeOfPersistentLarge() const
    {
      return _sizeOfPersistentLarge;
    };

  long bufferDeficit(ulong size) const 
    {
      return _activeBufferDeficit[size] + 
	_suspendedBufferDeficit[size];
    };
  
  long sizeOfBufferDeficitInLarge() const
    {
      return _sizeOfActiveBufferDeficitInLarge +
	_sizeOfSuspendedBufferDeficitInLarge;
    };

  long countTotalSizeOfPersistent() const
    {
      long res = _sizeOfPersistentLarge;
      for (ulong size = 0; size <= MaxSmallObjectSize; ++size)
	res += _persistent[size] * size;
      return res;
    };
  
  long totalSizeOfPersistent() const
    {
      CALL("totalSizeOfPersistent() const");
      ASSERT(invariant4());
      return _totalSizeOfPersistent;
    };

  long countTotalSizeOfBufferDeficit() const
    {
      return countTotalSizeOfActiveBufferDeficit() + 
	countTotalSizeOfSuspendedBufferDeficit();
    };
  

  long countTotalSizeOfActiveBufferDeficit() const
    {
      long res = _sizeOfActiveBufferDeficitInLarge;
      for (ulong size = 0; size <= MaxSmallObjectSize; ++size)
	res += _activeBufferDeficit[size] * size;
      return res;
    };
  long totalSizeOfActiveBufferDeficit() const
    {
      CALL("totalSizeOfActiveBufferDeficit() const");
      ASSERT(invariant5());
      return _totalSizeOfActiveBufferDeficit;
    };

  long countTotalSizeOfSuspendedBufferDeficit() const
    {
      long res = _sizeOfSuspendedBufferDeficitInLarge;
      for (ulong size = 0; size <= MaxSmallObjectSize; ++size)
	res += _suspendedBufferDeficit[size] * size;
      return res;
    };

  long totalSizeOfSuspendedBufferDeficit() const
    {
      return totalSizeOfBufferDeficit() - totalSizeOfActiveBufferDeficit();
    };

  

  // registered operations
  
  void registerAllocationFromFreeList(ulong size)
    {
      CALL("registerAllocationFromFreeList(ulong size)");
      ASSERT(size <= MaxSmallObjectSize);
      ASSERT(_free[size] > 0L);
      if (_bufferIsBeingUsed) 
	{
	  if (_free[size] <= _takenFromBuffer[size])
	    incActiveBufferDeficit(size);
	};
      decFree(size);
      incPersistent(size);
      ASSERT(invariant1(size));
    }; // void registerAllocationFromFreeList(ulong size)

  void registerDeallocationIntoFreeList(ulong size)
    {
      CALL("registerDeallocationIntoFreeList(ulong size)");
      ASSERT(size <= MaxSmallObjectSize);
      if (_bufferIsBeingUsed)
	{
	  if (_activeBufferDeficit[size])
	    {
	      decActiveBufferDeficit(size);
	    }
	  else
	    if (_suspendedBufferDeficit[size])
	      {
		decSuspendedBufferDeficit(size);
	      };
	};      
      incFree(size);
      decPersistent(size);
      ASSERT(invariant1(size));
    }; // void registerDeallocationIntoFreeList(ulong size)

  void registerAllocationFromFreeLarge(ulong sizeOfAllocated,
				       ulong sizeOfReminder)
    {
      CALL("registerAllocationFromFreeLarge(ulong sizeOfAllocated,ulong sizeOfReminder)");
      ASSERT(sizeOfAllocated > MaxSmallObjectSize);
      registerAllocationFromCurrentChunk(sizeOfAllocated);
      // ^ not enough, _sizeOfFreeLarge changes 
      _sizeOfFreeLarge -= sizeOfAllocated;
      if (sizeOfReminder > 0L)
	{
	  if (sizeOfReminder <= MaxSmallObjectSize)
	    {
	      _sizeOfFreeLarge -= sizeOfReminder;
	      // as if we allocated it and deallocated immediately
	      registerAllocationFromCurrentChunk(sizeOfReminder);
	      registerDeallocationIntoFreeList(sizeOfReminder);
	      ASSERT(invariant1(sizeOfReminder));
	    };
	};   
      ASSERT(invariant3());
    }; // void registerAllocationFromFreeLarge(ulong sizeOfAllocated,ulong sizeOfReminder)

  void registerDeallocationOfLarge(ulong size)
    {
      CALL("registerDeallocationOfLarge(ulong size)");
      ASSERT(size > MaxSmallObjectSize);
      if (_bufferIsBeingUsed)
	{
	  ulong availableReimbursment = size;
	  // first, try to cover some active deficit in large
	  if (_sizeOfActiveBufferDeficitInLarge >= (long)availableReimbursment)
	    {
	      decSizeOfActiveBufferDeficitInLarge(availableReimbursment);
	    }
	  else
	    {	  	  
	      availableReimbursment -= _sizeOfActiveBufferDeficitInLarge;
	      decSizeOfActiveBufferDeficitInLarge(_sizeOfActiveBufferDeficitInLarge);
	      ASSERT(_sizeOfActiveBufferDeficitInLarge == 0UL);
	      
	      // try to cover some suspended deficit in large
	      if (_sizeOfSuspendedBufferDeficitInLarge >= (long)availableReimbursment)
		{
		  decSizeOfSuspendedBufferDeficitInLarge(availableReimbursment);
		}
	      else
		{
		  availableReimbursment -= _sizeOfSuspendedBufferDeficitInLarge;
		  decSizeOfSuspendedBufferDeficitInLarge(_sizeOfSuspendedBufferDeficitInLarge);
		  ASSERT(_sizeOfSuspendedBufferDeficitInLarge == 0UL);

		  // try to cancel some active deficit in free lists
		  ulong i = availableReimbursment;
		  if (i > MaxSmallObjectSize) i = MaxSmallObjectSize;
		  while (i > 0UL)
		    {
		      ASSERT(i <= availableReimbursment);
		      if (_activeBufferDeficit[i])
			{
			  decActiveBufferDeficit(i);
			  decTakenFromBuffer(i);
			  ASSERT(invariant1(i));
			  availableReimbursment -= i;
			  if (availableReimbursment < i) 
			    i = availableReimbursment;
			}
		      else
			--i;
		    };	      

		  // try to cancel some suspended deficit in free lists
		  i = availableReimbursment;
		  if (i > MaxSmallObjectSize) i = MaxSmallObjectSize;
		  while (i > 0UL)
		    {
		      ASSERT(i <= availableReimbursment);
		      ASSERT(!_activeBufferDeficit[i]);
		      if (_suspendedBufferDeficit[i])
			{
			  decSuspendedBufferDeficit(i);
			  decTakenFromBuffer(i);
			  ASSERT(invariant1(i));
			  availableReimbursment -= i;
			  if (availableReimbursment < i) 
			    i = availableReimbursment;
			}
		      else
			--i;
		    };	  
		};
	    };
	};
      decPersistentLarge(size);
      _sizeOfFreeLarge += size;
      ASSERT(invariant3());
    }; // void registerDeallocationOfLarge(ulong size)

  void registerAllocationFromCurrentChunk(ulong size)
    {
      CALL("registerAllocationFromCurrentChunk(ulong size)");
      if (_bufferIsBeingUsed)
	{
	use_buffer:	  
	  if (size > MaxSmallObjectSize)
	    {
	      incSizeOfActiveBufferDeficitInLarge(size);
	      incPersistentLarge(size);	   
	      ASSERT(invariant3());
	    }
	  else // small object
	    {
	      // Do not assert _free[size] == 0L here!
	      // The function may be used in different circumstances!
	      incTakenFromBuffer(size);
	      if (_free[size] < _takenFromBuffer[size])		
		incActiveBufferDeficit(size);
	      incPersistent(size);
	      ASSERT(invariant1(size));
	    };
	  return;
	};
      if ((long)size > _sizeOfRemainingNonbufferMemory)
	{
	  _sizeOfRemainingNonbufferMemory = 0L;
	  _bufferIsBeingUsed = true;
	  goto use_buffer;
	};
      // buffer is not used
      _sizeOfRemainingNonbufferMemory -= size;
      if (size > MaxSmallObjectSize)
	{
	  incPersistentLarge(size);
	  ASSERT(invariant3());
	}
      else
	incPersistent(size);
    }; // void registerAllocationFromCurrentChunk(ulong size)
  
  void registerUtilisationOfCurrentChunkReminder(ulong size)
    {
      CALL("registerUtilisationOfCurrentChunkReminder(ulong size)");
      ASSERT(size > 0L);
      if (size > MaxSmallObjectSize)
	{
	  // buffer is not affected
	  _sizeOfFreeLarge += size;	
	}
      else // becomes a freed small object
	{
	  // as if we allocated it and deallocated immediately
	  registerAllocationFromCurrentChunk(size);
	  registerDeallocationIntoFreeList(size);
	  ASSERT(invariant1(size));
	};
    }; // void registerUtilisationOfCurrentChunkReminder(ulong size)

  void registerUseOfFreeLargeAsCurrentChunk(ulong size)
    {
      CALL("registerUseOfFreeLargeAsCurrentChunk(ulong size)");
      ASSERT(size > MaxSmallObjectSize)
      ASSERT(_sizeOfFreeLarge > 0L);
      _sizeOfFreeLarge -= size;
      // buffer is not affected    
    }; // void registerUseOfFreeLargeAsCurrentChunk(ulong size)
  
  void registerUseOfFreeAsCurrentChunk(ulong size)
    {
      CALL("registerUseOfFreeAsCurrentChunk(ulong size)");
      ASSERT(size <= MaxSmallObjectSize);
      if (_bufferIsBeingUsed)
	{
	  if (_free[size] <= _takenFromBuffer[size])
	    {
	      incActiveBufferDeficit(size);
	    }
	  else // this piece may cancel some buffer deficit for smaller sizes
	    {
	      ulong availableReimbursment = size;
	      // first, try to cancel some active deficit
	      ulong i = availableReimbursment - 1;
	      while (i > 0UL)
		{
		  ASSERT(i <= availableReimbursment);
		  if (_activeBufferDeficit[i])
		    {
		      decActiveBufferDeficit(i);
		      decTakenFromBuffer(i);
		      availableReimbursment -= i;
		      if (availableReimbursment < i) 
			i = availableReimbursment;
		    }
		  else
		    --i;
		};	      

	      // try to cancel some suspended deficit
	      i = availableReimbursment;
	      while (i > 0UL)
		{
		  ASSERT(i <= availableReimbursment);
		  ASSERT(!_activeBufferDeficit[i]);
		  if (_suspendedBufferDeficit[i])
		    {
		      decSuspendedBufferDeficit(i);
		      decTakenFromBuffer(i);
		      availableReimbursment -= i;
		      if (availableReimbursment < i) 
			i = availableReimbursment;
		    }
		  else
		    --i;
		};	  

	    };
	};
      decFree(size);
      ASSERT(invariant1(size));
    }; // void registerUseOfFreeAsCurrentChunk(ulong size)


#ifndef NO_DEBUG
  ostream& output(ostream& str,ulong size) const
    {
      str << "SIZE " << size << ":\n";
      str << "  FREE " << _free[size] << "\n";
      str << "  PERSISTENT " << _persistent[size] << "\n";
      str << "  TAKEN FROM BUFFER " << _takenFromBuffer[size] << "\n";
      str << "  ACTIVE BUFFER DEFICIT " << _activeBufferDeficit[size] << "\n";
      str << "  SUSPENDED BUFFER DEFICIT " << _suspendedBufferDeficit[size] << "\n\n";
      return str;
    };
  ostream& output(ostream& str) const
    {
      str << "TOTAL SIZE OF PERSISTENT " << totalSizeOfPersistent() 
	  << " == " << countTotalSizeOfPersistent() << "\n";
      str << "TOTAL SIZE OF ACTIVE BUFFER DEFICIT " 
	  << totalSizeOfActiveBufferDeficit() 
	  << " == " << countTotalSizeOfActiveBufferDeficit() << "\n\n";
      str << "LARGE PIECES:\n";
      str << "  SIZE OF FREE " << _sizeOfFreeLarge << "\n";
      str << "  SIZE OF PERSISTENT " << _sizeOfPersistentLarge << "\n";
      str << "  SIZE OF ACTIVE BUFFER DEFICIT " << _sizeOfActiveBufferDeficitInLarge << "\n";
      str << "  SIZE OF SUSPENDED BUFFER DEFICIT " << _sizeOfSuspendedBufferDeficitInLarge << "\n\n";     
      return str;
    };
#endif

 private:
  // primitives
  void incFree(ulong size)
    {
      CALL("incFree(ulong size)");
      ASSERT(size <= MaxSmallObjectSize);
      ++_free[size];      
    };
  
  void decFree(ulong size)
    {
      CALL("decFree(ulong size)");
      ASSERT(size <= MaxSmallObjectSize);
      --_free[size];      
    };
  
  void incPersistent(ulong size)
    {
      CALL("incPersistent(ulong size)");
      ASSERT(size <= MaxSmallObjectSize);
      ++_persistent[size];
      _totalSizeOfPersistent += size;
    };  
  
  void decPersistent(ulong size)
    {
      CALL("decPersistent(ulong size)");
      ASSERT(size <= MaxSmallObjectSize);
      --_persistent[size];
      _totalSizeOfPersistent -= size;
    };

  void incPersistentLarge(ulong size)
    {
      _sizeOfPersistentLarge += size;
      _totalSizeOfPersistent += size;
    };  

  void decPersistentLarge(ulong size)
    {
      _sizeOfPersistentLarge -= size;
      _totalSizeOfPersistent -= size;
    };  

  void incTakenFromBuffer(ulong size)
    {
      CALL("incTakenFromBuffer(ulong size)");
      ASSERT(size <= MaxSmallObjectSize);
      ++_takenFromBuffer[size];
    };

  void decTakenFromBuffer(ulong size)
    {
      CALL("decTakenFromBuffer(ulong size)");
      ASSERT(size <= MaxSmallObjectSize);
      --_takenFromBuffer[size];
    };

  void incActiveBufferDeficit(ulong size)
    {
      CALL("incActiveBufferDeficit(ulong size)");
      ASSERT(size <= MaxSmallObjectSize);
      ++_activeBufferDeficit[size];
      _totalSizeOfActiveBufferDeficit += size;
    };

  void decActiveBufferDeficit(ulong size)
    {
      CALL("decActiveBufferDeficit(ulong size)");
      ASSERT(size <= MaxSmallObjectSize);
      --_activeBufferDeficit[size];
      _totalSizeOfActiveBufferDeficit -= size;
    };

  void incSuspendedBufferDeficit(ulong size)
    {
      CALL("incSuspendedBufferDeficit(ulong size)");
      ASSERT(size <= MaxSmallObjectSize);
      ++_suspendedBufferDeficit[size];
    };

  void decSuspendedBufferDeficit(ulong size)
    {
      CALL("decSuspendedBufferDeficit(ulong size)");
      ASSERT(size <= MaxSmallObjectSize);
      --_suspendedBufferDeficit[size];
    };

  void incSizeOfActiveBufferDeficitInLarge(ulong size)
    {
      _sizeOfActiveBufferDeficitInLarge += size;
      _totalSizeOfActiveBufferDeficit += size;
    };

  void decSizeOfActiveBufferDeficitInLarge(ulong size)
    {
      _sizeOfActiveBufferDeficitInLarge -= size;
      _totalSizeOfActiveBufferDeficit -= size;
    };

  void decSizeOfSuspendedBufferDeficitInLarge(ulong size)
    {
      _sizeOfSuspendedBufferDeficitInLarge -= size;
    };

  // invariants 
  bool invariant1(ulong size) const 
    {
#ifndef DEBUG_ALLOC_NO_REUSE
      if (_free[size] <= _takenFromBuffer[size])
	{
	  return bufferDeficit(size) == _takenFromBuffer[size] - _free[size];
	}
      else
	return !bufferDeficit(size);
#else
      return true;
#endif
    };

  bool invariant3() const
    {
      return sizeOfBufferDeficitInLarge() <= _sizeOfPersistentLarge;
    };
  
  bool invariant4() const
    {
      return countTotalSizeOfPersistent() == _totalSizeOfPersistent;
    };

  
  bool invariant5() const
    {
      return countTotalSizeOfActiveBufferDeficit() == _totalSizeOfActiveBufferDeficit;
    };
  
  
  // WRONG:
  // 1) bufferDeficit(size) <= _persistent[size]
  //    See registerUseOfFreeAsCurrentChunk(ulong size).



 private:
  // mode
  bool _bufferIsBeingUsed;
  long _sizeOfRemainingNonbufferMemory;

  // basic statistics
  long _free[MaxSmallObjectSize+1];
  long _persistent[MaxSmallObjectSize+1];
  long _sizeOfFreeLarge;
  long _sizeOfPersistentLarge;

  // buffer usage
  long _takenFromBuffer[MaxSmallObjectSize+1];
  long _activeBufferDeficit[MaxSmallObjectSize+1];
  long _suspendedBufferDeficit[MaxSmallObjectSize+1];
  long _sizeOfActiveBufferDeficitInLarge;
  long _sizeOfSuspendedBufferDeficitInLarge;

  // fast access to derived values
  long _totalSizeOfPersistent;
  long _totalSizeOfActiveBufferDeficit;
  
}; // class BufferedAllocationStatistics<MaxSmallObjectSize>


}; // namespace BK



//=====================================================================
#endif
