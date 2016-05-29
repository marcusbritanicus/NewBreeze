#include "clipboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#ifdef FILENAME_SIZE
#error FILENAME_SIZE is already defined... :(
#endif

// 20 for filename, 1 for terminating character.
#define FILENAME_SIZE 21

#ifdef COMMAND_SIZE
#error COMMAND_SIZE is already defined... :(
#endif

// 20 for filename, 25 for command line, 1 for terminating character.
#define COMMAND_SIZE 46

#ifdef CLIPBOARD_ERRMSG_SIZE_
#error CLIPBOARD_ERRMSG_SIZE_ is already defined... :(
#endif

#define CLIPBOARD_ERRMSG_SIZE_ 512

char clipboard_errmsg[CLIPBOARD_ERRMSG_SIZE_];

const int CLIPBOARD_ERRMSG_SIZE = CLIPBOARD_ERRMSG_SIZE_;

const char * Get_clipboard_errmsg(void)
{
    return clipboard_errmsg;
}

void Set_clipboard_errmsg(const char * const msg)
{
    strcpy(clipboard_errmsg, msg);
}

void Set_clipboard_errmsgn(const char * const msg, const int size)
{
    strncpy(clipboard_errmsg, msg, size);
}

int Set_clipboard_text(const char * const text, const int size)
{
    int exitcode;
    int len;
    int status = 0;
    char * buffer = (char *)malloc(sizeof(char) * size);

    if(!buffer)
    {
        Set_clipboard_errmsg("Out of memory");
        goto cleanup;
    }

    // Copy maximum of size bytes to buffer of known size.
    memcpy(buffer, text, size);

    // Ensure buffer is terminated.
    buffer[size] = '\0';

    len = strlen(buffer);

    FILE * xclip = popen("/usr/bin/xclip -selection c", "w");

    if(!xclip)
    {
        Set_clipboard_errmsg("Failed to open a pipe to xclip");
        goto cleanup;
    }

    if(fwrite(buffer, sizeof(char), len, xclip) != len)
        goto cleanup;

    fflush(xclip);

    exitcode = pclose(xclip);

    if(exitcode != 0)
    {
        Set_clipboard_errmsg("xclip exited with nonzero exit status.");
    }
    
    status = exitcode == 0;

cleanup:
    free(buffer);

    return status;
}

const char * Get_clipboard_text()
{
    char * text = 0;
    int text_size = BUFSIZ;

    text = (char *)malloc(sizeof(char) * text_size);

    if(!text)
    {
        Set_clipboard_errmsg("Out of memory");
        goto cleanup;
    }

    text[0] = '\0';

    char buffer[BUFSIZ];
    FILE * xclip = popen("/usr/bin/xclip -o -selection c", "r");
    int len;
    int total_len = 0;

    if(!xclip)
    {
        Set_clipboard_errmsg("Failed to open a pipe to xclip");
        goto fatal;
    }

    do
    {
        len = fread(buffer, sizeof(char), BUFSIZ - 1, xclip);

        if(len)
        {
            if(total_len > 0)
            {
                text_size += len + 1;
                text = (char *)realloc(text, sizeof(char) * text_size);

                if(!text)
                {
                    Set_clipboard_errmsg("Out of memory");
                    goto cleanup;
                }
            }

            memcpy(text + total_len, buffer, len);
            total_len += len;
            *(text + total_len) = '\0';
        }
        else if(ferror(xclip))
        {
            Set_clipboard_errmsg("Failed to read from xclip child.");
            goto fatal;
        }
    }while(len == BUFSIZ - 1);

cleanup:
    if(xclip)
    {
        int exitcode = pclose(xclip);

        xclip = 0;

        if(exitcode != 0)
        {
            Set_clipboard_errmsg("xclip returned nonzero exit status.");
            goto fatal;
        }
    }

    return text;

fatal:
    free(text);
    text = 0;
    goto cleanup;
}

int Set_clipboard_bitmap(ALLEGRO_BITMAP* bmp , const char* image_extension) {
   return 0;
}

ALLEGRO_BITMAP* Get_clipboard_bitmap(const char* image_extension) {
   return 0;
}
