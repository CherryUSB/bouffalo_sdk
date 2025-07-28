cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_compile_definitions(-DCONFIG_USBHOST_PLATFORM_CDC_ECM)
sdk_add_compile_definitions(-DCONFIG_USBHOST_PLATFORM_CDC_NCM)
sdk_add_compile_definitions(-DCONFIG_USBHOST_PLATFORM_CDC_RNDIS)
sdk_add_compile_definitions(-DCONFIG_USBHOST_PLATFORM_ASIX)
sdk_add_compile_definitions(-DCONFIG_USBHOST_PLATFORM_RTL8152)
sdk_add_include_directories(.)

target_sources(app PRIVATE usb_host.c cherryusb/demo/video_static_mjpeg_template.c)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_CHERRYUSB_DEVICE 1)
set(CONFIG_CHERRYUSB_HOST 1)

add_subdirectory(cherryusb)
sdk_set_main_file(main.c)
sdk_set_linker_script(bl616_flash.ld)
project(cherryusb)
