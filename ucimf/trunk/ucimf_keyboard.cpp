#include <sys/ioctl.h>
#include <linux/kd.h>
#include <linux/keyboard.h>
#include <linux/input.h>
#include <iostream>
using namespace std;

enum
{
  CTRL_SPACE = 203,
  CTRL_SHIFT = 204,
  CTRL_F5 = 205,
  CTRL_F6 = 206,
};

typedef struct _new_key
{
  int table;
  int key;
  int new_key;
}
new_key;

static new_key nkeys[] = {
  /* [table]        [key]         [new_key] */
  {(1 << KG_CTRL), KEY_SPACE, K(KT_LATIN, CTRL_SPACE)},	/* Ctrl + Space */
  {(1 << KG_CTRL), KEY_LEFTSHIFT, K(KT_LATIN, CTRL_SHIFT)},	/* Ctrl + Shift */
  {(1 << KG_CTRL), KEY_F5, K(KT_LATIN, CTRL_F5)},	/* Ctrl + F5 */
  {(1 << KG_CTRL), KEY_F6, K(KT_LATIN, CTRL_F6)},	/* Ctrl + F6 */
};

int setup_keys()
{
  int i;
  for (i = 0; i < sizeof (nkeys) / sizeof (new_key); i++)
  {
    struct kbentry entry;
    struct kbentry tmp;
    entry.kb_table = nkeys[i].table;
    entry.kb_index = nkeys[i].key;

        /* save current value */
    int ret;
    cerr << "Get KeyBoard Event..." << endl;
    ret = ioctl (0, KDGKBENT, &entry);
    tmp.kb_value = entry.kb_value;

    entry.kb_value = nkeys[i].new_key;
    cerr << "Set KeyBoard Event..." << endl;
    ret=ioctl (0, KDSKBENT, &entry);
    nkeys[i].new_key = tmp.kb_value;
  }
  return 0;
}

int main()
{
  setup_keys();
  return 0;
}
