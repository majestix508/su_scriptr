#ifndef _CSCRIPT_H
#define _CSCRIPT_H 1
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "common.h"

using namespace std;

class CScript {
public:
    CScript();

    ~CScript();

    void ParseSUFile(char *filename);
    void ParseSU(char *buffer, long size);

    void ParseJSONFile(char *filename);
    void ParseJSON(char *jsonstring);
	void Print();

    bool CheckChecksum();
    bool HasChecksum();
    void CreateChecksum();
    void PrintChecksum();
    
    void WriteToFile(char* filename);
    string PrintJSON();

	mnlp_script_header_t m_header;
	
	vector<mnlp_times_table_t> m_timetable;
	
	vector<mnlp_script_sequence_t> m_seq;

	char *m_buffer;
	long m_fsize;

	uint8_t m_c0;
	uint8_t m_c1;
	bool m_hasChecksum;
};

#endif

