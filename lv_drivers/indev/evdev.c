/**
 * @file evdev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "evdev.h"
#if USE_EVDEV != 0

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/joystick.h>

#include "keymap.h"
#include "keyboard.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
int map(int x, int in_min, int in_max, int out_min, int out_max);

/**********************
 *  STATIC VARIABLES
 **********************/
int evdev_fd;
int evdev_root_x;
int evdev_root_y;
int evdev_button;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the evdev interface
 */
#if 0
void evdev_init(void)
{
    evdev_fd = open(EVDEV_NAME, O_RDWR | O_NOCTTY | O_NDELAY);
    if(evdev_fd == -1) {
        perror("unable open evdev interface:");
        return;
    }

    fcntl(evdev_fd, F_SETFL, O_ASYNC | O_NONBLOCK);

    evdev_root_x = 0;
    evdev_root_y = 0;
    evdev_button = LV_INDEV_STATE_REL;
}
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
int evdev_fd = -1;
int evdev_root_x;
int evdev_root_y;
int evdev_button;

int evdev_kbd_fd = -1;
int evdev_mouse0_fd = -1;
int evdev_mouse1_fd = -1;
int evdev_js0_fd = -1;
char event_kbd[9] = "";
char event_mouse0[9] = "";
char event_mouse1[9] = "";
char event_js0[9] = "";

/**
 * Initialize the evdev interface
 * note, instead of running a shell script, in future could use this C code:
 * https://github.com/kernc/logkeys/issues/89
 */
void evdev_init(void)
{
    printf("evdev_init\n");
    #if USE_EVDEV_KEYBOARD == 1
       char evdev_name_kbd[20] = "/dev/input/";
       /*note, instead of EV=120013, could test for "Handlers=kbd "*/
       char *command1 = "cat /proc/bus/input/devices | tr -d '|' | sed -e 's%^$%|%' | tr '\n' ' ' | tr '|' '\n' | grep 'input0 .*EV=120013 ' | head -n 1 | grep -Eo 'event[0-9]+'";
       FILE *pipe1 = popen(command1, "r");
       if (pipe1 != NULL) {
           fgets(event_kbd, 9, pipe1); /*ex: event1*/
           pclose(pipe1);
           if (event_kbd[0] != 0 ) {
               strcat(evdev_name_kbd, event_kbd);
	       evdev_name_kbd[strlen(evdev_name_kbd)-1]=0;
	       printf("evdev_name_kbd:  [%s]\n",evdev_name_kbd);
               evdev_kbd_fd = open(evdev_name_kbd, O_RDWR|O_NOCTTY|O_NDELAY);
               if (evdev_kbd_fd != -1) {
                   fcntl(evdev_kbd_fd, F_SETFL, O_ASYNC|O_NONBLOCK);
               }
	       }
       }
    #endif
       char evdev_name_js0[20] = "/dev/input/js0";
       //char *commandj0 = "cat /proc/bus/input/devices | tr -d '|' | sed -e 's%^$%|%' | tr '\n' ' ' | tr '|' '\n' | grep 'input0 .*Handlers=js0' | head -n 1 | grep -Eo 'event[0-9]+'";
       //FILE *pipej0 = popen(commandj0, "r");
       //if (pipej0 != NULL) {
       //    fgets(event_js0, 9, pipej0); /*ex: event0*/
       //    pclose(pipej0);
       //    if (event_js0[0] != 0) {
               //strcat(evdev_name_js0, "/dev/input/js0");
	       evdev_name_js0[strlen(evdev_name_js0)]=0;
	       printf("evdev_name_js0:  [%s]\n",evdev_name_js0);
               //evdev_js0_fd = open(evdev_name_js0, O_RDWR|O_NOCTTY|O_NDELAY);
               evdev_js0_fd = open(evdev_name_js0, O_RDONLY|O_NOCTTY|O_NDELAY);
               if (evdev_js0_fd != -1) {
                   fcntl(evdev_js0_fd, F_SETFL, O_ASYNC|O_NONBLOCK);
               }
	       //}
      // }
    
    #if USE_EVDEV_MOUSE == 1
       /*detect mouse0*/
       char evdev_name_mouse0[20] = "/dev/input/";
       char *command2 = "cat /proc/bus/input/devices | tr -d '|' | sed -e 's%^$%|%' | tr '\n' ' ' | tr '|' '\n' | grep 'input0 .*Handlers=mouse0 ' | head -n 1 | grep -Eo 'event[0-9]+'";
       FILE *pipe2 = popen(command2, "r");
       if (pipe2 != NULL) {
           fgets(event_mouse0, 9, pipe2); /*ex: event0*/
           pclose(pipe2);
           if (event_mouse0[0] != 0) {
               strcat(evdev_name_mouse0, event_mouse0);
	       evdev_name_mouse0[strlen(evdev_name_mouse0)-1]=0;
	       printf("evdev_name_mouse0:  [%s]\n",evdev_name_mouse0);
               evdev_mouse0_fd = open(evdev_name_mouse0, O_RDWR|O_NOCTTY|O_NDELAY);
               if (evdev_mouse0_fd != -1) {
                   fcntl(evdev_mouse0_fd, F_SETFL, O_ASYNC|O_NONBLOCK);
               }
	       }
       }
       /*detect mouse1*/
       char evdev_name_mouse1[20] = "/dev/input/";
       char *command3 = "cat /proc/bus/input/devices | tr -d '|' | sed -e 's%^$%|%' | tr '\n' ' ' | tr '|' '\n' | grep 'input0 .*Handlers=mouse1 ' | head -n 1 | grep -Eo 'event[0-9]+'";
       FILE *pipe3 = popen(command3, "r");
       if (pipe3 != NULL) {
           fgets(event_mouse1, 9, pipe3); /*ex: event2*/
           pclose(pipe3);
           if (event_mouse1[0] != 0) {
               strcat(evdev_name_mouse1, event_mouse1);
	       evdev_name_mouse1[strlen(evdev_name_mouse1)-1]=0;
	       printf("evdev_name_mouse1:  [%s]\n",evdev_name_mouse1);
               evdev_mouse1_fd = open(evdev_name_mouse1, O_RDWR|O_NOCTTY|O_NDELAY);
               if (evdev_mouse1_fd != -1) {
                   fcntl(evdev_mouse1_fd, F_SETFL, O_ASYNC|O_NONBLOCK);
               }
	       }
       }
    #endif
    
    /*old method with hard-coded event#*/
    if  (USE_EVDEV_KEYBOARD!=1 & USE_EVDEV_MOUSE!=1) {
        evdev_fd = open(EVDEV_NAME, O_RDWR|O_NOCTTY|O_NDELAY);
        if (evdev_fd != -1) {
            fcntl(evdev_fd, F_SETFL, O_ASYNC|O_NONBLOCK);
        }
    }

    /*error if no input devices found*/
    if (evdev_fd==-1 & evdev_kbd_fd==-1 & evdev_mouse0_fd==-1 & evdev_mouse1_fd==-1) {
        perror("unable open evdev interface");
        return;
    }
    
    evdev_root_x = 0;
    evdev_root_y = 0;
    evdev_button = LV_INDEV_STATE_REL;
}


/**
 * reconfigure the device file for evdev
 * @param dev_name set the evdev device filename
 * @return true: the device file set complete
 *         false: the device file doesn't exist current system
 */
bool evdev_set_file(char* dev_name)
{ 
     if(evdev_fd != -1) {
        close(evdev_fd);
     }
     printf("opening: %s\n",dev_name);
     evdev_fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);

     if(evdev_fd == -1) {
        perror("unable open evdev interface:");
        return false;
     }

     fcntl(evdev_fd, F_SETFL, O_ASYNC | O_NONBLOCK);

     evdev_root_x = 0;
     evdev_root_y = 0;
     evdev_button = LV_INDEV_STATE_REL;

     return true;
}
/**
 * Get the current position and state of the evdev
 * @param data store the evdev data here
 * @return false: because the points are not buffered, so no more data to be read
 */
void prtcode(int codes) {
    struct parse_key *p;

    for (p = keynames; p->name != NULL; p++) {
        if (p->value == (unsigned) codes) {
            printf("scancode %s (0x%02x)\n", p->name, codes);
            return;
        }
    }

    if (isprint(codes)) {
        printf("scancode '%c' (0x%02x)\n", codes, codes);
    } else {
        printf("scancode 0x%02x\n", codes);
    }
}
#define KEY_RELEASE 0
#define KEY_PRESS 1
#define KEY_KEEPING_PRESSED 2

bool evdev_read_keyboard(lv_indev_data_t * data)
{
    struct input_event in;
    while(read(evdev_kbd_fd, &in, sizeof(struct input_event)) > 0) {
        if(in.type == EV_KEY) {
                if ((in.value == KEY_PRESS) || (in.value == KEY_KEEPING_PRESSED)) {
                    //prtcode(in.code);
                    //printf("type %d code %d value %d %c %s [%s]\n", in.type, in.code, in.value, keynames[in.code].value,keynames[in.code].name,keys[in.code]);
                    //printf("\n");
		    data->state = LV_INDEV_STATE_PR;          /*Save the key is pressed now*/
            	//state = LV_INDEV_STATE_REL;         /*Save the key is released but keep the last key*/
    		data->key = keys[in.code][0];

                }

	}
    }
    return false;
}
bool keyboard_read(lv_indev_data_t * data) {
	return evdev_read_keyboard(data);
}
bool evdev_read_joystick(lv_indev_data_t * data) {
    static int16_t enc_diff = 0;
    struct js_event in;

    
    data->state = LV_INDEV_STATE_REL;

    //struct input_event in;
    //while(read(evdev_fd, &in, sizeof(struct input_event)) > 0) {
    while(read(evdev_js0_fd, &in, sizeof(struct js_event)) > 0) {
	    printf("in.time: %d in.value %d in.type%d in.number%d\n",in.time,in.value,in.type,in.number);
	    if (in.number==0) {
		    if (in.value==1)
			data->state = LV_INDEV_STATE_PR;          /*Save the key is pressed now*/
		    else
            		data->state = LV_INDEV_STATE_REL;         /*Save the key is released but keep the last key*/
	    } else if (in.number==7) {
		    if (in.value<0) 
			    enc_diff=-1;
		    else if (in.value>0) 
			    enc_diff=1;
	    }

    data->enc_diff = enc_diff;

    }
    return false;
}
bool evdev_read_mouse(lv_indev_data_t * data )
{
    struct input_event in;
    //while(read(evdev_fd, &in, sizeof(struct input_event)) > 0) {
    while(read(evdev_mouse0_fd, &in, sizeof(struct input_event)) > 0) {
        if(in.type == EV_REL) {
            if(in.code == REL_X)
				#if EVDEV_SWAP_AXES
					evdev_root_y += in.value;
				#else
					evdev_root_x += in.value;
				#endif
            else if(in.code == REL_Y)
				#if EVDEV_SWAP_AXES
					evdev_root_x += in.value;
				#else
					evdev_root_y += in.value;
				#endif
        } else if(in.type == EV_ABS) {
            if(in.code == ABS_X)
				#if EVDEV_SWAP_AXES
					evdev_root_y = in.value;
				#else
					evdev_root_x = in.value;
				#endif
            else if(in.code == ABS_Y)
				#if EVDEV_SWAP_AXES
					evdev_root_x = in.value;
				#else
					evdev_root_y = in.value;
				#endif
        } else if(in.type == EV_KEY) {
            if(in.code == BTN_MOUSE || in.code == BTN_TOUCH) {
                if(in.value == 0)
                    evdev_button = LV_INDEV_STATE_REL;
                else if(in.value == 1)
                    evdev_button = LV_INDEV_STATE_PR;
            }
        }
    }

    /*Store the collected data*/

#if EVDEV_SCALE
    data->point.x = map(evdev_root_x, 0, EVDEV_SCALE_HOR_RES, 0, LV_HOR_RES);
    data->point.y = map(evdev_root_y, 0, EVDEV_SCALE_VER_RES, 0, LV_VER_RES);
#else
#if EVDEV_CALIBRATE
	data->point.x = map(evdev_root_x, EVDEV_HOR_MIN, EVDEV_HOR_MAX, 0, LV_HOR_RES);
	data->point.y = map(evdev_root_y, EVDEV_VER_MIN, EVDEV_VER_MAX, 0, LV_VER_RES);
#else
    data->point.x = evdev_root_x;
    data->point.y = evdev_root_y;
#endif
#endif

    data->state = evdev_button;

    if(data->point.x < 0)
      data->point.x = 0;
    if(data->point.y < 0)
      data->point.y = 0;
    if(data->point.x >= LV_HOR_RES)
      data->point.x = LV_HOR_RES - 1;
    if(data->point.y >= LV_VER_RES)
      data->point.y = LV_VER_RES - 1;

    return false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
int map(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif
