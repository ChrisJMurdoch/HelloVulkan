
#include "swapchain/pipeline.hpp"

#include "configuration/device.hpp"
#include "configuration/shaderModule.hpp"
#include "swapchain/renderPass.hpp"
#include "vertex/vertex.hpp"
#include "memory/descriptorSetLayout.hpp"
#include "utility/check.hpp"

#include <vector>

Pipeline::Pipeline(Device const *device, ShaderModule const &vertShaderModule, ShaderModule const &fragShaderModule, RenderPass const *renderPass, VkExtent2D const &viewportExtent, DescriptorSetLayout const *descriptorSetLayout) : device(device)
{
    // Specify shader stages
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages
    {
        VkPipelineShaderStageCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertShaderModule.getHandle(),
            .pName = "main"
        },
        VkPipelineShaderStageCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragShaderModule.getHandle(),
            .pName = "main"
        }
    };

    // Specify vertex input state
    VkVertexInputBindingDescription vertexBindingDescription = Vertex::getBindingDescription();
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions = Vertex::getAttributeDescriptions();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vertexBindingDescription,
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size()),
        .pVertexAttributeDescriptions = vertexAttributeDescriptions.data()
    };

    // Specify input assembly state
    VkPipelineInputAssemblyStateCreateInfo inputAssembly
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    // Specify viewport state
    VkViewport viewport
    {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float) viewportExtent.width,
        .height = (float) viewportExtent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };
    VkRect2D scissor
    {
        .offset = {0, 0},
        .extent = viewportExtent
    };
    VkPipelineViewportStateCreateInfo viewportState
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor
    };

    // Specify rasterization state
    VkPipelineRasterizationStateCreateInfo rasterizer
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .lineWidth = 1.0f
    };

    // Specify multisample state
    VkPipelineMultisampleStateCreateInfo multisampling
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE
    };

    // Specify colour blending    
    VkPipelineColorBlendAttachmentState colourBlendAttachment
    {
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
    };
    VkPipelineColorBlendStateCreateInfo colorBlending
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colourBlendAttachment,
        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
    };

    // Create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &descriptorSetLayout->getHandle()
    };
    check::fail( vkCreatePipelineLayout(device->getHandle(), &pipelineLayoutInfo, nullptr, &pipelineLayout),  "vkCreatePipelineLayout failed.");

    // Create pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = static_cast<uint32_t>(shaderStages.size()),
        .pStages = shaderStages.data(),
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pColorBlendState = &colorBlending,
        .layout = pipelineLayout,
        .renderPass = renderPass->getHandle(),
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE
    };
    check::fail( vkCreateGraphicsPipelines(device->getHandle(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &handle), "vkCreateGraphicsPipelines failed." );
}

Pipeline::~Pipeline()
{
    vkDestroyPipeline(device->getHandle(), handle, nullptr);
    vkDestroyPipelineLayout(device->getHandle(), pipelineLayout, nullptr);
}

VkPipeline const &Pipeline::getHandle() const
{
    return handle;
}

VkPipelineLayout const &Pipeline::getLayout() const
{
    return pipelineLayout;
}
