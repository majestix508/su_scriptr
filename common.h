#ifndef _COMMON_H
#define _COMMON_H 1
#include <time.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <iterator>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

using namespace std;

/* Command and response IDs */
#define OBC_SU_ON 		0xF1
#define OBC_SU_OFF 		0xF2
#define SU_RESET		0x02
#define SU_LDP			0x05
#define SU_HC			0x06
#define SU_CAL			0x07
#define SU_SCI			0x08
#define SU_HK			0x09
#define SU_STM			0x0A
#define SU_DUMP			0x0B
#define SU_BIAS_ON		0x53
#define SU_BIAS_OFF		0xC9
#define SU_MTEE_ON		0x35
#define SU_MTEE_OFF		0x9C

/* Only response IDs */
#define SU_ERR			0xBB
#define OBC_SU_ERR		0xFA
#define OBC_EOT			0xFE

/* Definition of Script_INDEX */
#define SU_S1_SCRIPT	0x41
#define SU_S2_SCRIPT	0x42
#define SU_S3_SCRIPT	0x43
#define SU_S4_SCRIPT	0x44
#define SU_S5_SCRIPT	0x45
#define SU_EOT			0x55

extern std::map<uint8_t,std::string> g_cmds;

typedef struct __attribute__ ((__packed__)) mnlp_script_header_s{
	uint16_t script_length;
 	uint32_t t_starttime;
 	uint32_t file_sn;
 	uint8_t sw_ver;
 	uint8_t script_type;
 }mnlp_script_header_t;

typedef struct  __attribute__ ((__packed__)) mnlp_times_table_s{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t script_index;
}mnlp_times_table_t;

typedef struct  __attribute__ ((__packed__)) mnlp_script_sequence_s{
	uint8_t deltaTIME_sec;
	uint8_t deltaTIME_min;
	uint8_t cmd_id;
	uint8_t length;
	uint8_t seq_cnt;
	uint8_t param[254];
}mnlp_script_sequence_t;

uint16_t SlowFletcher16( uint8_t* data, int count );
uint16_t fletcher16( uint8_t const *data, size_t bytes );
int AdjustFletcher2Zero(uint8_t *buffer, size_t n);
string getStrDate(time_t timestamp);
time_t getTimeByString(string timestr);
time_t get2Ktime();

vector<string> split(string str,string sep);
string getCMDStr(uint8_t a_code);
uint8_t getCMDCode(string &a_str);
void initCMDMap();

#endif
