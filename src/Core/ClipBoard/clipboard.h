#ifndef swag_clipboard_h
#define swag_clipboard_h

#ifdef __cplusplus
extern "C" {
#endif

#include <allegro5/allegro.h>

const char * Get_clipboard_errmsg(void);
int Set_clipboard_text(const char * const text, const int size);
const char * Get_clipboard_text(void);
int Set_clipboard_bitmap(ALLEGRO_BITMAP* bmp, const char* image_extension);
ALLEGRO_BITMAP* Get_clipboard_bitmap(const char* image_extension);

#ifdef __cplusplus
}
#endif

#endif

