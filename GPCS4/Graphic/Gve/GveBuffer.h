#pragma once

#include "GveCommon.h"
#include "GveMemory.h"
#include "GveDescriptor.h"
#include "../Gnm/GnmBuffer.h"

namespace gve
{;

class GveDevice;

struct GveBufferCreateInfo
{
	GnmBuffer buffer;
	VkBufferUsageFlags usage;
};

class GveBuffer : public RcObject
{

public:
	GveBuffer(const RcPtr<GveDevice>& device,
		const GveBufferCreateInfo& createInfo,
		GveMemoryAllocator&  memAlloc,
		VkMemoryPropertyFlags memFlags);
	~GveBuffer();

	VkBuffer handle() const;

	const GnmBuffer* getGnmBuffer() const;

private:
	void convertCreateInfo(const GveBufferCreateInfo& gveInfo,
		VkBufferCreateInfo& vkInfo);

private:
	RcPtr<GveDevice> m_device;
	GveBufferCreateInfo m_info;
	GveMemoryAllocator* m_memAlloc;
	VkMemoryPropertyFlags m_memFlags;

	VkBuffer m_buffer = VK_NULL_HANDLE;
	GveMemory m_memory;
};


class GveBufferView : public RcObject
{
public:
	GveBufferView();
	~GveBufferView();

private:

};



}  // namespace gve