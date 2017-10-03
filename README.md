# Science Unit Script Generator

This is a helper tool for the Science Scripts of the QB50 CubeSat


## Installation

libcgicc is required for the cgi. Install it on a Debian/Ubuntu like system with:

```
sudo apt-get install libcgicc-dev
```

## Usage

```
//Convert json to SU-Script
scriptr -j ./testscript.json -o ./output.bin

//Generate Checksum for SU-Script without checksum
scriptr -s ./testscript_nochecksum.bin -o ./output

//Print content of SU-Script (works with -j and -s)
scriptr -s ./testscript.bin -p

//Show checksum for Script
scriptr -j ./testscript.json -f

//Check the checksum for a script
scriptr -s ./testscript.bin -c

```

## Format of the JSON File

```
{
	"header":{
		"starttime": "2015-06-01 12:00:00", //Starttime of the script (UTC!)
                                            // Allowed formats: YYYY-MM-DD 
                                            //                  YYYY-MM-DD HH:mm:SS
		"file_sn": "1", // File serial number (intern 4byte)
		"sw_ver": 65,   // 1 byte (dec) - b4..b0 Version of script tool
                        //                b6,b5 = 10 for MNLP
                        //                b7 not used -> default "0"
		"script_type": 68 // 1 byte (dec) see page 51 in the pdf
	},
	"timetable":[
		{
			"second":0, // Range 0 - 59
			"minute":6, // Range 0 - 59
			"hour":12, // Range 0 - 23
			"script_index":"41" // 0x41 = S1, 0x42 = S2, 0x43 = S3, 0x44 = S4, 0x45 = S5, 0x55 = EOT
		},
		{
			"second":0,
			"minute":7,
			"hour":12,
			"script_index":"55" // timetable must end with a 0x55
		}

	],
	"sequences":[
		{
			"deltaTIME_seconds":10, // 0-59 seconds to wait
			"deltaTIME_minutes":0, // 0-59 minutes to wait
			"cmd_id":"F1", // hex value of command (see common.h for all available commands!)
			"length":1, //len of bytes for parameter (seq_cnt is always the first byte!)
			"seq_cnt":1, //command counter 0-255 then rollover
			"params":"" //parameter bytes - write hex bytes with " " seperator
                        // e.g. "0A 0C" for 2 bytes hex
		},
		{
			"deltaTIME_seconds":10,
			"deltaTIME_minutes":0,
			"cmd_id":"06",
			"length":1,
			"seq_cnt":2,
			"params":""
		},
		{
			"deltaTIME_seconds":10,
			"deltaTIME_minutes":0,
			"cmd_id":"06",
			"length":1,
			"seq_cnt":3,
			"params":""
		},
		{
			"deltaTIME_seconds":10,
			"deltaTIME_minutes":0,
			"cmd_id":"F2",
			"length":1,
			"seq_cnt":4,
			"params":""
		},
		{
			"deltaTIME_seconds":10,
			"deltaTIME_minutes":0,
			"cmd_id":"FE", // Always end a Script part with 0xFE = OBC_EOT
                           // In this script this indicates the end of S1 part!
			"length":1,
			"seq_cnt":5,
			"params":""
		}
	]
}

```
