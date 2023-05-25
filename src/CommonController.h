
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
#endif