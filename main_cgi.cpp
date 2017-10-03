#include <iostream>
#include <iomanip>
#include <vector>  
#include <string>  
#include <stdio.h>  
#include <stdlib.h> 
#include <sstream>
#include <map>
#include <stdint.h>

#include <cgicc/CgiDefs.h> 
#include <cgicc/Cgicc.h> 
#include <cgicc/HTTPHTMLHeader.h> 
#include <cgicc/HTMLClasses.h>  

#include "CScript.h"

using namespace std;
using namespace cgicc;

std::map<uint8_t,string> g_cmds; //dummy!

void ErrorResponse(string &message){
    cout << "Content-type:application/json\r\n\r\n";
    cout << "{ \"error\": \"" << message << "\"}\n";
}

int main () {
   Cgicc cgi;
   
   const_file_iterator file = cgi.getFile("sufile");
   if(file != cgi.getFiles().end()) {
      // send data type at cout.
      //cout << HTTPContentHeader(file->getDataType());
      // write content at cout.

      string::size_type l_size = file->getDataLength();
      //cout << "size:" << l_size << endl;
      //file->writeToStream(l_fstr);
      string l_data = file->getData();

      //cout << "Data:\n";
      char *l_buffer = (char *)malloc(l_size);

      for(int i=0;i < l_size;i++){
         l_buffer[i] = l_data[i];
      	//cout << hex << setfill('0') << setw(2) << (int) l_data[i] << " ";
      }
      CScript l_script;
      l_script.ParseSU(l_buffer,l_size);

      cout << "Content-type:application/json\r\n\r\n";
      cout << l_script.PrintJSON();
   }
   
   const_file_iterator jfile = cgi.getFile("jsonfile");
   if(jfile != cgi.getFiles().end()) {
      string::size_type l_size = jfile->getDataLength();
      string l_data = jfile->getData();
      char *l_buffer = (char *)malloc(l_size+1);
      for(int i=0;i < l_size;i++){
         l_buffer[i] = l_data[i];
      }
      l_buffer[l_size]='\0';
      CScript l_script;
      l_script.ParseJSON(l_buffer);

      cout << "Content-type:application/octet-stream\r\n";
      cout << "Content-Disposition: attachment; filename=\"suscript.bin\"\r\n\r\n";
      //Print the content
      for(int x=0;x<l_script.m_fsize;x++){
         cout << l_script.m_buffer[x];
      }
   }
   


   return 0;
}