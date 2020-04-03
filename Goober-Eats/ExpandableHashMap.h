#ifndef EXPANDHASH_H
#define EXPANDHASH_H
// ExpandableHashMap.h
// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.
#include <vector>
#include <iostream>

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
private:
	struct KeyValue
	{
		KeyType key;
		ValueType value;
	};
	int m_buckets;
	double m_load;
	std::vector<KeyValue>** thisHash; //pointer to our overall hash table

public:
	ExpandableHashMap(double maximumLoadFactor=0.5);

	~ExpandableHashMap(); 
	void reset();
	int size() const;
	
	void associate(const KeyType& key, const ValueType& value);
	// for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	// for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
	}

	// C++11 syntax for preventing copying and assignment
	ExpandableHashMap(const ExpandableHashMap&) = delete;
	ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

};

template<typename KeyType, typename ValueType>
ExpandableHashMap <KeyType, ValueType> ::ExpandableHashMap(double maximumLoadFactor): m_buckets(8), m_load(maximumLoadFactor)  
{
	if (maximumLoadFactor < 0 || maximumLoadFactor>1)
		m_load = 0.5;
	thisHash = new std::vector <KeyValue>*[m_buckets]; //dynamically allocated array of pointers to a vector of KeyValue structure objects
	for (int i = 0; i < m_buckets; i++) //initilaising pointer values to nullptr initially
		thisHash[i] = nullptr;

}
template<typename KeyType, typename ValueType>
ExpandableHashMap <KeyType, ValueType>::~ExpandableHashMap()//deallocating all the memory
{
	for (int i = 0; i < m_buckets; i++)
		delete thisHash[i];
	delete[] thisHash;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::reset()
{
	for (int i = 0; i < m_buckets; i++)
		delete thisHash[i];
	delete thisHash;

	m_buckets = 8;
	thisHash = new std::vector <ValueType> * [m_buckets];
	for (int i = 0; i < m_buckets; i++) //initilaising pointer values to nullptr initially
		thisHash[i] = nullptr;
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType, ValueType>::size() const
{
	int Size = 0;
	for (int i = 0; i < m_buckets; i++)
	{
		if (thisHash[i] != nullptr)
			Size+=(*thisHash[i]).size();
	}
	return Size;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value)
{
	

		if (find(key) != nullptr)
		{
			(*find(key)) = value;
			return;
		}
		unsigned int hasher(const KeyType & key);
		unsigned int hashedVal = hasher(key);
		unsigned int index = hashedVal % m_buckets;
		std::vector <KeyValue>* vec_ptr = thisHash[index];
		KeyValue inserted;
		inserted.key = key;
		(inserted.value) = value;
		if (vec_ptr == nullptr)
		{
			vec_ptr = new std::vector <KeyValue>;
			thisHash[index] = vec_ptr;
		}
		(*vec_ptr).push_back(inserted);
		double thisSize = size();
		if ((thisSize / m_buckets) >= m_load)
		{
			std::vector<KeyValue>** tempHash = new std::vector <KeyValue> *[2 * m_buckets];
			for (int k = 0; k < 2 * m_buckets; k++)
			{
				tempHash[k] = nullptr;
			}
			for (int i = 0; i < m_buckets; i++)
			{
				if (thisHash[i] != nullptr)
				{
					for (int j = 0; j < (*thisHash[i]).size(); j++)
					{
						unsigned int newIndex = hasher((*thisHash[i])[j].key) % (2 * m_buckets);
						if (tempHash[newIndex] == nullptr)
						{
							tempHash[newIndex] = new std::vector <KeyValue>;
						}
						KeyValue myBabyKeyValue = (*thisHash[i])[j];
						(*tempHash[newIndex]).push_back(myBabyKeyValue);
					}

				}
				delete thisHash[i];
			}
			delete[] thisHash;
			thisHash = tempHash;
			m_buckets *= 2;
		}

	

}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType, ValueType>::find(const KeyType& key) const
{
	unsigned int hasher(const KeyType & k);
	unsigned int hashedVal = hasher(key);
	unsigned int index = hashedVal % m_buckets;
	std::vector <KeyValue>* vec_ptr = thisHash[index];
	if (vec_ptr != nullptr)
	{
		for (int i = 0; i < (*vec_ptr).size(); i++)
		{
			if ((*vec_ptr)[i].key == key)
			{
				const ValueType* ptr = &((*vec_ptr)[i].value);
				return ptr;
			}
		}
	}
	return nullptr;
	
}

#endif



