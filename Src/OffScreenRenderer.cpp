#include "OffScreenRenderer.h"

OffScreenRenderer::OffScreenRenderer(coreDevice &deviceRef, VkExtent2D extent) 
    : device{deviceRef}, windowExtent{extent} {
    init();
    isFrameStarted = false;
    currentImageIndex = 0;
    currentFrameIndex = 0;
    }


//for keeping up with swapchain resizes
OffScreenRenderer::OffScreenRenderer(coreDevice &deviceRef, VkExtent2D extent, std::shared_ptr<OffScreenRenderer> previous) 
  : device{deviceRef}, windowExtent{extent}, oldoffScreen{previous} {
    init();
    oldoffScreen = nullptr;
}

OffScreenRenderer::~OffScreenRenderer() {

  // Color attachment
  vkDestroyImageView(device.device(), offScreenImageView, nullptr);
  vkDestroyImage(device.device(), offScreenImage, nullptr);
  vkFreeMemory(device.device(), offScreenImageMemory, nullptr);

  // vkDestroyBuffer(device.device(), offscreenPass.color.staging, nullptr);
  vkDestroyFramebuffer(device.device(), offScreenFramebuffer, nullptr);

  vkDestroyRenderPass(device.device(), offScreenRenderPass, nullptr);

  // Depth attachment
  vkDestroyImageView(device.device(), depthImageView, nullptr);
  vkDestroyImage(device.device(), depthImage, nullptr);
  vkFreeMemory(device.device(), depthImageMemory, nullptr);
  freeCommandBuffers();
}

void OffScreenRenderer::init() {
  offScreenExtent = windowExtent;
      createCommandBuffers();
    createOffScreenRenderer();
    createImageViews();
    createRenderPass();
    createDepthResources();
    createFramebuffers();
    //cdcreateSyncObjects();
}

//creating Images
void OffScreenRenderer::createOffScreenRenderer() {
    //Color attachment
    VkImageCreateInfo image{};
    image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image.imageType = VK_IMAGE_TYPE_2D;
    image.format = VK_FORMAT_R8G8B8A8_UNORM;// VK_FORMAT_B8G8R8A8_SRGB;//mayebe change
    image.extent.width = windowExtent.width;
    image.extent.height = windowExtent.height;
    image.extent.depth = 1;
    image.mipLevels = 1;
    image.arrayLayers = 1;
    image.samples = VK_SAMPLE_COUNT_1_BIT;//must match the swapchain
    image.tiling = VK_IMAGE_TILING_OPTIMAL;

    // We will sample directly from the color attachment
    image.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT;

        device.createImageWithInfo(
        image,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        offScreenImage,
        offScreenImageMemory);

}

//required imageviews
void OffScreenRenderer::createImageViews() {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = offScreenImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format =  VK_FORMAT_R8G8B8A8_UNORM;//VK_FORMAT_B8G8R8A8_SRGB;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;



    if (vkCreateImageView(device.device(), &viewInfo, nullptr, &offScreenImageView) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create texture image view!");
    }
}

void OffScreenRenderer::createRenderPass() {

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format =  VK_FORMAT_R8G8B8A8_UNORM;//VK_FORMAT_B8G8R8A8_SRGB;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstSubpass = 0;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device.device(), &renderPassInfo, nullptr, &offScreenRenderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create offscreen render pass!");
    }
}

void OffScreenRenderer::createDepthResources() {
    VkFormat depthFormat = findDepthFormat();
    // Color attachment
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = windowExtent.width;
    imageInfo.extent.height = windowExtent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = depthFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    device.createImageWithInfo(
        imageInfo,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depthImage,
        depthImageMemory);

    // Depth stencil attachment
    VkImageViewCreateInfo depthStencilView{};
    depthStencilView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depthStencilView.image = depthImage;
    depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthStencilView.format = depthFormat;
    depthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (depthFormat >= VK_FORMAT_D16_UNORM_S8_UINT) {
			depthStencilView.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
    depthStencilView.subresourceRange.baseMipLevel = 0;
    depthStencilView.subresourceRange.levelCount = 1;
    depthStencilView.subresourceRange.baseArrayLayer = 0;
    depthStencilView.subresourceRange.layerCount = 1;

    if (vkCreateImageView(device.device(), &depthStencilView, nullptr, &depthImageView) != VK_SUCCESS) {
      throw std::runtime_error("failed to create texture image view!");
    }
}

void OffScreenRenderer::createFramebuffers() {
  std::array<VkImageView, 2> attachments = {offScreenImageView, depthImageView};

  VkFramebufferCreateInfo framebufferInfo = {};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = offScreenRenderPass;
  framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  framebufferInfo.pAttachments = attachments.data();
  framebufferInfo.width = windowExtent.width;
  framebufferInfo.height = windowExtent.height;
  framebufferInfo.layers = 1;

  if (vkCreateFramebuffer(
          device.device(),
          &framebufferInfo,
          nullptr,
          &offScreenFramebuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to create framebuffer!");
  }
}
/*
void OffScreenRenderer::createSyncObjects() {
  imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
  imagesInFlight.resize(0, VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(device.device(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) !=
            VK_SUCCESS ||
        vkCreateSemaphore(device.device(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) !=
            VK_SUCCESS ||
        vkCreateFence(device.device(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
  }
}
*/

VkCommandBuffer OffScreenRenderer::getCurrentCommandBuffer() const { 
    assert( isFrameStarted && "Cannot get command buffer when frame not in progress!");
    return commandBuffers[currentFrameIndex];
}


VkRenderPass OffScreenRenderer::getoffRenderPass() const { 
    return offScreenRenderPass; 
}


VkFormat OffScreenRenderer::findDepthFormat() {
  return device.findSupportedFormat(
      {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
      VK_IMAGE_TILING_OPTIMAL,
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}


void OffScreenRenderer::beginOffRenderPass(VkCommandBuffer& commandBuffer) {
    assert(isFrameStarted && "Can't call beginoffRenderPass if frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin renderpass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = offScreenRenderPass;
    renderPassInfo.framebuffer = offScreenFramebuffer;

    renderPassInfo.renderArea.offset = {0,0};
    renderPassInfo.renderArea.extent = {offScreenExtent.width,offScreenExtent.height};

    std::vector<VkClearValue> clearValues = std::vector<VkClearValue>(2);

    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(offScreenExtent.width);
    viewport.height = static_cast<float>(offScreenExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, offScreenExtent};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void OffScreenRenderer::endOffRenderPass(VkCommandBuffer& commandBuffer) {
    assert(isFrameStarted && "Can't call endOffRenderPass if frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() && "Can't end renderpass on command buffer from a different frame");

    vkCmdEndRenderPass(commandBuffer);

}

VkCommandBuffer OffScreenRenderer::beginFrame() {
    assert(!isFrameStarted && "Can't call beginFrame while already in progress");

 
    isFrameStarted = true;
    VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    return commandBuffer;
}

void OffScreenRenderer::endFrame() {
    assert(isFrameStarted && "Can't call endFrame while frame is not in progress");

    VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }


    isFrameStarted = false;
    currentFrameIndex = (currentFrameIndex + 1) % 1;
}

void OffScreenRenderer::createCommandBuffers() {
    VkCommandBufferAllocateInfo allocInfo{};

    commandBuffers.resize(1);
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;//primary may be subimted for execution, not called by other command buffers
    allocInfo.commandPool = device.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

     if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

}

void OffScreenRenderer::freeCommandBuffers() {
    vkFreeCommandBuffers(device.device(), device.getCommandPool(),  static_cast<uint32_t>(commandBuffers.size()),commandBuffers.data() );
    commandBuffers.clear();

}

void OffScreenRenderer::transitionImgLayout(VkCommandBuffer& cmdBuffer,VkImageLayout oldLayout,VkImageLayout newLayout) {


    VkImageMemoryBarrier memoryBarrier{};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memoryBarrier.oldLayout = oldLayout;
    memoryBarrier.newLayout = newLayout;
    memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memoryBarrier.image = offScreenImage;
    memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    memoryBarrier.subresourceRange.baseMipLevel = 0;
    memoryBarrier.subresourceRange.levelCount = 1;
    memoryBarrier.subresourceRange.baseArrayLayer = 0;
    memoryBarrier.subresourceRange.layerCount = 1;
    memoryBarrier.srcAccessMask = 0;
    memoryBarrier.dstAccessMask = 0;
  
  
  if (oldLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_GENERAL) {
        memoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // Previous access
        memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT; // Compute shader access
    } else {
        // Handle other cases as needed
    }


    vkCmdPipelineBarrier(cmdBuffer,VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0,0,nullptr,0,nullptr, 1,&memoryBarrier);


}

void OffScreenRenderer::bindDescriptors(VkDescriptorSet& computeDesc) {
    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageView = offScreenImageView;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = computeDesc;
    descriptorWrite.dstBinding = 0;  // Must match the binding index in the shader
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device.device(), 1, &descriptorWrite, 0, nullptr);

}