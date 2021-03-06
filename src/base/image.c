﻿/**
 * File:   image.h
 * Author: AWTK Develop Team
 * Brief:  image
 *
 * Copyright (c) 2018 - 2018  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2018-01-28 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "base/mem.h"
#include "base/enums.h"
#include "base/image.h"
#include "base/utils.h"
#include "base/image_manager.h"

static ret_t image_on_paint_self(widget_t* widget, canvas_t* c) {
  bitmap_t bitmap;
  image_t* image = IMAGE(widget);
  vgcanvas_t* vg = lcd_get_vgcanvas(c->lcd);
  image_base_t* image_base = IMAGE_BASE(widget);
  return_value_if_fail(widget != NULL && c != NULL, RET_BAD_PARAMS);

  if (image_base->image == NULL) {
    return RET_OK;
  }

  return_value_if_fail(widget_load_image(widget, image_base->image, &bitmap) == RET_OK,
                       RET_BAD_PARAMS);

  if (vg != NULL) {
    if (image_need_transform(widget)) {
      vgcanvas_save(vg);
      image_transform(widget, c);
      vgcanvas_draw_icon(vg, &bitmap, 0, 0, bitmap.w, bitmap.h, 0, 0, widget->w, widget->h);
      vgcanvas_restore(vg);

      return RET_OK;
    }
  }

  if (bitmap.data != NULL) {
    rect_t dst = rect_init(0, 0, widget->w, widget->h);
    canvas_draw_image_ex(c, &bitmap, image->draw_type, &dst);
  }

  widget_paint_helper(widget, c, NULL, NULL);

  return RET_OK;
}

static ret_t image_get_prop(widget_t* widget, const char* name, value_t* v) {
  image_t* image = IMAGE(widget);
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_DRAW_TYPE)) {
    value_set_int(v, image->draw_type);
    return RET_OK;
  } else {
    return image_base_get_prop(widget, name, v);
  }
}

static ret_t image_set_prop(widget_t* widget, const char* name, const value_t* v) {
  return_value_if_fail(widget != NULL && name != NULL && v != NULL, RET_BAD_PARAMS);

  if (tk_str_eq(name, WIDGET_PROP_DRAW_TYPE)) {
    if (v->type == VALUE_TYPE_STRING) {
      const key_type_value_t* kv = image_draw_type_find(value_str(v));
      if (kv != NULL) {
        return image_set_draw_type(widget, (image_draw_type_t)(kv->value));
      }
    } else {
      return image_set_draw_type(widget, (image_draw_type_t)value_int(v));
    }

    return RET_OK;
  } else {
    return image_base_set_prop(widget, name, v);
  }
}

static const char* s_image_clone_properties[] = {WIDGET_PROP_IMAGE,      WIDGET_PROP_DRAW_TYPE,
                                                 WIDGET_PROP_SCALE_X,    WIDGET_PROP_SCALE_Y,
                                                 WIDGET_PROP_ANCHOR_X,   WIDGET_PROP_ANCHOR_Y,
                                                 WIDGET_PROP_ROTATION,   WIDGET_PROP_CLICKABLE,
                                                 WIDGET_PROP_SELECTABLE, NULL};

static const widget_vtable_t s_image_vtable = {.size = sizeof(image_t),
                                               .type = WIDGET_TYPE_IMAGE,
                                               .clone_properties = s_image_clone_properties,
                                               .create = image_create,
                                               .destroy = image_base_destroy,
                                               .on_event = image_base_on_event,
                                               .on_paint_self = image_on_paint_self,
                                               .set_prop = image_set_prop,
                                               .get_prop = image_get_prop};

widget_t* image_create(widget_t* parent, xy_t x, xy_t y, wh_t w, wh_t h) {
  image_t* image = TKMEM_ZALLOC(image_t);
  widget_t* widget = WIDGET(image);
  return_value_if_fail(image != NULL, NULL);

  widget_init(widget, parent, &s_image_vtable, x, y, w, h);
  image_base_init(widget);
  image->draw_type = IMAGE_DRAW_DEFAULT;

  return widget;
}

ret_t image_set_draw_type(widget_t* widget, image_draw_type_t draw_type) {
  image_t* image = IMAGE(widget);
  return_value_if_fail(widget != NULL, RET_BAD_PARAMS);

  image->draw_type = draw_type;

  return widget_invalidate(widget, NULL);
}

widget_t* image_cast(widget_t* widget) {
  return_value_if_fail(widget != NULL && widget->vt == &s_image_vtable, NULL);

  return widget;
}
