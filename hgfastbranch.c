/*
 Copyright 2011 Jonathan D. Page, 2017 Kyle Vedder

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

#define MALLOC_CHUNK 10
#define SAFE_CHAR_MALLOC(ptr, size) if (((ptr) = (char *)malloc((size) * sizeof(char))) == NULL) panic();
#define SAFE_CHAR_REALLOC(ptr, size) if (((ptr) = (char *)realloc((ptr), (size) * sizeof(char))) == NULL) panic();

void panic() {
  exit(EXIT_FAILURE);
}

bool is_hg_dir(const char *pwd) {
  char *hg_path;
  size_t plen;
  int statret;
  struct stat st;

  plen = strlen(pwd);
  SAFE_CHAR_MALLOC(hg_path, plen + 5);
  sprintf(hg_path, "%s/.hg", pwd);
  statret = stat(hg_path, &st);
  free(hg_path);

  return statret == 0;
}

char* get_branch(const char *repo) {
  FILE *f;
  char *branchfile, *branchname;
  size_t plen, read, cchar, csize;
  int k;

  plen = strlen(repo);
  SAFE_CHAR_MALLOC(branchfile, plen + 12);
  sprintf(branchfile, "%s/.hg/branch", repo);

  if ((f = fopen(branchfile, "r")) == NULL)
    return "default";

  free(branchfile);

  csize = MALLOC_CHUNK;
  SAFE_CHAR_MALLOC(branchname, csize);

  cchar = 0;
  while (true) {
    read = fread(branchname + cchar, sizeof(char), MALLOC_CHUNK, f);
    cchar += read;

    if (read < MALLOC_CHUNK) {
      break;  // Don't care about the difference between eof and err
    }

    csize += MALLOC_CHUNK;
    SAFE_CHAR_REALLOC(branchname, csize);
  }

  branchname[cchar] = 0;
  for (k = 0; k < cchar; k++) {
    if (branchname[k] == '\n') {
      branchname[k] = 0;
      break;
    }
  }
  SAFE_CHAR_REALLOC(branchname, k + 1);
  fclose(f);
  return branchname;
}

char* get_repo() {
  char *dir_path;
  int len, k;
  dir_path = getcwd(NULL, 0);

  while ((len = strlen(dir_path)) > 0) {
    if (is_hg_dir(dir_path)) {
      SAFE_CHAR_REALLOC(dir_path, strlen(dir_path) + 1);
      return dir_path;
    }

    for (k = len - 1; k >= 0; k--) {
      if (dir_path[k] == '/') {
        dir_path[k] = 0;
        break;
      }
    }
  }

  free(dir_path);
  return NULL;
}

int main(int argc, char** argv) {
  char *repo;
  char *format;

  repo = get_repo();
  if (repo == NULL) {
    return 0;
  }

  if (argc > 1) {
    format = " %s \\[\e[33m\\]%s\\[\e[0m\\]";
    printf(format, argv[1], get_branch(repo));
  } else {
    format = " on \\[\e[33m\\]%s\\[\e[0m\\]";
    printf(format, get_branch(repo));
  }

  return 0;
}
