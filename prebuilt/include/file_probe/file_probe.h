#ifndef __FILE_PROBE_H__
#define __FILE_PROBE_H__

void *file_probe_create(int source_type);
void file_probe_destroy(void *hd);
int file_probe(void *hd, const char *name);

#endif
