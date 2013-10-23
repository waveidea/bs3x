#ifndef ASSIGN_TABLES
#define ASSIGN_TABLES

// Strings values for menus  {offset of first substr, offset of second str, ..., str1, str2, ...}

/////////////////////// MENUS BITMAPS //////////////////////////////////////

////////////////////////// all menus and sub menus indexes ///////////////////////////////
enum {
// TOP LEVEL MENUS
ASSIGN_STD_INDEX, ASSIGN_USER_INDEX,
// SUB LEVEL MENUS
ASSIGN_STD_EVENT_TYPE_INDEX, ASSIGN_STD_EVENT_NUM_INDEX,
ASSIGN_USR_EVENT_TYPE_INDEX
};

/////////////////////// THE WHOLE MENUS STRUCTURES DEFINITION //////////////////////////////////////
rom MENU menuassigntable[] = {
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //////////// TOP LEVEL MENUS
      /////////////////////////////////////////////////////////////////////////////////////////////////////
      { // index 0 [CALL TO A SUBMENU]
        "Assign Std mode",          // Menu associated string
        TOP_LEVEL_MENU,         // menu_depth           : the menu depth
        MODES_NB,      			// value_max            : 12 sub menus from this global menu
        UNDEFINED_VALUE,        // value_index          : No table index for a global menu
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : Dynamically filled
        midi_setup_bmp,         // GlobalMenuBmp        : Associated BMP ptr if global menu
        ASSIGN_STD_EVENT_TYPE_INDEX,     // SubMenuBaseIndex     : index of the first submenu for this menu
        TRUE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      },
      { // index 0 [CALL TO A SUBMENU]
        "Assign User mode",          // Menu associated string
        TOP_LEVEL_MENU,         // menu_depth           : the menu depth
        MODES_NB,      			// value_max            : 12 sub menus from this global menu
        UNDEFINED_VALUE,        // value_index          : No table index for a global menu
        SUB_MENU,               // table_id             : ID of the table to modify
        NO_BIT,                 // bit_pos_msk          : position of the bit to modify
        DISP_STRING,            // display_type         : The value of this menu is a string type (sub menus names)
        NULL,                   // MenuValueStr         : Dynamically filled
        midi_setup_bmp,         // GlobalMenuBmp        : Associated BMP ptr if global menu
        ASSIGN_USR_EVENT_TYPE_INDEX,     // SubMenuBaseIndex     : index of the first submenu for this menu
        FALSE                    // NextMenuExist        : TRUE if a next menu exist on same depth
      }

};

#endif
