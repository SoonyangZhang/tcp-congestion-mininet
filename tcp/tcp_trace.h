#pragma once
#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <string>
namespace tcp{
class TcpTrace{
public:
	TcpTrace(std::string &name);
	~TcpTrace();
	void OnRecvData(uint32_t client_id,uint32_t ts,uint32_t len);
	void Close();
private:
	void OpenTraceFile(std::string &name);
	void CloseTraceFile();
	std::fstream log_;
};
}
