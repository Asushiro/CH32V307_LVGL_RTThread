#include "hal_config.h"
#include "lvgl.h"

/* LVGL线程 */
struct rt_thread lvgl_thread;       //LVGL线程结构体
__attribute__((aligned(4))) uint8_t lvgl_thread_stack[2560];    //LVGL线程的栈，如果空间不够使用可以适当放大，可以通过shell命令查看线程占用情况

/* LCD刷新互斥量 */
struct rt_mutex lcd_mutex;          //LCD刷新互斥量结构体
lv_obj_t *label = NULL;

/* 屏幕旋转 */
void screen_rotate(void) {
    lv_disp_t * disp;

    rt_mutex_take(&lcd_mutex, RT_WAITING_FOREVER); //获取互斥量，获取后LVGL线程将不允许执行，也就是屏幕会停止刷新
    
    if(lcddev.width == 320) {
        Set_Dir(D2U_R2L);
        LCD_WR_REG(0x36); // Memory Access Control
        LCD_WR_DATA(0x08|D2U_R2L);
    } else {
        Set_Dir(R2L_U2D);
        LCD_WR_REG(0x36); // Memory Access Control
        LCD_WR_DATA(0x08|R2L_U2D);
    }

    disp = lv_disp_get_default();
    disp->driver->hor_res = lcddev.width;
    disp->driver->ver_res = lcddev.height;
    lv_disp_drv_update(disp, disp->driver);

    rt_mutex_release(&lcd_mutex); //释放互斥量，释放后LVGL线程将允许执行，屏幕将被刷新
    hal_delay_ms(50);
}

void btn_event_cb(lv_event_t * e) {
    lv_label_set_text(label, "Clicked");
}

/* LVGL显示线程 */
void lvgl_thread_entry(void *parameter) {
    LCD_Init();             //LCD初始化
    GT911_init();           //触摸模块初始化
    lv_init();              //LVGL库初始化
    lv_port_disp_init();    //LVGL显示初始化
    lv_port_indev_init();   //LVGL输入初始化
    screen_rotate();

    lv_obj_t *scr = lv_scr_act();   //整个屏幕容器

    /* 深蓝背景，覆盖主题的白色 */
    lv_obj_set_style_bg_color(scr, lv_palette_main(LV_PALETTE_CYAN), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    /* 白色文字 */
    label = lv_label_create(scr);
    lv_label_set_text(label, "Hello LVGL");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, -40);

    /* 按钮 */
    lv_obj_t *btn = lv_btn_create(scr);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 40);
    lv_obj_add_event_cb(btn,btn_event_cb,LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Click");
    lv_obj_center(btn_label);


    while(1) {
        rt_mutex_take(&lcd_mutex, RT_WAITING_FOREVER);  //获取互斥量
        lv_timer_handler();                             //LVGL事件处理函数
        rt_mutex_release(&lcd_mutex);                   //释放互斥量
        hal_delay_ms(10);
    }
}

int main(void) {
    rt_err_t status;

    hal_printf("\r\n MCU: CH32V307\r\n");
    SystemCoreClockUpdate();
    hal_printf(" SysClk: %dHz\r\n",SystemCoreClock);
    hal_printf(" ChipID: %08x\r\n",DBGMCU_GetCHIPID());

    /* 初始化LVGL线程 */
    rt_mutex_init(&lcd_mutex, "lcd", RT_IPC_FLAG_PRIO);
    status = rt_thread_init( &lvgl_thread,                  //线程结构体地址?
                             "tlvgl",                       //线程名字
                              lvgl_thread_entry,            //线程入口地址
                              RT_NULL,                      //线程入口函数的参数为空
                              lvgl_thread_stack,            //线程栈的地址
                              sizeof(lvgl_thread_stack),    //线程栈的大小
                              5,
                              1000);
    if(status == (-RT_ERROR)) {
        hal_printf(" lvgl thread create fail\r\n");
    } else {
        hal_printf(" lvgl thread create success\r\n");
        rt_thread_startup(&lvgl_thread);                    //启动线程
    }
    return 0;
}
