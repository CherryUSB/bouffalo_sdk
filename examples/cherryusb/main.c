#include <FreeRTOS.h>
#include "semphr.h"
#include "usbh_core.h"
#include "bflb_uart.h"
#include "board.h"
#include "shell.h"
#include "lwip/tcpip.h"

extern void usbh_class_test(void);

static struct bflb_device_s *uart0;

extern void shell_init_with_task(struct bflb_device_s *shell);

int main(void)
{
    board_init();

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    /* Initialize the LwIP stack */
    tcpip_init(NULL, NULL);

    printf("Starting usb host task...\r\n");
    usbh_initialize(0, 0x20072000);

    vTaskStartScheduler();

    while (1) {
    }
}

int usbh_deinit(int argc, char **argv)
{
    printf("usbh_deinit\r\n");
    usbh_deinitialize(0);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(usbh_deinit, usbh_deinit, usbh deinit);

int usbh_init(int argc, char **argv)
{
    printf("usbh_init\r\n");
    usbh_initialize(0, 0x20072000);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(usbh_init, usbh_init, usbh init);

SHELL_CMD_EXPORT_ALIAS(lsusb, lsusb, ls usb);

// extern int bluetooth_demo_init(int argc, char **argv);

// SHELL_CMD_EXPORT_ALIAS(bluetooth_demo_init, bluetooth_demo_init, bluetooth_demo_init);
int uvcinit(int argc, char **argv)
{
    video_init(0, 0x20072000);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(uvcinit, uvcinit, uvcinit);

int uvcsend(int argc, char **argv)
{
    extern void video_test(uint8_t busid);
    video_test(0);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(uvcsend, uvcsend, uvcsend);
