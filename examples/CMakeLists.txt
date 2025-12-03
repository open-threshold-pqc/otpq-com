file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS "*.cpp")

foreach(example_src ${EXAMPLE_SOURCES})
    get_filename_component(example_name ${example_src} NAME_WE)

    add_executable(${example_name} ${example_src})
    target_link_libraries(${example_name} PRIVATE libotpqcom otpqcom_project_options)

    message(STATUS "Configured OTPQCOM Example: ${example_name}")
endforeach()
