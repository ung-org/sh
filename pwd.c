/*
 * UNG's Not GNU
 * 
 * Copyright (c) 2011, Jakob Kaivo <jakob@kaivo.net>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include "sh.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const char *pwd_desc = "return working directory name";
const char *pwd_inv  = "pwd [-L|-P]";

int
pwd_main (int argc, char **argv)
{
  //char mode = 'L';
  char *cwd;
  int c;

  while ((c = getopt (argc, argv, ":LP")) != -1) {
    switch (c) {
      case 'L':
      case 'P':
        ////mode = c;
        break;
      default:
        return 1;
    }
  }

  if (optind < argc)
    return 1;
 
  // FIXME:
  // if (mode == 'L')
  //   if (cwd !~ /./ && cwd !~ /../)
  //     print (cwd);
  //   else if (strlen (cwd) > PATH_MAX && NO_DOT_OR_DOT_DOT)
  //     unspecified
  //   else
  //     mode = 'P';
  // if (mode == 'P')
  //  decompose_to_shortest_path
  //  print (decomposed)
  cwd = (char*)getenv ("PWD");
  printf ("%s\n", cwd);

  return 0;
}
