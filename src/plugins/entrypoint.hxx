#ifndef _ENTRYPOINT_
#define _ENTRYPOINT_
#pragma once

#include <boost/config.hpp>
#include "../meta_driver.hxx"

class BOOST_SYMBOL_VISIBLE entrypoint
{
public:
    virtual std::map<std::string, MetaDriverFactory *> getInformationAndFactory() = 0;
};

#endif