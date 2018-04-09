/*
 * Copyright (C) 2018 The University of Notre Dame This software is
 * distributed under the GNU General Public License. See the file LICENSE
 * for details. 
 */

#include "kobject.h"
#include "kmalloc.h"
#include "fs.h"
#include "device.h"

struct kobject *kobject_create_file(struct fs_file *f) {
    struct kobject *k = kmalloc(sizeof(*k));
    k->type = FILE;
    k->rc = 0;
    k->data.file = f;
    return k;
}

struct kobject *kobject_create_device(struct device *d){ 
    struct kobject *k = kmalloc(sizeof(*k));
    k->type = DEVICE;
    k->rc = 0;
    k->data.device = d;
    return k;
}

struct kobject *kobject_create_graphics(struct graphics *g){ 
    struct kobject *k = kmalloc(sizeof(*k));
    k->type = GRAPHICS;
    k->rc = 0;
    k->data.graphics = g;
    return k;
}

int kobject_read(struct kobject *kobject, void *buffer, int size) {
    switch (kobject->type) {
        case INVALID: return 0;
        case GRAPHICS: return 0;
        case FILE:
            return fs_file_read(kobject->data.file, (char*)buffer, (uint32_t)size);
        case DEVICE:
            return device_read(kobject->data.device, buffer, size/kobject->data.device->block_size, 0);
    }
    return 0;
}

int kobject_write(struct kobject *kobject, void *buffer, int size) {
    switch (kobject->type) {
        case INVALID: return 0;
        case GRAPHICS:
            return graphics_object_write((struct graphics_command*)buffer, kobject->data.graphics);
        case FILE:
            return fs_file_write(kobject->data.file, (char*)buffer, (uint32_t)size);
        case DEVICE:
            return device_write(kobject->data.device, buffer, size/kobject->data.device->block_size, 0);
    }
    return 0;
}

int kobject_close(struct kobject *kobject) {
    int ret;
    if (--kobject->rc <= 0) {
        switch (kobject->type) {
        case INVALID: return 0;
        case GRAPHICS: return 0;
            case FILE:
                ret = fs_file_close(kobject->data.file);
                kfree(kobject);
                return ret;
            case DEVICE:
                kfree(kobject);
                return 0;
        }
    }
    return 0;
}

