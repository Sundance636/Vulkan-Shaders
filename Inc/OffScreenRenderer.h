#pragma once

#include "coreDevice.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>
#include <memory>
#include <array>

// only every going to be dealing with one image at a time
class OffScreenRenderer {

    public:
        OffScreenRenderer(coreDevice &deviceRef, VkExtent2D windowExtent);
        OffScreenRenderer(coreDevice &deviceRef, VkExtent2D extent, std::shared_ptr<OffScreenRenderer> previous);

        ~OffScreenRenderer();

        VkFormat findDepthFormat();
          static constexpr int MAX_FRAMES_IN_FLIGHT = 2;


    private:
        void createOffScreenRenderer();
        void createImageViews();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();
        void init();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        VkFormat offScreenImageFormat;
        VkFormat offScreenDepthFormat;
        VkExtent2D offScreenExtent;

        VkFramebuffer offScreenFramebuffer;
        VkRenderPass offScreenRenderPass;

        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;
        VkImage offScreenImage;
        VkDeviceMemory offScreenImageMemory;
        VkImageView offScreenImageView;

        coreDevice &device;
        VkExtent2D windowExtent;

        //VkoffScreenKHR offScreen;
        std::shared_ptr<OffScreenRenderer> oldoffScreen;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;


};