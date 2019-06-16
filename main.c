#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lv_examples/lv_apps/demo/demo.h"
#include "lv_examples/lv_tests/lv_test_group/lv_test_group.h"
#include <unistd.h>

#include "lv_mister/window.h"

int main(void)
{
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*Add a display the LittlevGL sing the frame buffer driver*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.disp_flush = fbdev_flush;      /*It flushes the internal graphical buffer to the frame buffer*/
    lv_disp_drv_register(&disp_drv);


    /*Create a Demo*/
    //demo_create();

//lv_test_group_1();
create_main_window( );
        // enable event input
        evdev_init();

        // get an input device like mouse
        lv_indev_drv_t indev_drv;
        lv_indev_drv_init(&indev_drv);
        indev_drv.type = LV_INDEV_TYPE_POINTER;
        indev_drv.read = evdev_read_mouse;
        // get an input device like mouse
	//
        lv_indev_drv_t joy_indev_drv;
        lv_indev_drv_init(&joy_indev_drv);
        joy_indev_drv.type = LV_INDEV_TYPE_ENCODER;
        joy_indev_drv.read = evdev_read_joystick;
      lv_indev_drv_register(&joy_indev_drv);

    lv_indev_t * mouse_indev = lv_indev_drv_register(&indev_drv);
    lv_obj_t * cursor_obj =  lv_img_create(lv_scr_act(), NULL); /*Create an image for the cursor */
    lv_img_set_src(cursor_obj, SYMBOL_POWER);                 /*For simlicity add a built in symbol not an image*/
    lv_indev_set_cursor(mouse_indev, cursor_obj); /* connect the object to the driver*/

#if 0
    lv_indev_drv_t rael_kb_drv;
    rael_kb_drv.type = LV_INDEV_TYPE_KEYPAD;
    //rael_kb_drv.read =  keyboard_read;
    rael_kb_drv.read =  evdev_read_keyboard;
    lv_indev_t * real_kb_indev = lv_indev_drv_register(&rael_kb_drv);
#endif

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_tick_inc(5);
        lv_task_handler();
        usleep(5000);
    }

    return 0;
}
