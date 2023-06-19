
#ifndef CommonController_h
#define CommonController_h

typedef struct
{
  const char *name;
  uint8_t  value;
}stLookup;

static stLookup XInputControlLookup[] =
{
  {  "BUTTON_LOGO", BUTTON_LOGO  },
  {  "BUTTON_A", BUTTON_A  },
  {  "BUTTON_B", BUTTON_B },
  {  "BUTTON_X", BUTTON_X },
  {  "BUTTON_Y", BUTTON_Y },
  {  "BUTTON_LB", BUTTON_LB },
  {  "BUTTON_RB", BUTTON_RB },
  {  "BUTTON_BACK", BUTTON_BACK },
  {  "BUTTON_START", BUTTON_START },
  {  "BUTTON_L3", BUTTON_L3 },
  {  "BUTTON_R3", BUTTON_R3 },
  {  "DPAD_UP", DPAD_UP },
  {  "DPAD_DOWN", DPAD_DOWN },
  {  "DPAD_LEFT", DPAD_LEFT },
  {  "DPAD_RIGHT", DPAD_RIGHT },
  {  "TRIGGER_LEFT", TRIGGER_LEFT },
  {  "TRIGGER_RIGHT", TRIGGER_RIGHT },
  {  "JOY_LEFT", JOY_LEFT },
  {  "JOY_RIGHT", JOY_RIGHT },
  {   NULL , 0     }
};

static uint8_t xboxlookup( String name )
{
  stLookup *lptr;
  for ( lptr = XInputControlLookup; lptr->name != NULL; lptr++ )
      if ( strcmp( lptr->name, name.c_str() ) == 0 )
          break;
  return( lptr->value );
}

static stLookup KeyboardLookup[] =
{
{ "KEY_LEFT_CTRL", 0x80 },
{ "KEY_LEFT_SHIFT", 0x81 },
{ "KEY_LEFT_ALT",  0x82 },
{ "KEY_LEFT_GUI",  0x83 },
{ "KEY_RIGHT_CTRL", 0x84 },
{ "KEY_RIGHT_SHIFT", 0x85 },
{ "KEY_RIGHT_ALT", 0x86 },
{ "KEY_RIGHT_GUI", 0x87 },

{ "KEY_UP_ARROW",  0xDA },
{ "KEY_DOWN_ARROW", 0xD9 },
{ "KEY_LEFT_ARROW", 0xD8 },
{ "KEY_RIGHT_ARROW", 0xD7 },
{ "KEY_BACKSPACE", 0xB2 },
{ "KEY_TAB", 0xB3 },
{ "KEY_RETURN", 0xB0 },
{ "KEY_MENU",   0xED },
{ "KEY_ESC", 0xB1 },
{ "KEY_INSERT", 0xD1 },
{ "KEY_DELETE", 0xD4 },
{ "KEY_PAGE_UP", 0xD3 },
{ "KEY_PAGE_DOWN", 0xD6 },
{ "KEY_HOME",   0xD2 },
{ "KEY_END", 0xD5 },
{ "KEY_CAPS_LOCK", 0xC1 },
{ "KEY_PRINT_SCREEN", 0xCE },
{ "KEY_SCROLL_LOCK", 0xCF },
{ "KEY_PAUSE",  0xD0 },

{ "KEY_NUM_LOCK",  0xDB },
{ "KEY_KP_SLASH",  0xDC },
{ "KEY_KP_ASTERISK", 0xDD },
{ "KEY_KP_MINUS",  0xDE },
{ "KEY_KP_PLUS", 0xDF },
{ "KEY_KP_ENTER",  0xE0 },
{ "KEY_KP_1",   0xE1 },
{ "KEY_KP_2",   0xE2 },
{ "KEY_KP_3",   0xE3 },
{ "KEY_KP_4",   0xE4 },
{ "KEY_KP_5",   0xE5 },
{ "KEY_KP_6",   0xE6 },
{ "KEY_KP_7",   0xE7 },
{ "KEY_KP_8",   0xE8 },
{ "KEY_KP_9",   0xE9 },
{ "KEY_KP_0",   0xEA },
{ "KEY_KP_DOT", 0xEB },

{ "KEY_F1",  0xC2 },
{ "KEY_F2",  0xC3 },
{ "KEY_F3",  0xC4 },
{ "KEY_F4",  0xC5 },
{ "KEY_F5",  0xC6 },
{ "KEY_F6",  0xC7 },
{ "KEY_F7",  0xC8 },
{ "KEY_F8",  0xC9 },
{ "KEY_F9",  0xCA },
{ "KEY_F10", 0xCB },
{ "KEY_F11", 0xCC },
{ "KEY_F12", 0xCD },
{ "KEY_F13", 0xF0 },
{ "KEY_F14", 0xF1 },
{ "KEY_F15", 0xF2 },
{ "KEY_F16", 0xF3 },
{ "KEY_F17", 0xF4 },
{ "KEY_F18", 0xF5 },
{ "KEY_F19", 0xF6 },
{ "KEY_F20", 0xF7 },
{ "KEY_F21", 0xF8 },
{ "KEY_F22", 0xF9 },
{ "KEY_F23", 0xFA },
{ "KEY_F24", 0xFB },
{   NULL , 0x00   }
};

static uint8_t keyboardlookup( String name )
{
  stLookup *lptr;
  for ( lptr = KeyboardLookup; lptr->name != NULL; lptr++ )
      if ( strcmp( lptr->name, name.c_str() ) == 0 )
          break;
  return( lptr->value );
}

static stLookup MouseLookup[] =
{
#ifdef MOUSE_INTERFACE
{ "MOUSE_LEFT", MOUSE_LEFT },
{ "MOUSE_RIGHT", MOUSE_RIGHT },
{ "MOUSE_MIDDLE",  MOUSE_MIDDLE },
{ NULL, MOUSE_LEFT }
#endif
};

static uint8_t mouselookup( String name )
{
  stLookup *lptr;
  for ( lptr = MouseLookup; lptr->name != NULL; lptr++ )
      if ( strcmp( lptr->name, name.c_str() ) == 0 )
          break;
  return( lptr->value );
}

#endif