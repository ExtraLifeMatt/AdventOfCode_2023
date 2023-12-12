#pragma once

#include <cstdint>
#include <cassert>
#include <atomic>
#include <memory>

namespace Memory
{

	// Thread safe linear allocator.
	// Just keeps allocating and asserts when it runs out.
	class LinearAllocator
	{
	public:
		LinearAllocator(size_t sizeInBytes)
			: m_memory(nullptr),
			m_totalSize(sizeInBytes),
			m_offset(0ULL)
		{
			m_memory = (char*)malloc(m_totalSize);
			assert(m_memory);
		}

		~LinearAllocator()
		{
			assert(m_memory);
			free(m_memory);
			m_memory = nullptr;
		}

		// Templated alloc  - int* myInt = Alloc<int>();
		template<typename T>
		T* Alloc()
		{
			return (T*)Alloc(sizeof(T));
		}

		// Templated object constructor - MyClass* myClass = Construct<MyClass>(someConstructorParam);
		template<class U, typename ...Args>
		U* Construct(Args&&... args)
		{
			U* memory = Alloc<U>();
			memory->U(std::forward(args)...);
		}

		// Templated object destructor call. Destroy<MyClass>(myClass);
		template<class U>
		void Destroy(U* obj)
		{
			assert(obj);
			obj->~U();
			obj = nullptr;
		}

		// Malloc style allocator
		void* Alloc(size_t size)
		{
			assert((m_offset.load(std::memory_order_acquire) + size) < m_totalSize);
			return m_memory + m_offset.fetch_add(size, std::memory_order_release);
		}

		uintptr_t GetBaseOffset() const { return (uintptr_t)m_memory; }

		void Reset() { m_offset.store(0, std::memory_order_release); }
	private:
		char* m_memory;
		size_t m_totalSize;
		std::atomic<uint64_t> m_offset;
	};

	// Thread safe ring allocator.
	// Keeps a block of memory and simply wraps around if it runs out. Additionally will call the "OnWrapAroundFunc" argument when the wrap around happens.
	// Ring buffers never "free" memory. There is a call to call a destructor, but nothing is returned to the OS.
	// When a wrap around happens, it's up to the user to clear any previous allocations. 
	class Ringbuffer
	{
	public:
		typedef void (*OnWrapAroundFunc)(void);
		
		Ringbuffer(size_t sizeInBytes, OnWrapAroundFunc wrapCallback = nullptr)
		: m_memory(nullptr),
		m_totalSize(sizeInBytes),
		m_offset(0ULL),
		m_wrapCallback(wrapCallback)
		{
			m_memory = (char*)malloc(m_totalSize);
			assert(m_memory);
		}

		~Ringbuffer()
		{
			assert(m_memory);
			free(m_memory);
			m_memory = nullptr;
		}

		// Templated alloc  - int* myInt = Alloc<int>();
		template<typename T>
		T* Alloc()
		{
			return (T*)Alloc(sizeof(T));
		}

		// Templated object constructor - MyClass* myClass = Construct<MyClass>(someConstructorParam);
		template<class U, typename ...Args>
		U* Construct(Args&&... args)
		{
			U* memory = Alloc<U>();
			memory->U(std::forward(args)...);
		}

		// Templated object destructor call. Destroy<MyClass>(myClass);
		template<class U>
		void Destroy(U* obj)
		{
			assert(obj);
			obj->~U();
			obj = nullptr;
		}

		// Malloc style allocator
		void* Alloc(size_t size)
		{
			assert(size < m_totalSize);
			if (m_offset.load(std::memory_order_acquire) + size >= m_totalSize)
			{
				Reset();
				if (m_wrapCallback)
				{
					(*m_wrapCallback)();
				}
			}

			return m_memory + m_offset.fetch_add(size, std::memory_order_release);
		}

		uintptr_t GetBaseOffset() const { return (uintptr_t)m_memory; }

		void Reset()
		{
			m_offset.store(0, std::memory_order_release);
		}
	private:
		char* m_memory;
		size_t m_totalSize;
		std::atomic<uint64_t> m_offset;
		OnWrapAroundFunc m_wrapCallback;
	};

}
