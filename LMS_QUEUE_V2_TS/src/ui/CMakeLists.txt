SET(SOURCES screens/ui_Queue_hueue.c
    screens/ui_addToQueueScreen.c
    screens/ui_adminScreen.c
    screens/ui_addMemberScreen.c
    screens/ui_queueManagmentScreen.c
    screens/ui_manageMyPrintsScreen.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_31439854.c
    ui_temporary_image.c)

add_library(ui ${SOURCES})
