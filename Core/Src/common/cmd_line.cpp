#include "cmd_line.h"

CMDLine::CMDLine() {

}

void CMDLine::InitCMDLine(Args_t *tblCMDLine) {
    if (tblCMDLine != NULL) {
		mTblCMDLine = tblCMDLine;
	}
	memset(mBuffers, 0, CMD_LINE_BUFFERS_SIZE);
	mInsertPosision = mParserCounters = 0;
}

void CMDLine::addData(char c) {
    if (mInsertPosision < CMD_LINE_BUFFERS_SIZE) {
		mBuffers[mInsertPosision++] = c;
	}
}

char CMDLine::removeData() {
    char c;

    if (mInsertPosision != 0) {
        c = mBuffers[mInsertPosision];
		--(mInsertPosision);
	}

    return c;
}

uint8_t CMDLine::parseLocal(char delimiter) {
    uint8_t ret = CMD_SUCCESS;
	char *argCMD = NULL;
	uint8_t ind = 0;

	if (mTblCMDLine == NULL) {
		ret = CMD_TBL_NOT_FOUND;
		goto END_OF_PARSER_PROCESS;
	}

	if (mInsertPosision == 0) {
		ret = CMD_PARSER_FAILED;
		goto END_OF_PARSER_PROCESS;
	}

	for (uint8_t id = 0; id < mInsertPosision; ++id) {
		if (mBuffers[id] == delimiter || mBuffers[id] == '\r' || mBuffers[id] == '\n') {
			mBuffers[id] = 0;
			++mParserCounters;
		}
	}

	argCMD = &mBuffers[0];
	while (mTblCMDLine[ind].cmd) {
		if (strcmp((const char*)mTblCMDLine[ind].cmd, (const char*)argCMD) == 0) {
			mTblCMDLine[ind].onArguments(argCMD);
			break;
		}
		++ind;
	}

END_OF_PARSER_PROCESS:
	memset(mBuffers, 0, CMD_LINE_BUFFERS_SIZE);
	mInsertPosision = mParserCounters = 0;

	return ret;
}

uint8_t CMDLine::getParserCounts() {
    return mParserCounters;
}
