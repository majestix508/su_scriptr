#include "common.h"

uint16_t SlowFletcher16( uint8_t* data, int count ) {
	uint16_t sum1 = 0;
	uint16_t sum2 = 0;
	int index;
	for( index = 0; index < count; ++index ) {
		sum1 = (sum1 + data[index]) % 255;
		sum2 = (sum2 + sum1) % 255;
	}
	return (sum2 << 8) | sum1;
}

/*
Two bytes c0, c1 are added to the script making the Fletcher
checksum of the script equal zero. They are calculated with the
statements.

uint16_t csum;
uint8_t c0,c1,f0,f1;
csum = Fletcher16( data, length);
f0 = csum & 0xff;
f1 = (csum >> 8) & 0xff;
c0 = 0xff - (( f0 + f1) % 0xff);
c1 = 0xff - (( f0 + c0 ) % 0xff);

Where length is the length in bytes of the data input (script)
before the c0 and c1 are added.
*/

uint16_t fletcher16( uint8_t const *data, size_t bytes )
{
        uint16_t sum1 = 0xff, sum2 = 0xff;
 
        while (bytes) {
                size_t tlen = bytes > 20 ? 20 : bytes;
                bytes -= tlen;
                do {
                        sum2 += sum1 += *data++;
                } while (--tlen);
                sum1 = (sum1 & 0xff) + (sum1 >> 8);
                sum2 = (sum2 & 0xff) + (sum2 >> 8);
        }
        /* Second reduction step to reduce sums to 8 bits */
        sum1 = (sum1 & 0xff) + (sum1 >> 8);
        sum2 = (sum2 & 0xff) + (sum2 >> 8);
        return sum2 << 8 | sum1;
}

// int AdjustFletcher2Zero()
//
// Add two bytes to give the fletcher checksum a return value of zero
//         
// The function returns with the final Fletcher16 checksum, (it ought to be 0).

int AdjustFletcher2Zero(uint8_t *buffer, size_t n)
{
    uint16_t u16;
    uint16_t u17;
    unsigned char c0;
    unsigned char c1;
    unsigned char cb0;
    unsigned char cb1;
    int retval;

    retval = 9999;

    u16 = SlowFletcher16(buffer, n);

    // calculate the two bytes to be added 
    
    c0 =  (u16 & 0x00FF);
    c1 = ((u16 & 0xFF00) >> 8);
    
    cb0 = 255 - ((c0+c1) % 255);
    cb1 = 255 - ((c0 + cb0) % 255);

    // add the two bytes
     
    buffer[n] = cb0;    n++;
    buffer[n] = cb1;    n++;

    u17 = SlowFletcher16(buffer, n);

    retval = (int)(u17);

    return(retval); 
}

time_t get2Ktime(){
	time_t now = time(NULL);
	struct tm *tx = gmtime(&now);

	struct tm start;

	/* 01.01.2000, 00:00:00 */
	start.tm_hour = 0;
	start.tm_min = 0;
	start.tm_sec = 0;
	start.tm_mday = 1; /* Day of month (1 - 31) */
	start.tm_mon = 0; /* Months since January (0 -11)*/
	start.tm_year = 100; /* Years since 1900 */
	start.tm_isdst = tx->tm_isdst;

	return mktime(&start);
}

// Input is timestamp from su script (seconds since 01012000)
string getStrDate(time_t timestamp){
    char buff[20];
    time_t t = timestamp + get2Ktime();
    //printf("t:%d\n",(int)t);
    strftime(buff, 20, "%Y-%m-%d %H:%M:%S", gmtime(&t));
    return buff;
}

//Get Time by String and reurn timestamp since 01012000
time_t getTimeByString(string timestr){
	struct tm tm;
	
	time_t now = time(NULL);
	struct tm *tx = gmtime(&now);
    
    time_t epoch;
    if ( strptime(timestr.c_str(), "%Y-%m-%d %H:%M:%S", &tm) != NULL ){
        tm.tm_isdst=tx->tm_isdst;
        epoch = mktime(&tm);
        //printf("epoch:%d %d\n",(int)epoch,(int)epoch - get2Ktime());
        //printf("timestr:%s\n",timestr.c_str());
        int ret = epoch -get2Ktime(); 
        return ret;
    }
    else if( strptime(timestr.c_str(), "%Y-%m-%d", &tm) != NULL ){
        tm.tm_sec=0;
        tm.tm_min=0;
        tm.tm_hour=0;
        tm.tm_isdst=tx->tm_isdst;
        epoch = mktime(&tm);
        return (epoch - get2Ktime());
    }
    return 0;
}

vector<string> split(string str,string sep){
    char* cstr=const_cast<char*>(str.c_str());
    char* current;
    vector<string> arr;
    current=strtok(cstr,sep.c_str());
    while(current!=NULL){
        arr.push_back(current);
        current=strtok(NULL,sep.c_str());
    }
    return arr;
}

string getCMDStr(uint8_t a_code)
{
    return (g_cmds[a_code] != "" )?g_cmds[a_code]:"Unknown";
}

uint8_t getCMDCode(string &a_str)
{
    map<uint8_t,string>::iterator l_Iter=g_cmds.begin();
    while(l_Iter != g_cmds.end()){
        if (l_Iter->second == a_str){
        	return l_Iter->first;
        }
    	l_Iter++;
    }
    return 0;
}

void initCMDMap()
{
	g_cmds[0xF1] = "OBC_SU_ON";
	g_cmds[0xF2]="OBC_SU_OFF";
	g_cmds[0x02]="SU_RESET";
	g_cmds[0x05]="SU_LDP";
	g_cmds[0x06]="SU_HC";
	g_cmds[0x07]="SU_CAL";
	g_cmds[0x08]="SU_SCI";
	g_cmds[0x09]="SU_HK";
	g_cmds[0x0A]="SU_STM";
	g_cmds[0x0B]="SU_DUMP";
	g_cmds[0x53]="SU_BIAS_ON";
	g_cmds[0xC9]="SU_BIAS_OFF";
	g_cmds[0x35]="SU_MTEE_ON";
	g_cmds[0x9C]="SU_MTEE_OFF";
	g_cmds[0xBB]="SU_ERR";
	g_cmds[0xFA]="OBC_SU_ERR";
	g_cmds[0xFE]="OBC_EOT";
	g_cmds[0x41]="SU_S1_SCRIPT";
	g_cmds[0x42]="SU_S2_SCRIPT";
	g_cmds[0x43]="SU_S3_SCRIPT";
	g_cmds[0x44]="SU_S4_SCRIPT";
	g_cmds[0x45]="SU_S5_SCRIPT";
	g_cmds[0x55]="SU_EOT";
}