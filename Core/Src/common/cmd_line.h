#ifndef __CMD_LINE_H
#define __CMD_LINE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define CMD_LINE_BUFFERS_SIZE	32

enum {
	CMD_SUCCESS = 0,
	CMD_PARSER_FAILED,
	CMD_TBL_NOT_FOUND,
	CMD_NOT_FOUND,
	CMD_TOO_LONG,
};

typedef int8_t (*onCMDLine)(char* argv);

typedef struct {
    const char *cmd;
	onCMDLine onArguments;
	const char *desc;
} Args_t;

class CMDLine {
public:
	CMDLine();

	void InitCMDLine(Args_t *tblCMDLine);
	void addData(char c);
	char removeData();
	uint8_t parseLocal(char delimiter);
	uint8_t getParserCounts();

private:
	Args_t *mTblCMDLine = NULL;
	char mBuffers[CMD_LINE_BUFFERS_SIZE];
	uint8_t mInsertPosision;
	uint8_t mParserCounters;
};

#ifdef __cplusplus
}
#endif

#endif /* __CMD_LINE_H */
