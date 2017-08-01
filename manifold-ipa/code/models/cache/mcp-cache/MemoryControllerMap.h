#ifndef MANIFOLD_MCP_CACHE_MEMORYCONTROLLERMAP_H
#define MANIFOLD_MCP_CACHE_MEMORYCONTROLLERMAP_H

#include "uarch/DestMap.h"

#include <assert.h>
#include <vector>

namespace manifold {
namespace mcp_cache_namespace {


class PageBasedMap : public manifold::uarch::DestMap {
public:
    PageBasedMap(std::vector<int>& nodes, int page_offset_bits) : m_page_offset_bits(page_offset_bits)
    {
	assert(nodes.size() > 0);

	for(unsigned i=0; i<nodes.size(); i++)
	    m_nodes.push_back(nodes[i]);

	int bits_for_mask = myLog2(nodes.size());

        m_selector_mask = (0x1 << bits_for_mask) - 1;
    }

    int lookup(uint64_t addr)
    {
        return m_nodes[m_selector_mask & (addr >> m_page_offset_bits)];
    }

    int get_idx(uint64_t m_id)
	{
		for (unsigned int i = 0; i < m_nodes.size(); i++)
		{
			if (m_nodes[i] == m_id)
			{
#ifdef HMCDEBUG
			std::cerr << "VaultMap get_idx: " << i << std::endl;
#endif
				return i;
			}
		}

		assert(0);
	}

    uint64_t get_local_addr(uint64_t addr)
	{
		uint64_t up_addr = addr >> (m_page_offset_bits + m_selector_bits);
		uint64_t low_addr = addr & m_page_offset_mask;
		uint64_t laddr = (up_addr << m_page_offset_bits) | low_addr;

#ifdef HMCDEBUG
		std::cerr << "VaultMap addr: " << std::hex << addr << " laddr " << laddr
				<< std::dec << " m_page_offset_bits " << m_page_offset_bits << " m_selector_bits " << m_selector_bits
				<< " m_page_offset_mask " << std::hex << m_page_offset_mask << std::dec << std::endl;
#endif

		return laddr;
	}

	//fixme: assume the cache offset field is less than a page
	uint64_t get_global_addr(uint64_t addr, uint64_t m_id)
	{
		int idx = get_idx(m_id);

		uint64_t up_addr = addr >> m_page_offset_bits;
		uint64_t low_addr = (addr & m_page_offset_mask) | idx << (m_page_offset_bits);
		uint64_t gaddr = (up_addr << (m_page_offset_bits + m_selector_bits)) | low_addr;

#ifdef HMCDEBUG
		std::cerr << "VaultMap addr: " << std::hex << addr << " gaddr " << gaddr << " idx " << std::dec << idx << std::endl;
#endif
		return  gaddr;
	}

	int get_page_offset_bits()
	{
		return m_page_offset_bits;
	}

	int get_selector_bits()
	{
		return m_selector_bits;
	}

	uint64_t get_selector_mask()
	{
		return m_selector_mask;
	}

	int get_nodes_size()
	{
		return m_nodes.size();
	}
private:

    static int myLog2(unsigned num)
    {
	assert(num > 0);

	int bits = 0;
	while(((unsigned)0x1 << bits) < num) {
	    bits++;
	}
	return bits;
    }

    std::vector<int> m_nodes; //target node ids
    const int m_page_offset_bits;
    uint64_t m_selector_mask;
    int m_selector_bits;
    uint64_t m_page_offset_mask;
};


} //namespace mcp_cache_namespace
} //namespace manifold

#endif //MANIFOLD_MCP_CACHE_MEMORYCONTROLLERMAP_H
