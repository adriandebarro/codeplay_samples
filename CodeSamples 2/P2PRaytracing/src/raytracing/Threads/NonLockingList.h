#pragma once 

#include <atomic>

#include "../CoreClasses/UtilClasses/Common.h"

using namespace Engine::Common;
namespace Engine
{
	namespace Threads
	{
		namespace NonLocking
		{
			//---------------------------------------------------------------------
			//
			//---------------------------------------------------------------------
			template<typename T, size_t array_size, size_t default_value>
			struct LinkedListNode
			{
				T* m_array[array_size] = {nullptr};
				std::atomic<LinkedListNode<T, array_size, default_value>*> m_tail = ATOMIC_VAR_INIT(nullptr);
				
				LinkedListNode()
				{}

				T* GetElement(int p_index)
				{
					return m_array[p_index];
				}
			};

			//---------------------------------------------------------------------
			// NonLockingList: Waitfree list
			//---------------------------------------------------------------------
			template<typename T, size_t default_value>
			class NonLockingList
			{
			public:
				typedef LinkedListNode<T, ELEMENTS_PER_LINKED_LIST_NODE, default_value> ListNode;
				NonLockingList()
				{
					m_indexCounter = ATOMIC_VAR_INIT(0);
					m_head = new ListNode();
				}

				//---------------------------------------------------------------------
				//
				//---------------------------------------------------------------------
				int InsertValue(T* p_curentSample)
				{
					//atomic increment 
					int index = std::atomic_fetch_add(&m_indexCounter, 1);
					int localIndex = int(index % ELEMENTS_PER_LINKED_LIST_NODE);
					int blockNumber = int(index / ELEMENTS_PER_LINKED_LIST_NODE);

					_ASSERT(localIndex < ELEMENTS_PER_LINKED_LIST_NODE);
					
					ListNode* currentBlock = m_head;
					int blockIndex = 0;
					while(blockIndex != blockNumber)
					{
						if(std::atomic_load(&currentBlock->m_tail) == nullptr)
						{
							ListNode* temp = new ListNode();
							ListNode* nullPtr = nullptr;
							bool success = std::atomic_compare_exchange_strong(&(currentBlock->m_tail), &nullPtr, temp);
							if (!success)
								delete temp;
						}

						currentBlock = currentBlock->m_tail;
						blockIndex++;
					}

					currentBlock->m_array[localIndex] = p_curentSample;
					return index;
				}
				//---------------------------------------------------------------------

				//---------------------------------------------------------------------
				//
				//---------------------------------------------------------------------
				T* GetAtIndex(int p_index, bool& p_found)
				{
					int localIndex = int(p_index % ELEMENTS_PER_LINKED_LIST_NODE);
					int blockNumber = int(p_index / ELEMENTS_PER_LINKED_LIST_NODE);
					
					_ASSERT(localIndex < ELEMENTS_PER_LINKED_LIST_NODE);

					int blockIndex = 0;
					ListNode* currentNode = m_head;
					while(blockIndex != blockNumber)
					{
						currentNode = currentNode->m_tail;
						if (currentNode == nullptr)
						{
							p_found = false;
							return nullptr;
						}
						else
							blockIndex++;
					}		

					if (currentNode->m_array[localIndex] == nullptr)
					{
						p_found = false;
						return nullptr;
					}
					else
					{
						p_found = true;
						return currentNode->m_array[localIndex];
					}
				}

				T* GetAtIndex(int p_index)
				{
					int localIndex = int(p_index % ELEMENTS_PER_LINKED_LIST_NODE);
					int blockNumber = int(p_index / ELEMENTS_PER_LINKED_LIST_NODE);

					_ASSERT(localIndex < ELEMENTS_PER_LINKED_LIST_NODE);

					int blockIndex = 0;
					ListNode* currentNode = m_head;
					while (blockIndex != blockNumber)
					{
						currentNode = currentNode->m_tail;
						if (currentNode == nullptr)
							return nullptr;
						else
							blockIndex++;
					}

					return currentNode->m_array[localIndex];
				}


				//---------------------------------------------------------------------

				int GetTotalSize()
				{
					return std::atomic_load(&m_indexCounter);

				}

				void InvalidateNode()
				{
					std::atomic_store(&m_indexCounter, 0);
				}


			private:
				ListNode* m_head;
				std::atomic<int> m_indexCounter;
			};

		}
	}
}