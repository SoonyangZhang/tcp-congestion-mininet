#include <memory.h>
#include "tcp_trace.h"
namespace tcp{
TcpTrace::TcpTrace(std::string &name){
	OpenTraceFile(name);
}
TcpTrace::~TcpTrace(){
	Close();
}
void TcpTrace::OnRecvData(uint32_t client_id,uint32_t ts,uint32_t len){
	if(log_.is_open()){
		char line [256];
		memset(line,0,256);
		sprintf (line, "%d %16d %16d",
				client_id,ts,len);
		log_<<line<<std::endl;
	}
}
void TcpTrace::Close(){
	if(log_.is_open()){
		log_.close();
	}
}
void TcpTrace::OpenTraceFile(std::string &name){
	log_.open(name.c_str(), std::fstream::out);
}
void TcpTrace::CloseTraceFile(){

}
}
