
#ifndef _USER_CLI_H_
#define _USER_CLI_H_

//typedef int (*pfUserCli_cb_t)(char op, unsigned char* pData, char Len);

typedef int (*pfUserCli_cb_t)(const char *const pCmd, char **pParam, const char paramNum);


int Cli_RegisterUserCmd(pfUserCli_cb_t pfUserCli_cb_func);

#endif
