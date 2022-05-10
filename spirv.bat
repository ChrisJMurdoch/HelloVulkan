:: Compile GLSL source into SPIR-V bytecode
if not exist ".\shaders\bin\" mkdir ".\shaders\bin\"
C:/VulkanSDK/1.3.211.0/Bin/glslc.exe shaders/src/shader.vert -o shaders/bin/shader.vert.spv
C:/VulkanSDK/1.3.211.0/Bin/glslc.exe shaders/src/shader.frag -o shaders/bin/shader.frag.spv