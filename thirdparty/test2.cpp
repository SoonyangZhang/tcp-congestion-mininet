/*
Copyright (c) 2009, Hideyuki Tanaka
All rights reserved.
from  http://d.hatena.ne.jp/tanakh/20091028
*/

#include "cmdline.h"

#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
  cmdline::parser a;
  a.add<string>("host", 'h', "host name", true, "");
  a.add<int>("port", 'p', "port number", false, 80, cmdline::range(1, 65535));
  a.add<string>("type", 't', "protocol type", false, "http", cmdline::oneof<string>("http", "https", "ssh", "ftp"));
  a.add("help", 0, "print this message");
  a.footer("filename ...");
  a.set_program_name("test");

  bool ok=a.parse(argc, argv);

  if (argc==1 || a.exist("help")){
    cerr<<a.usage();
    return 0;
  }
  
  if (!ok){
    cerr<<a.error()<<endl<<a.usage();
    return 0;
  }

  cout<<a.get<string>("host")<<":"<<a.get<int>("port")<<endl;

  for (size_t i=0; i<a.rest().size(); i++)
    cout<<"- "<<a.rest()[i]<<endl;

  return 0;
}
