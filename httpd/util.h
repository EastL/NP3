#ifndef __UTIL__
#define __UTIL__

void split(char ***arr, char *str, char *del, size_t *count);
int regular_match(char *str, char *reg_str);
void replace_html(char *str);
#endif
