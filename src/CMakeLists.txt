target_sources(${PROJECT_NAME} PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/engine/buffers.cpp   
${CMAKE_CURRENT_SOURCE_DIR}/engine/vertex.cpp              
${CMAKE_CURRENT_SOURCE_DIR}/engine/model.cpp       
${CMAKE_CURRENT_SOURCE_DIR}/engine/config_pipeline.cpp
${CMAKE_CURRENT_SOURCE_DIR}/engine/command_buffers.cpp  
${CMAKE_CURRENT_SOURCE_DIR}/engine/window.cpp              
${CMAKE_CURRENT_SOURCE_DIR}/engine/pipeline.cpp    
${CMAKE_CURRENT_SOURCE_DIR}/engine/interface.cpp
${CMAKE_CURRENT_SOURCE_DIR}/engine/device.cpp           
${CMAKE_CURRENT_SOURCE_DIR}/engine/swap_chain.cpp  
${CMAKE_CURRENT_SOURCE_DIR}/engine/draw_frame.cpp       
${CMAKE_CURRENT_SOURCE_DIR}/engine/config_device.cpp
${CMAKE_CURRENT_SOURCE_DIR}/engine/cleanup.cpp
${CMAKE_CURRENT_SOURCE_DIR}/engine/fileIO.cpp
${CMAKE_CURRENT_SOURCE_DIR}/engine/config_buffers.cpp
${CMAKE_CURRENT_SOURCE_DIR}/engine/images.cpp
${CMAKE_CURRENT_SOURCE_DIR}/engine/descriptor_sets.cpp
${CMAKE_CURRENT_SOURCE_DIR}/engine/transform.cpp
${CMAKE_CURRENT_SOURCE_DIR}/engine/main_loader.cpp
${CMAKE_CURRENT_SOURCE_DIR}/engine/entity.cpp
${CMAKE_CURRENT_SOURCE_DIR}/engine/error_handling.cpp
)
target_sources(testbin PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/testbin/Main.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/engine)
