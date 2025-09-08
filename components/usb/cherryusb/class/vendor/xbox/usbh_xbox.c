/*
 * Copyright (c) 2024 Till Harbaum
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "usbh_core.h"
#include "usbh_xbox.h"

#define DEV_FORMAT "/dev/xbox%d"

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t g_xbox_buf[128];

static struct usbh_xbox g_xbox_class[CONFIG_USBHOST_MAX_XBOX_CLASS];
static uint32_t g_devinuse = 0;

static struct usbh_xbox *usbh_xbox_class_alloc(void)
{
    int devno;

    for (devno = 0; devno < CONFIG_USBHOST_MAX_XBOX_CLASS; devno++) {
        if ((g_devinuse & (1 << devno)) == 0) {
            g_devinuse |= (1 << devno);
            memset(&g_xbox_class[devno], 0, sizeof(struct usbh_xbox));
            g_xbox_class[devno].minor = devno;
            return &g_xbox_class[devno];
        }
    }
    return NULL;
}

static void usbh_xbox_class_free(struct usbh_xbox *xbox_class)
{
    int devno = xbox_class->minor;

    if (devno >= 0 && devno < 32) {
        g_devinuse &= ~(1 << devno);
    }
    memset(xbox_class, 0, sizeof(struct usbh_xbox));
}

int usbh_xbox_connect(struct usbh_hubport *hport, uint8_t intf)
{
    struct usb_endpoint_descriptor *ep_desc;

    struct usbh_xbox *xbox_class = usbh_xbox_class_alloc();
    if (xbox_class == NULL) {
        USB_LOG_ERR("Fail to alloc xbox_class\r\n");
        return -USB_ERR_NOMEM;
    }

    xbox_class->hport = hport;
    xbox_class->intf = intf;

    hport->config.intf[intf].priv = xbox_class;

    for (uint8_t i = 0; i < hport->config.intf[intf].altsetting[0].intf_desc.bNumEndpoints; i++) {
        ep_desc = &hport->config.intf[intf].altsetting[0].ep[i].ep_desc;
        if (ep_desc->bEndpointAddress & 0x80) {
            USBH_EP_INIT(xbox_class->intin, ep_desc);
        } else {
            USBH_EP_INIT(xbox_class->intout, ep_desc);
        }
    }

    snprintf(hport->config.intf[intf].devname, CONFIG_USBHOST_DEV_NAMELEN, DEV_FORMAT, xbox_class->minor);

    USB_LOG_INFO("Register XBOX Class:%s\r\n", hport->config.intf[intf].devname);

    usbh_xbox_run(xbox_class);
    return 0;
}

int usbh_xbox_disconnect(struct usbh_hubport *hport, uint8_t intf)
{
    int ret = 0;

    struct usbh_xbox *xbox_class = (struct usbh_xbox *)hport->config.intf[intf].priv;

    if (xbox_class) {
        if (xbox_class->intin) {
            usbh_kill_urb(&xbox_class->intin_urb);
        }

        if (xbox_class->intout) {
            usbh_kill_urb(&xbox_class->intout_urb);
        }

        if (hport->config.intf[intf].devname[0] != '\0') {
            USB_LOG_INFO("Unregister XBOX Class:%s\r\n", hport->config.intf[intf].devname);
            usbh_xbox_stop(xbox_class);
        }

        usbh_xbox_class_free(xbox_class);
    }

    return ret;
}

__WEAK void usbh_xbox_run(struct usbh_xbox *xbox_class)
{
}

__WEAK void usbh_xbox_stop(struct usbh_xbox *xbox_class)
{
}

const struct usbh_class_driver xbox_class_driver = {
    .driver_name = "xbox",
    .connect = usbh_xbox_connect,
    .disconnect = usbh_xbox_disconnect
};

CLASS_INFO_DEFINE const struct usbh_class_info xbox_custom_class_info = {
    .match_flags = USB_CLASS_MATCH_INTF_CLASS| USB_CLASS_MATCH_INTF_SUBCLASS | USB_CLASS_MATCH_INTF_PROTOCOL,
    .class = USB_DEVICE_CLASS_VEND_SPECIFIC,
    .subclass = 0x5d,
    .protocol = 0x01,
    .vid = 0x00,
    .pid = 0x00,
    .class_driver = &xbox_class_driver
};
