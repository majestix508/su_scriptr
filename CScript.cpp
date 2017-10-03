#include "CScript.h"
#include "parson.h"

CScript::CScript()
{
    m_buffer = NULL;
    m_fsize = 0;

    m_c0=0;
    m_c1=0;
    m_hasChecksum=false;
}

CScript::~CScript()
{
    if(m_buffer)
    	delete []m_buffer;
}

void CScript::Print()
{
    printf("head script_length %d filesize %d\n",m_header.script_length, (int)m_fsize);
    printf("head t_starttime %d == %s\n",m_header.t_starttime,getStrDate(m_header.t_starttime).c_str());
    printf("head file_sn %d\n",m_header.file_sn);
    printf("head sw_ver %d\n",m_header.sw_ver);
    printf("head script_type %d\n",m_header.script_type);
    
    vector<mnlp_times_table_t>::iterator l_Iter = this->m_timetable.begin();
    while(l_Iter != this->m_timetable.end()){
        printf("ttable %02d:%02d:%02d - %s\n",l_Iter->hours, 
        									  l_Iter->minutes, 
        									  l_Iter->seconds, 
        									  getCMDStr(l_Iter->script_index).c_str());
    	l_Iter++;
    }

    vector<mnlp_script_sequence_t>::iterator l_SIter = this->m_seq.begin();
    while(l_SIter != this->m_seq.end()){
	    printf("seq %02d:%02d %s %d %d\n",l_SIter->deltaTIME_min, 
	    	                              l_SIter->deltaTIME_sec,
	    	                              getCMDStr(l_SIter->cmd_id).c_str(),
	    	                              l_SIter->length,
	    	                              l_SIter->seq_cnt);

        if (l_SIter->length >1){
        	int x = l_SIter->length -1;
        	printf("param 0x");
        	for(int y=0;y<x;y++){
                printf("%02x",l_SIter->param[y]);
        	}
        	printf("\n");
        }

    	l_SIter++;
    }

    if (m_hasChecksum){
    	printf("Checksum: 0x%02x 0x%02x\n",m_c0,m_c1);
    }
	char *x = m_buffer;
	for(int a=0;a<m_header.script_length;a++){
	    printf("%02x ",(uint8_t)*x);
	    x++;
    }
    printf("\n");

}

void CScript::WriteToFile(char* filename)
{
    FILE *f = fopen(filename, "wb");
    if (f == NULL)
    {
        printf("Error opening file %s!\n",filename);
        exit(1);
    }

    fwrite(m_buffer,m_fsize,1,f);

    fclose(f);
}

string CScript::PrintJSON()
{
	string json;

    json = "{\n"; //start

	//Print Header
    json +="  \"header\":{\n";
    json +="    \"starttime\": \""+getStrDate(m_header.t_starttime)+"\",\n";
    stringstream fsn;
    fsn << (int)m_header.file_sn;
    json +="    \"filesn\": \""+fsn.str()+"\",\n";
    stringstream sv;
    sv << (int)m_header.sw_ver;
    json +="    \"sw_ver\": "+sv.str()+",\n";
    stringstream st;
    st << (int)m_header.script_type;
    json +="    \"script_type\": "+st.str()+"\n";
    json +="  },\n";

    //Print timetable
    json +="  \"timetable\":[";
    
    vector<mnlp_times_table_t>::iterator l_Iter = m_timetable.begin();
    bool first=true;
    while(l_Iter!=m_timetable.end()){
        if (!first)
        	json +=",\n";
        json +="    {\n";
        stringstream sec;
        sec << (int)l_Iter->seconds;
        json +="        \"second\": "+sec.str()+",\n";
        stringstream min;
        min << (int)l_Iter->minutes;
        json +="        \"minute\": "+min.str()+",\n";
        stringstream hr;
        hr<< (int)l_Iter->hours;
        json +="        \"hour\": "+hr.str()+",\n";
        stringstream scr;
        scr << hex << setfill('0') << setw(2) << (int)l_Iter->script_index;
        json +="        \"script_index\": \""+scr.str()+"\"\n";
        json +="    }";

        first=false;
    	l_Iter++;
    }
    json +="\n  ],\n";

    //Print Sequences
    json +="  \"sequences\":[\n";
    
    vector<mnlp_script_sequence_t>::iterator l_It= m_seq.begin();
    first=true;
    while(l_It != m_seq.end()){
        if (!first)
        	json +=",\n";
        json +="    {\n";
        
        stringstream ssec;
        ssec << (int)l_It->deltaTIME_sec;
        json +="        \"deltaTIME_seconds\":"+ssec.str()+",\n";
        stringstream smin;
        smin << (int)l_It->deltaTIME_min;
        json +="        \"deltaTIME_minutes\":"+smin.str()+",\n";
    	stringstream cmd;
    	cmd << hex << setfill('0') << setw(2) << (int)l_It->cmd_id;
    	json +="        \"cmd_id\": \""+cmd.str()+"\",\n";
        stringstream lng;
        lng << (int)l_It->length;
        json +="        \"length\": "+lng.str()+",\n";
        stringstream sc;
        sc << (int)l_It->seq_cnt;
        json +="        \"seq_cnt\": "+sc.str()+",\n";

        stringstream params;
        bool f2=true;
        for(int i=0; i<l_It->length-1;i++){
            if(!f2){
            	params << " ";
            }
            params << hex << setfill('0') << setw(2) << (int)l_It->param[i];
            f2=false;
        }
        json +="        \"params\": \""+params.str()+"\"\n";

        json +="    }";
        first=false;
    	l_It++;
    }

    json +="\n  ]\n";

	json += "}\n"; //end
    return json;
}

void CScript::ParseJSONFile(char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (f==NULL){
        printf("Can't open file %s\n",filename);
        return;
    }
    fseek(f, 0, SEEK_END);
    long l_fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *l_buffer = (char *)malloc(l_fsize+1);
    fread(l_buffer, m_fsize, 1, f);
    fclose(f);
    l_buffer[l_fsize]='\0';
    this->ParseJSON(l_buffer);
}

void CScript::ParseJSON(char *jsonstring)
{   
    JSON_Value *jval = json_parse_string(jsonstring);
	//JSON_Value *jval = json_parse_file(filename);
	if (jval==NULL){
		printf("Error parsing JSON file\n");
		return;
	}
	int script_size=0;

	JSON_Object *root_obj = json_value_get_object (jval);
	m_header.t_starttime = (uint32_t)getTimeByString(json_object_dotget_string(root_obj, "header.starttime"));
	m_header.file_sn = (uint32_t)atoi(json_object_dotget_string(root_obj, "header.file_sn"));
	m_header.sw_ver = (uint8_t)json_object_dotget_number(root_obj, "header.sw_ver");
    m_header.script_type = (uint8_t)json_object_dotget_number(root_obj, "header.script_type");

    script_size=sizeof(mnlp_script_header_t);

    JSON_Array  *l_tt = json_object_get_array(root_obj,"timetable");
    for (int i = 0; i < json_array_get_count(l_tt); i++) {
        JSON_Object *stt = json_array_get_object(l_tt, i);
        mnlp_times_table_t l_tstr;
        l_tstr.seconds = json_object_dotget_number(stt, "second");
        l_tstr.minutes = json_object_dotget_number(stt, "minute");
        l_tstr.hours = json_object_dotget_number(stt, "hour");
        l_tstr.script_index = (uint8_t)strtol(json_object_dotget_string(stt, "script_index"), NULL, 16);
        this->m_timetable.push_back(l_tstr);
        script_size+=sizeof(mnlp_times_table_t);
    }

    JSON_Array  *l_sq = json_object_get_array(root_obj,"sequences");
    for (int i = 0; i < json_array_get_count(l_sq); i++) {
        JSON_Object *stt = json_array_get_object(l_sq, i);
        mnlp_script_sequence_t l_tec;
        l_tec.deltaTIME_sec = (uint8_t)json_object_dotget_number(stt, "deltaTIME_seconds");
        l_tec.deltaTIME_min= (uint8_t)json_object_dotget_number(stt, "deltaTIME_minutes");
        l_tec.cmd_id = (uint8_t)strtol(json_object_dotget_string(stt, "cmd_id"),NULL,16);
        l_tec.length = (uint8_t)json_object_dotget_number(stt, "length");
        l_tec.seq_cnt = (uint8_t)json_object_dotget_number(stt, "seq_cnt");
        
        script_size+=5;

        string l_params = json_object_dotget_string(stt, "params");
        vector<string> token = split(json_object_dotget_string(stt, "params")," ");
        vector<string>::iterator l_Iter = token.begin();
        uint8_t *p = l_tec.param;
        while(l_Iter != token.end()){
            *p = (uint8_t)strtol((*l_Iter).c_str(),NULL,16);
            p++;
        	l_Iter++;
        }
        this->m_seq.push_back(l_tec);
        script_size+=token.size();
    }
    m_header.script_length=script_size+2;//checksum!!

    //Create m_buffer and m_fsize
    char *buffer = (char*)malloc(m_header.script_length);
    memset(buffer,0,m_header.script_length);
    char *p = buffer;

    memcpy(p,&m_header,sizeof(mnlp_script_header_t));
    p+=sizeof(mnlp_script_header_t);

    vector<mnlp_times_table_t>::iterator l_Iter = m_timetable.begin();
    while(l_Iter!=m_timetable.end()){
        
        memcpy(p,&(*l_Iter),sizeof(mnlp_times_table_t));
        p+=sizeof(mnlp_times_table_t);

    	l_Iter++;
    }
    vector<mnlp_script_sequence_t>::iterator l_SIter = m_seq.begin();
    while(l_SIter!=m_seq.end()){
        
        *p = l_SIter->deltaTIME_sec;
        p++;
        *p = l_SIter->deltaTIME_min;
        p++;
        *p = l_SIter->cmd_id;
        p++;
        *p = l_SIter->length;
        p++;
        *p = l_SIter->seq_cnt;
        p++;

        for(int i=0;i<l_SIter->length-1;i++){
        	*p=l_SIter->param[i];
        	p++;
        }

    	l_SIter++;
    }

    this->m_hasChecksum=false;
    this->m_buffer = buffer;
    this->m_fsize = m_header.script_length-2;
    
    m_buffer = (char *)realloc(m_buffer,m_fsize+2);
    AdjustFletcher2Zero((uint8_t *)m_buffer,(size_t)m_fsize);
    m_fsize+=2;
    m_c0 = m_buffer[m_fsize-2];
    m_c1 = m_buffer[m_fsize-1];
    m_hasChecksum = true;

}

void CScript::ParseSUFile(char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (f==NULL){
        printf("Can't open file %s\n",filename);
        return;
    }
    fseek(f, 0, SEEK_END);
    long l_fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *l_buffer = (char *)malloc(l_fsize);
    fread(l_buffer, m_fsize, 1, f);
    fclose(f);
    this->ParseSU(l_buffer,l_fsize);
}

void CScript::ParseSU(char *buffer, long size)
{
    m_buffer = buffer;
    m_fsize = size;

    int remaining = (int)m_fsize;

    char *p = m_buffer;
	memcpy(&m_header,p,sizeof(mnlp_script_header_t));

	p+=sizeof(mnlp_script_header_t);
	remaining -=sizeof(mnlp_script_header_t);

    while(true){
        mnlp_times_table_t ttable;
        memcpy(&ttable,p, sizeof(mnlp_times_table_t));
        
        m_timetable.push_back(ttable);

        if (ttable.script_index == 0x55)
        	break;

        p=p+sizeof(mnlp_times_table_t);
        remaining -=sizeof(mnlp_times_table_t);
    }

    p=p+sizeof(mnlp_times_table_t);
    remaining-=sizeof(mnlp_times_table_t);

    while(true){
	    mnlp_script_sequence_t seq;
	    seq.deltaTIME_sec = *p;
	    p++;
	    seq.deltaTIME_min = *p;
	    p++;
	    seq.cmd_id = *p;
	    p++;
	    seq.length = *p;
	    p++;
	    seq.seq_cnt = *p;
	    p++;
	    remaining-=5;

	    if (seq.length>1){
		    memcpy(seq.param,p,seq.length-1);
		    p+=(seq.length-1);
		    remaining-=seq.length-1;
	    }

	    m_seq.push_back(seq);
	    if(remaining <=2)
	       break;
    }

    if (remaining==2){ //there's a checksum
        m_c0 = (uint8_t)*p;
        m_c1 = (uint8_t)*(++p);
        m_hasChecksum=true;
    }
    else {
        m_buffer = (char *)realloc(m_buffer,m_fsize+2);
        AdjustFletcher2Zero((uint8_t *)m_buffer,(size_t)m_fsize);
        m_fsize+=2;
        m_c0 = m_buffer[m_fsize-2];
        m_c1 = m_buffer[m_fsize-1];
        m_hasChecksum = true;
     }
}

bool CScript::CheckChecksum()
{
	if (!this->HasChecksum())
		this->CreateChecksum();

    uint16_t csum1;
	csum1 = SlowFletcher16( (uint8_t*)m_buffer, (int)m_fsize);
    if (csum1==0)
    	return true;

    return false;
}

void CScript::PrintChecksum(){
	printf("Checksum: 0x%02x 0x%02x\n",m_c0,m_c1);
}

void CScript::CreateChecksum()
{
	int size = m_fsize;
	if (this->HasChecksum())
		size -=2;

    uint16_t csum;
	uint8_t c0,c1,f0,f1;
	csum = fletcher16( (uint8_t*)m_buffer, size);
	f0 = csum & 0xff;
	f1 = (csum >> 8) & 0xff;
	m_c1 = 0xff - (( f0 + f1) % 0xff);
	m_c0 = 0xff - (( f0 + c0 ) % 0xff);
    //printf("calculated checksum 0x%02x 0x%02x\n",m_c0,m_c1);
}

bool CScript::HasChecksum()
{
	if (m_hasChecksum)
		return true;
	return false;
}