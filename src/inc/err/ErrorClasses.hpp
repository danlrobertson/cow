/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef COW_ERRORS_HPP
#define COW_ERRORS_HPP

#include <cstring>

namespace cow {
namespace err {

class Err {
private:
  char   m_msg[250];
public:
  Err() { strcpy( m_msg, "cow::err::Err: Unknown Error" ); }

  Err( const char* msg ) { strncpy( m_msg, msg, 200 ); }
  const char* what() { return m_msg; }
};

class CowBaseErr : public Err {
private:
  static char s_str[25];
public:
  CowBaseErr(): Err() { }

  CowBaseErr( const char* msg ): Err( strcat( s_str, msg ) ) { }
};

char CowBaseErr::s_str[25] = "cow::err::CowBaseErr: ";

} // namespace err
} // namespace cow

#endif
