#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include "lock2key.h"

char *lock_to_key(char *lock)
{
  int len = strlen(lock);
  char *key = (char *)calloc(1, len + 1);
  int len2 = 0;
  register int i;

  for(i = 1; i < len; i++)
    key[i] = lock[i] ^ lock[i-1];
  key[0] = lock[0] ^ lock[len-1] ^ lock[len-2] ^ 5;
  for(i = 0; i < len; i++)
  {
    key[i] = ((key[i]<<4) & 0xF0) | ((key[i]>>4) & 0x0F);
    switch(key[i])
    {
      case 0:
      case 5:
      case 36:
      case 96:
      case 124:
      case 126:
        len2+=10;
			break;
      default:
        len2++;
    }
  }

  char *newkey = (char *)calloc(1, len2 + 1);//Junior: correct allocation of memory
  char *newkey_p = newkey;
  for(i = 0; i < len; i++)
  {
    switch(key[i])
    {
      case 0:
      case 5:
      case 36:
      case 96:
      case 124:
      case 126:
        sprintf(newkey_p, "/%%DCN%03d%%/", key[i]);
        newkey_p += 10;
      break;

      default:
        *newkey_p = key[i];
        newkey_p++;
    }
  }
  *newkey_p = '\0';
  free(key); //Junior: free memory
  return newkey;
}
