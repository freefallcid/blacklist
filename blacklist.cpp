/*
 * main.cpp
 *
 * The MIT License (MIT) {{{
 * 
 * Copyright (c) 2015 Dominic Reich
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. }}}
 * 
 */

/*
 * SOME SPECIAL NOTES FOR THIS PROGRAM
 *  1. When compiling, use -lboost_system in 'link-libraries'
 *  2. When compiling for other hosts, use -static in 'linker-options'
 *
 *
 */

#include <iostream>
#include <fstream>
#include <unistd.h>                             // get the current user-id
#include <boost/system/error_code.hpp>          // ip-address check
#include <boost/asio/ip/address.hpp>            // ip-address check
#include <vector>
//#include <algorithm>                            // sorting the vector
#include <boost/range/algorithm/sort.hpp>       // sort in vector
#include <boost/range/algorithm/unique.hpp>     // get duplicates in vector
#include <boost/range/algorithm_ext/erase.hpp>  // remove duplicates in vectora
#include <boost/algorithm/string/predicate.hpp> // find ip in file (string compare)
#include "version.h"                            // Code::Blocks AutoVersioning


/** \brief Some definitions
 * \param DEFAULT_FILENAME Sets the default filename for the blacklist file
 * \param IPTABLES iptables' default location
 *
 * \info We might change this later so we can include those defines
 * \info at compile time like:
 * \info ./configure --iptables=[new-location] --default-file=[new-location]
 *
 */

#define DEFAULT_FILENAME  "/etc/blacklist-ip"
#define TMP_FILENAME      "/tmp/blacklist"
#define IPTABLES          "/sbin/iptables"

using namespace std;

inline bool checkRoot();
inline bool fileExists(const string &name);
inline bool checkIptables();
void printHelp(bool printAll);
////inline bool checkBlacklistFile(const char *filename);
//const char *setFilename(char *tmpName);
inline bool listFile(const char *filename);
inline bool checkFile(const char *filename);
//inline bool compare(string a, string b);
int loadIptables(const char *filename);
int flushIptables();
int listIptables();
int addIpToIptables(string ip);
int checkIp(string ip);
inline bool addIpToFile(const char *filename, string ip);
inline bool removeIpFromFile(const char *filename, string ip);
inline bool countIpAdresses(const char *filename);
inline bool findIpInFile(const char *filename, string ip);

/** \brief main function (startup function)
 *
 * \param argc int
 * \param argv char**
 * \return int
 *
 */
int main(int argc, char **argv)
{
  if(checkIptables() == false)
    return 1;

  if(argc < 2 || argc > 3)
  {
    printHelp(false);
    return 0;
  }

  string Choice = argv[1];
  int cmd = 0;

  const char *Filename = DEFAULT_FILENAME;

  /// add ip-address to file (check for root)
  if(Choice == "-a")
  {
    if(checkRoot() == false)
      return 1;

    if(argc != 3)
      return 1;

    cmd = addIpToFile(Filename, argv[2]);
    if(cmd == false)
      return 1;

  /// remove ip-address from file (check for root)
  } else if(Choice == "-d")
  {
    if(checkRoot() == false)
      return 1;

    if(argc != 3)
      return 1;

    cmd = removeIpFromFile(Filename, argv[2]);
    if(cmd == false)
      return 1;

  /// list ip-addresses from file on screen (no root required)
  } else if(Choice == "-l")
  {
    if(!listFile(Filename))
      return 1;

  /// find an ip-address in file
  } else if(Choice == "-f")
  {
    if(argc != 3)
      return 1;

    cmd = checkIp(argv[2]);
    if(cmd != 0)
      return 1;

    if(!findIpInFile(Filename, argv[2]))
      return 1;

  /// show how many ips are stored in file
  } else if(Choice == "-C")
  {
    if(!countIpAdresses(Filename))
      return 1;

  /// sort and check file for duplicates (check for root)
  } else if(Choice == "-c")
  {
    if(checkRoot() == false)
      return 1;

    if(!checkFile(Filename))
      return 1;

  /// reload iptables chain BLACKLIST (check for root)
  /// (check file, flush iptables chain, re-load ips from file to chain)
  } else if(Choice == "-r")
  {
    if(checkRoot() == false)
      return 1;

    // flush chain
    cmd = flushIptables();
    if(cmd != 0)
      return 1;

    // check file
    if(!checkFile(Filename))
      return 1;

    // re-load ips
    cmd = loadIptables(Filename);
    if(cmd != 0)
      return 1;

  /// print iptables content to screen (check for root, iptables needs root)
  } else if(Choice == "-L")
  {
    if(checkRoot() == false)
      return 1;

    cmd = listIptables();
    if(cmd != 0)
      return 1;

  /// flush iptables (check for root, iptables needs root)
  } else if(Choice == "-F")
  {
    if(checkRoot() == false)
      return 1;

    cmd = flushIptables();
    if(cmd != 0)
      return 1;

  /// print help screen (no root required)
  } else if(Choice == "-h")
  {
    printHelp(true);
  } else
  {
    printHelp(false);
  }

  /// exit program with exit code 0
  return 0;
}

/** \brief checks for superuser
 *
 * \return bool
 *
 */
inline bool checkRoot()
{
  if(getuid() != 0)
  {
    cout << "you must be root for this operation" << endl;
    return false;
  }
  return true;
}

/** \brief checks for iptables installation
 *
 * \return bool
 *
 */
inline bool checkIptables()
{
  if(!fileExists(IPTABLES))
  {
    cout << "could not find " << IPTABLES << endl;
    return false;
  }
  return true;
}

/** \brief checks if a file exists
 *  Source: http://stackoverflow.com/a/12774387
 *
 * \param name const string&
 * \return bool
 *
 */
inline bool fileExists(const string &name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}

/** \brief prints a help text on screen (short or long)
 *
 * \param printAll bool
 * \return void
 *
 */
void printHelp(bool printAll)
{
  cout << "blacklist " << AutoVersion::FULLVERSION_STRING << ", ";
  cout << AutoVersion::STATUS << endl;
  cout << "Copyright (c) 2015 by Dominic Reich\n" << endl;
  cout << "Usage:  blacklist [options] <ip-address>" << endl;
  if(printAll == true)
  {
    cout << "The blacklist file is located at: " << DEFAULT_FILENAME << endl;
    cout << "Options:  -a   add ip-address (to file)" << endl;
    cout << "          -d   delete ip-address (from file)" << endl;
    cout << "          -l   list ip-addresses (from file)" << endl;
    cout << "          -f   find ip-address in file" << endl;
    cout << "          -c   check file (sort and remove dulicates)" << endl;
    cout << "          -r   reload (check file, flush iptables, load ips from file)" << endl;
    cout << "          -C   count ip-addresses (from file)" << endl;
    cout << "          -L   list ip-addresses (from iptables)" << endl;
    cout << "          -F   flush (iptables)" << endl;
  }

}

/** \brief checks file for duplicates and sorts it
 *
 * \param filename const char*
 * \return bool
 *
 */
inline bool checkFile(const char *filename)
{
  fstream File;

  File.open(filename, ios::in);
  if(!File.is_open())
  {
    return false;
  }

  string line = "";
  vector<string> ips;

  while(getline(File, line))
  {
    ips.push_back(line.c_str());
  }

  boost::erase(ips, boost::unique<boost::return_found_end>(boost::sort(ips)));

  File.close();

  File.open(filename, ios::out | ios::trunc);
  if(!File.is_open())
  {
    return false;
  }

  for(size_t i = 0; i < ips.size(); i++)
  {
    File << ips[i] << endl;
  }

  File.close();

  return true;
}

/** \brief prints content of file to screen
 *
 * \param filename const char*
 * \return bool
 *
 */
inline bool listFile(const char *filename)
{
  ifstream inFile(filename);
  if(!inFile.is_open())
  {
    //cout << "could not open file " << filename << endl;
    return false;
  }

  string line = "";
  while(getline(inFile, line))
  {
    cout << line << endl;
  }
  inFile.close();
  return true;
}

/** \brief sets iptables chain BLACKLIST up with ips from file
 *
 * \param filename const char*
 * \return int
 *
 */
int loadIptables(const char *filename)
{
  ifstream File(filename);
  if(!File.is_open())
  {
    //cout << "could not open file " << filename << endl;
    return 1;
  }

  int cmd = 0;
  string line = "";
  while(getline(File, line))
  {
    cmd = addIpToIptables(line);
    if(cmd != 0)
    {
      return cmd;
    }
  }
  File.close();

  return 0;
}

/** \brief flushes iptables chain BLACKLIST
 *
 * \return int
 *
 */
int flushIptables()
{
  string cmd_iptables = IPTABLES;
  int cmd = 0;
  cmd = system((cmd_iptables+" -F BLACKLIST").c_str());

  return cmd;
}


/** \brief prints iptables content to screen
 *
 * \return int
 *
 */
int listIptables()
{
  string cmd_iptables = IPTABLES;
  int cmd = 0;
  cmd = system((cmd_iptables+" -L BLACKLIST -vn").c_str());

  return cmd;
}

/** \brief adds ip to iptables chain BLACKLIST
 *
 * \param ip string
 * \return int
 *
 */
int addIpToIptables(string ip)
{
  string cmd_iptables = IPTABLES;
  int cmd = 0;
  cmd = checkIp(ip);
  if(cmd != 0)
  {
    return cmd;
  }

  // for now we don't add a destination rule as we normally won't send
  // something if the host is unable to talk to us
  //cmd = system((cmd_iptables+" -A BLACKLIST -d "+ip+"/32 -j DROP").c_str());
  //if(cmd != 0)
  //{
  //  return cmd;
  //}

  // original line
  //cmd = system((cmd_iptables+" -A BLACKLIST -s "+ip+"/32 -j DROP").c_str());
  cmd = system((cmd_iptables+" -A BLACKLIST -s "+ip+" -j DROP").c_str());
  if(cmd != 0)
  {
    return cmd;
  }

  return 0;
}

/** \brief checks for a valid ip-address
 *
 * \param ip string
 * \return int
 *
 */
int checkIp(string ip)
{
  boost::system::error_code ec;
  boost::asio::ip::address::from_string(ip, ec);
  if(ec)
  {
    cout << ec.message() << endl;
    return ec.value();
  }

  return 0;
}

/** \brief adds ip-address to file
 *
 * \param filename const char*
 * \param ip string
 * \return bool
 *
 */
inline bool addIpToFile(const char *filename, string ip)
{
  int cmd = 0;
  cmd = checkIp(ip);
  if(cmd != 0)
  {
    return false;
  }

  ofstream File(filename, ios::app);
  if(!File.is_open())
  {
    return false;
  }
  File << ip << endl;
  File.close();

  // disable auto-check for now
  //checkFile(Filename);

  return true;
}

/** \brief removes ip-address from file
 *
 * \param filename const char*
 * \param ip string
 * \return bool
 *
 */
inline bool removeIpFromFile(const char *filename, string ip)
{
  int cmd = 0;
  cmd = checkIp(ip);
  if(cmd != 0)
  {
    return false;
  }

  if(!findIpInFile(filename, ip))
  {
    return false;
  }

  const char *tmpFile = TMP_FILENAME;

  ifstream inFile(filename);
  if(!inFile.is_open())
  {
    return false;
  }

  ofstream outFile(tmpFile, ios::out);
  if(!outFile.is_open())
  {
    return false;
  }

  string line = "";
  while(getline(inFile, line))
  {
    if(boost::iequals(ip, line) == false)
    {
      outFile << line << endl;
    }
  }
  inFile.close();
  outFile.close();

  ifstream a(tmpFile);
  if(!a.is_open())
  {
    return false;
  }

  ofstream b(filename, ios::out);
  if(!b.is_open())
  {
    return false;
  }

  b << a.rdbuf();

  a.close();
  b.close();

  remove(tmpFile);

  // TODO: Read file, load every line into an array
  //       search ip in that array and remove that array element
  //       write array elements into file (without appending, just create new one)
  return true;
}

/** \brief Counts ip-addresses in file
 *
 * \param filename const char*
 * \return bool
 *
 */
inline bool countIpAdresses(const char *filename)
{
  int Count = 0;

  ifstream File(filename);
  if(!File.is_open())
  {
    //cout << "could not open file " << filename << endl;
    return false;
  }

  string line = "";
  while(getline(File, line))
  {
    Count++;
  }
  File.close();

  //cout << "There are currently " << Count << "IP Adresses blacklisted." << endl;
  // as when we want to use that number to calculate something, only print the count
  cout << Count << endl;
  return true;
}

/** \brief Finds an ip-address in file
 *
 * \param filename const char*
 * \param ip string
 * \return bool
 *
 */
inline bool findIpInFile(const char *filename, string ip)
{
  ifstream File(filename);
  if(!File.is_open())
  {
    return false;
  }

  string line = "";
  while(getline(File, line))
  {
    if(boost::iequals(ip, line) == true)
    {
      //cout << ip << " found in " << filename << "!" << endl;
      return true;
    }
  }
  File.close();
  return false;
}

/* vim: set ts=2 sw=2 tw=0 et :*/
