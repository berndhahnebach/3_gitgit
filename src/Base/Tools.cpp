/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"
#ifndef _PreComp_
# include <sstream>
#endif

#include "Tools.h"

namespace Base {
struct string_comp  : public std::binary_function<std::string, 
                                                  std::string, bool>
{
    // s1 and s2 must be numbers represented as string
    bool operator()(const std::string& s1, const std::string& s2)
    {
        if (s1.size() < s2.size())
            return true;
        else if (s1.size() > s2.size())
            return false;
        else
            return s1 < s2;
    }
    static std::string increment(const std::string& s)
    {
        std::string n = s;
        int addcarry=1;
        for (std::string::reverse_iterator it = n.rbegin(); it != n.rend(); ++it) {
            if (addcarry == 0)
                break;
            int d = *it - 48;
            d = d + addcarry;
            *it = ((d%10) + 48);
            addcarry = d / 10;
        }
        if (addcarry > 0) {
            std::string b;
            b.resize(1);
            b[0] = addcarry + 48;
            n = b + n;
        }

        return n;
    }
};
}

std::string Base::Tools::getUniqueName(const std::string& name, const std::vector<std::string>& names, int d)
{
    // find highest suffix
    std::string num_suffix;
    for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); ++it) {
        if (it->substr(0, name.length()) == name) { // same prefix
            std::string suffix(it->substr(name.length()));
            if (suffix.size() > 0) {
                std::string::size_type pos = suffix.find_first_not_of("0123456789");
                if (pos==std::string::npos)
                    num_suffix = std::max<std::string>(num_suffix, suffix, Base::string_comp());
            }
        }
    }

    std::stringstream str;
    str << name;
    if (d > 0) {
        str.fill('0');
        str.width(d);
    }
    str << Base::string_comp::increment(num_suffix);
    return str.str();
}

std::string Base::Tools::getIdentifier(const std::string& name)
{
    // check for first character whether it's a digit
    std::string CleanName = name;
    if (!CleanName.empty() && CleanName[0] >= 48 && CleanName[0] <= 57)
        CleanName[0] = '_';
    // strip illegal chars
    for (std::string::iterator it = CleanName.begin(); it != CleanName.end(); ++it) {
        if (!((*it>=48 && *it<=57) ||  // number
             (*it>=65 && *it<=90)  ||  // uppercase letter
             (*it>=97 && *it<=122)))   // lowercase letter
             *it = '_'; // it's neither number nor letter
    }

    return CleanName;
}
