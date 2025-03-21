#include "ui.h"
#include "ui_helpers.h"
#include "main_common.h"
#include "stdio.h"

///////////////////// VARIABLES ////////////////////
void appear_Animation(lv_obj_t *TargetObject, int delay);
void screenLoad_Animation(lv_obj_t *TargetObject, int delay);

// SCREEN: ui_welcomescreen
void ui_welcomescreen_screen_init(void);
lv_obj_t *ui_welcomescreen;
void ui_event_devicename(lv_event_t *e);
lv_obj_t *ui_devicename;
void ui_event_continueContainer(lv_event_t *e);
void start_continue_Animation(lv_obj_t *TargetObject, int delay);
lv_obj_t *ui_continueContainer;
lv_obj_t *ui_continueLabel;
// CUSTOM VARIABLES
lv_obj_t *uic_continueContainer;

// SCREEN: ui_mainscreen
void ui_mainscreen_screen_init(void);
void ui_event_mainscreen(lv_event_t *e);
lv_obj_t *ui_mainscreen;
lv_obj_t *wifi_icon;
lv_obj_t *sdcard_icon;
lv_obj_t *adc_icon;
lv_obj_t *temp_sensor_icon;
lv_obj_t *ui_TempContainer;
lv_obj_t *ui_tempLabel;
lv_obj_t *ui_tempUnit;
lv_obj_t *ui_tempValue;
lv_obj_t *ui_HumContainer;
lv_obj_t *ui_humiLabel;
lv_obj_t *ui_humiUnit;
lv_obj_t *ui_humiValue;
lv_obj_t *ui_timelabel;
void ui_event_measureBTN(lv_event_t *e);
lv_obj_t *ui_measureBTN;
lv_obj_t *ui_measureLabel;
lv_obj_t *ui_resultContainer;
lv_obj_t *ui_resultValue;
lv_obj_t *ui_resultLabel;
lv_obj_t *ui_error_notify;
lv_obj_t *ui_titleLabel1;
lv_obj_t *ui_titleLabel2;
void ui_event_mainscreenMenu(lv_event_t *e);
lv_obj_t *ui_mainscreenMenu;
lv_obj_t *ui_measureState;
lv_obj_t *ui_loadingSpinner;

// SCREEN: ui_aboutscreen
void ui_aboutscreen_screen_init(void);
lv_obj_t *ui_aboutscreen;
void ui_event_aboutscreenMenu(lv_event_t *e);
lv_obj_t *ui_aboutscreenMenu;
lv_obj_t *ui_abouttitleLabel;
lv_obj_t *ui_about_deviceName;
lv_obj_t *ui_about_deviceID;
lv_obj_t *ui_about_Developer;
lv_obj_t *ui_dev_value;
lv_obj_t *ui_device_name_value;
lv_obj_t *ui_device_ID_value;
// CUSTOM VARIABLES
lv_obj_t *uic_about_deviceName;
lv_obj_t *uic_Devname_value;
lv_obj_t *uic_device_ID_value;

// SCREEN: ui_settingscreen
void ui_settingscreen_screen_init(void);
lv_obj_t *ui_settingscreen;
void ui_event_settingscreenMenu(lv_event_t *e);
void ui_event_wifi_setting_onoff(lv_event_t *e);
void ui_event_heating_setting_onoff(lv_event_t *e);
void ui_wifi_setting_label_state_change(uint8_t state, char *str);
lv_obj_t *ui_settingscreenMenu;
lv_obj_t *ui_settingtitleLabel;
lv_obj_t *ui_wifi_setting_label;
lv_obj_t *ui_wifi_setting_onoff;
lv_obj_t *ui_setting_wifi_state_panel;
lv_obj_t *ui_setting_wifi_state_label;
lv_obj_t *ui_heating_setting_label;
lv_obj_t *ui_heating_setting_onoff;
lv_obj_t *ui_setting_heating_state_panel;
lv_obj_t *ui_setting_heating_state_label;

// EVENTS
lv_obj_t *ui____initial_actions0;

///////////////////// ANIMATIONS ////////////////////
void appear_Animation(lv_obj_t *TargetObject, int delay)
{
    ui_anim_user_data_t *PropertyAnimation_0_user_data = lv_mem_alloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 2000);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_ease_in);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, true);
    // lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_opacity);

    lv_anim_start(&PropertyAnimation_0);
}

void update_continue_label()
{
    _ui_flag_modify(ui_continueContainer, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_ADD);
}

void start_continue_Animation(lv_obj_t *TargetObject, int delay)
{
    ui_anim_user_data_t *PropertyAnimation_0_user_data = lv_mem_alloc(sizeof(ui_anim_user_data_t));
    PropertyAnimation_0_user_data->target = TargetObject;
    PropertyAnimation_0_user_data->val = -1;
    lv_obj_set_style_text_opa(ui_continueLabel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_anim_t PropertyAnimation_0;
    lv_anim_init(&PropertyAnimation_0);
    lv_anim_set_time(&PropertyAnimation_0, 1000);
    lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
    lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_opacity);
    lv_anim_set_values(&PropertyAnimation_0, 0, 255);
    lv_anim_set_path_cb(&PropertyAnimation_0, lv_anim_path_ease_in);
    lv_anim_set_delay(&PropertyAnimation_0, delay + 0);
    lv_anim_set_deleted_cb(&PropertyAnimation_0, _ui_anim_callback_free_user_data);
    lv_anim_set_playback_time(&PropertyAnimation_0, 0);
    lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_count(&PropertyAnimation_0, 0);
    lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
    lv_anim_set_early_apply(&PropertyAnimation_0, true);
    lv_anim_set_ready_cb(&PropertyAnimation_0, update_continue_label);

    lv_anim_start(&PropertyAnimation_0);
}

///////////////////// FUNCTIONS ////////////////////
void ui_event_devicename(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_SCREEN_LOAD_START)
    {
        appear_Animation(ui_devicename, 1500);
    }
}

void ui_event_continueContainer(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_CLICKED)
    {
        _ui_screen_change(&ui_mainscreen, LV_SCR_LOAD_ANIM_FADE_ON, 1500, 0, &ui_mainscreen_screen_init);
    }
}

void ui_event_mainscreen(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_SCREEN_LOADED)
    {
        lv_obj_clean(ui_welcomescreen);
        lv_obj_remove_event_cb(ui_mainscreen, ui_event_mainscreen);
    }
}

void ui_event_measureBTN(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_CLICKED)
    {
        _ui_flag_modify(ui_measureBTN, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_REMOVE);
        _ui_opacity_set(ui_measureBTN, 130);
        _ui_flag_modify(ui_loadingSpinner, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify(ui_measureState, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
        _ui_flag_modify(ui_heating_setting_onoff, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_REMOVE);
        lv_obj_set_style_bg_opa(ui_heating_setting_onoff, 130, LV_PART_INDICATOR | LV_STATE_CHECKED);
    }
}

void ui_reset_before_measure_state(lv_event_t *e)
{
    _ui_flag_modify(ui_measureBTN, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_ADD);
    _ui_opacity_set(ui_measureBTN, 255);
    _ui_flag_modify(ui_loadingSpinner, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_measureState, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_ADD);
    _ui_flag_modify(ui_heating_setting_onoff, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_ADD);
    lv_obj_set_style_bg_opa(ui_heating_setting_onoff, 255, LV_PART_INDICATOR | LV_STATE_CHECKED);
}

void ui_event_mainscreenMenu(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        char *buf;
        buf = (char *)malloc(20 * sizeof(char));
        lv_dropdown_get_selected_str(ui_mainscreenMenu, buf, 20 * sizeof(char));
        if (strcmp(buf, "About") == 0)
        {
            _ui_screen_change(&ui_aboutscreen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_aboutscreen_screen_init);
        }
        else if (strcmp(buf, "Setting") == 0)
        {
            _ui_screen_change(&ui_settingscreen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_settingscreen_screen_init);
        }
        free(buf);
    }
}

void ui_event_aboutscreenMenu(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        char *buf;
        buf = (char *)malloc(20 * sizeof(char));
        lv_dropdown_get_selected_str(ui_aboutscreenMenu, buf, 20 * sizeof(char));
        if (strcmp(buf, "Main") == 0)
        {
            _ui_screen_change(&ui_mainscreen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_mainscreen_screen_init);
        }
        else if (strcmp(buf, "Setting") == 0)
        {
            _ui_screen_change(&ui_settingscreen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_settingscreen_screen_init);
        }
        free(buf);
    }
}

void ui_event_settingscreenMenu(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        char *buf;
        buf = (char *)malloc(20 * sizeof(char));
        lv_dropdown_get_selected_str(ui_settingscreenMenu, buf, 20 * sizeof(char));
        if (strcmp(buf, "Main") == 0)
        {
            _ui_screen_change(&ui_mainscreen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_mainscreen_screen_init);
        }
        else if (strcmp(buf, "About") == 0)
        {
            _ui_screen_change(&ui_aboutscreen, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, &ui_aboutscreen_screen_init);
        }
        free(buf);
    }
}

void ui_event_wifi_setting_onoff(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        if (lv_obj_has_state(ui_wifi_setting_onoff, LV_STATE_CHECKED))
        {
            xEventGroupSetBits(wifi_control_eventGroup, WIFI_ON_BIT);
        }
        else
        {
            xEventGroupSetBits(wifi_control_eventGroup, WIFI_OFF_BIT);
        }
    }
}

void ui_wifi_setting_label_state_change(uint8_t state, char *str)
{

    switch (state)
    {
    case WIFI_START:
    case WIFI_SCAN_DONE:
    case WIFI_GOT_SSID_PWD:
    case WIFI_RECONNECT:
    {
        _ui_label_set_property(ui_setting_wifi_state_label, _UI_LABEL_PROPERTY_TEXT, str);
        _ui_obj_set_style_text_color(ui_setting_wifi_state_label, 0x000000);
        break;
    }
    case WIFI_FOUND_CHANNEL:
    {
        _ui_label_set_property(ui_setting_wifi_state_label, _UI_LABEL_PROPERTY_TEXT, str);
        _ui_obj_set_style_text_color(ui_setting_wifi_state_label, 0x000000);
        break;
    }
    case WIFI_GOT_IP:
    {
        _ui_label_set_property(ui_setting_wifi_state_label, _UI_LABEL_PROPERTY_TEXT, str);
        _ui_obj_set_style_text_color(ui_setting_wifi_state_label, 0x000000);
        break;
    }
    case WIFI_NOT_CONNECTED:
    {
        _ui_label_set_property(ui_setting_wifi_state_label, _UI_LABEL_PROPERTY_TEXT, str);
        _ui_obj_set_style_text_color(ui_setting_wifi_state_label, 0xd92626);
        break;
    }

    default:
        break;
    }
}

void ui_event_heating_setting_onoff(lv_event_t *e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if (event_code == LV_EVENT_VALUE_CHANGED)
    {
        if (lv_obj_has_state(ui_heating_setting_onoff, LV_STATE_CHECKED))
        {
            _ui_label_set_property(ui_setting_heating_state_label, _UI_LABEL_PROPERTY_TEXT, "Turned on");
            _ui_obj_set_style_text_color(ui_setting_heating_state_label, 0x000000);
            _ui_flag_modify(ui_measureBTN, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_ADD);
            _ui_opacity_set(ui_measureBTN, 255);
        }
        else
        {
            _ui_label_set_property(ui_setting_heating_state_label, _UI_LABEL_PROPERTY_TEXT, "Turned off");
            _ui_obj_set_style_text_color(ui_setting_heating_state_label, 0xd92626);
            _ui_flag_modify(ui_measureBTN, LV_OBJ_FLAG_CLICKABLE, _UI_MODIFY_FLAG_REMOVE);
            _ui_opacity_set(ui_measureBTN, 130);
        }
    }
}

/**
 * @brief Update the state of a device icon on the main screen.
 *
 * If the state is true, the icon will be set to green. Otherwise, the icon will be set to red.
 *
 * @param TargetObject The LVGL object to update.
 * @param state The state of the device.
 */
void ui_update_device_icon_state(lv_obj_t *TargetObject, bool state)
{
    if (state == true)
    {
        _ui_obj_set_style_text_color(TargetObject, 0x04A904);
    }
    else
    {
        _ui_obj_set_style_text_color(TargetObject, 0xd92626);
    }
}

///////////////////// TIMER HANDLE ////////////////////
void lvgl_timer_handle_task(void *param)
{
    while (1)
    {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10)); // Delay 1 giây
    }
}

///////////////////// SCREENS ////////////////////

/**
 * @brief Initializes the UI.
 *
 * This function sets up the display, initializes all the UI screens, and sets the main screen to be the active screen.
 */
void ui_init(void)
{
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                              false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_welcomescreen_screen_init();
    ui_mainscreen_screen_init();
    ui_aboutscreen_screen_init();
    ui_settingscreen_screen_init();
    ui____initial_actions0 = lv_obj_create(NULL);
    lv_scr_load_anim(ui_mainscreen, LV_SCR_LOAD_ANIM_FADE_IN, 1000, 0, false);
}
