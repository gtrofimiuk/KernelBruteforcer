#pragma once
#include "cxxopts.hpp"
#include "defines.h"

class CCmdOptionParser
{
	bool m_Parsed;
	int m_argc;
	char** m_ppargv;
public:
	cxxopts::Options m_Opt;
	cxxopts::ParseResult m_Result;
	CCmdOptionParser(const char* pName, const char* pDescription,unsigned argc,char** argv):
		m_Opt(pName,pDescription),m_Parsed(false),
		m_argc(argc),m_ppargv(argv)
	{

	}
	const cxxopts::OptionValue& operator[](const std::string& option) 
	{
		if (!m_Parsed)
		{
			m_Result = m_Opt.parse(m_argc,m_ppargv);
			m_Parsed = true;
		};
		return m_Result[option];
	}
};
template <class T> class CCmdOptionVal
{
	const std::string m_LongName;
	CCmdOptionParser& m_Parser;
public:
	CCmdOptionVal(CCmdOptionParser& O, ///option engine
		const std::string&  ShortName,///short option name
		const std::string&  LongName, ///long option name
		const std::string&  Description///description of the option
	):m_LongName(LongName),m_Parser(O)
	{
		if (ShortName.length() > 1) throw Exception("%s: short option name cannot be longer than 1",ShortName.c_str());
		O.m_Opt.add_option("",ShortName, LongName,Description, cxxopts::value<T>(), "");
	}
	CCmdOptionVal(CCmdOptionParser& O, ///option engine
		const std::string&  ShortName,///short option name
		const std::string&  LongName, ///long option name
		const std::string&  Description,///description of the option
		const char* pDefaultValue ///default value in string format
	) :m_LongName(LongName), m_Parser(O)
	{
		if (ShortName.length() > 1)
			throw Exception("%s: short option name cannot be longer than 1", ShortName.c_str());
		O.m_Opt.add_option("", ShortName, LongName, Description, cxxopts::value<T>()->default_value(pDefaultValue), "");
	};
	bool Present()const
	{
		return m_Parser[m_LongName].count();
	};
	operator const T&()const
	{
		return m_Parser[m_LongName].as<T>();
	};
};


#define CMDOPTIONS_BEGIN(Description)\
	CCmdOptionParser CmdLineEngine(argv[0], Description, argc, argv);try{

#define CMDOPTION_VALUE(type,longname,shortname,description)\
		CCmdOptionVal<type> longname(CmdLineEngine, shortname, #longname, description);
#define CMDOPTION_VALUE_DEF(type,longname,shortname,Description,Default)\
		CCmdOptionVal<type> longname(CmdLineEngine, shortname, #longname, Description,Default);

#define CMDOPTIONS_END }catch (const cxxopts::OptionException& ex){std::cout << CmdLineEngine.m_Opt.help();std::cerr << ex.what() << std::endl;return 1;}