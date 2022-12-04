#pragma once
#include <iostream>
#include <list>
#include <vector>
#include <unordered_map>

namespace engine
{
    template <typename T>
    struct Ref
    {
        Ref() = default;

        static Ref<T> makeEmpty() {
            return Ref(0, 0);
        }
    private:
        Ref(uint32_t i, uint32_t b) : m_pIndex(i), m_pGeneration(b) {}

        bool isValid() { return m_pGeneration != 0; }

        uint32_t m_pIndex = 0;
        uint32_t m_pGeneration = 0;

        template <typename A, typename B>
        friend class Pool;
    };

    template <typename T, typename R>
    struct Pool
    {
        using InternalDataArray = std::list<R>;
        using FreeRefArray = std::list<Ref<T>>;
        using GenerationArray = std::unordered_map<unsigned int, uint32_t>;
        using IndexArray = std::vector<typename std::list<R>::iterator>;

        template <typename... Args>
        Ref<T> emplace(Args... args)
        {
            if (m_pFreeRefs.size() > 0)
            {
                Ref<T> ref = m_pFreeRefs.back();
                m_pFreeRefs.pop_back();

                ref.m_pGeneration = ++m_pGeneration[ref.m_pIndex];
                auto it = m_pInternalData.emplace(m_pIndexArray.at(ref.m_pIndex),
                                                  args...);
                m_pIndexArray.at(ref.m_pIndex) = it;
                std::cout << "reused reference\n";
                return ref;
            }

            m_pInternalData.emplace_back(args...);
            m_pIndexArray.push_back(std::prev(m_pInternalData.end()));

            uint32_t index = m_pIndexArray.size() - 1;

            Ref<T> ref(index, 1);
            m_pGeneration[index] = 1;

            return ref;
        }
        Ref<T> insert(R element)
        {
            if (m_pFreeRefs.size() > 0)
            {
                Ref<T> ref = m_pFreeRefs.back();
                m_pFreeRefs.pop_back();

                ref.m_pGeneration = ++m_pGeneration[ref.m_pIndex];

                m_pInternalData.insert(m_pIndexArray.at(ref.m_pIndex), element);
                return ref;
            }

            m_pInternalData.push_back(element);
            m_pIndexArray.push_back(std::prev(m_pInternalData.end()));
            uint32_t index = m_pIndexArray.size() - 1;
            Ref<T> ref(index, 1);
            m_pGeneration[index] = 1;

            return ref;
        }

        [[nodiscard]] R *get(Ref<T> ref)
        {
            if (m_pIndexArray.size() > ref.m_pIndex && ref.isValid())
            {
                if (m_pGeneration[ref.m_pIndex] == ref.m_pGeneration)
                {
                    return &(*m_pIndexArray.at(ref.m_pIndex));
                }
            }
            std::cout << "object is invalid\n";

            return nullptr;
        }

        void destroy(Ref<T> &ref)
        {
            std::cout << "internal data size before: " << m_pInternalData.size()
                      << std::endl;
            if (m_pIndexArray.size() > ref.m_pIndex)
            {
                if (ref.m_pGeneration ==
                    m_pGeneration[ref.m_pIndex])
                { // we dont want to add this free
                  // reference twice
                    m_pFreeRefs.insert(m_pFreeRefs.end(), ref);
                    m_pInternalData.erase(m_pIndexArray.at(ref.m_pIndex));
                    ref.m_pGeneration = 0; // invalidate reference
                }
            }

            std::cout << "internal data size after: " << m_pInternalData.size()
                      << std::endl;
        }

        auto begin()
        {
            return m_pInternalData.begin();
        }

        auto end() {
            return m_pInternalData.end(); 
        }

    private:
        GenerationArray m_pGeneration;
        IndexArray m_pIndexArray;
        InternalDataArray m_pInternalData;
        FreeRefArray m_pFreeRefs;
    };

}