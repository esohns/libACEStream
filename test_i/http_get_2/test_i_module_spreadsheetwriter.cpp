/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "stdafx.h"

#include "test_i_module_spreadsheetwriter.h"

#include "com/sun/star/frame/FrameSearchFlag.hpp"
//#include "com/sun/star/registry/XSimpleRegistry.hpp"
//#include "cppuhelper/bootstrap.hxx"
//#include "rtl/bootstrap.hxx"
#include "rtl/bootstrap.h"

#include "stream_macros.h"

Test_I_Stream_SpreadsheetWriter::Test_I_Stream_SpreadsheetWriter ()
 : inherited ()
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Stream_SpreadsheetWriter::Test_I_Stream_SpreadsheetWriter"));

}

Test_I_Stream_SpreadsheetWriter::~Test_I_Stream_SpreadsheetWriter ()
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Stream_SpreadsheetWriter::~Test_I_Stream_SpreadsheetWriter"));

}

void
Test_I_Stream_SpreadsheetWriter::handleSessionMessage (Test_I_Stream_SessionMessage*& message_inout,
                                                       bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Test_I_Stream_SpreadsheetWriter::handleSessionMessage"));

  int result = -1;

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);
  ACE_ASSERT (inherited::mod_);

  const Test_I_Stream_SessionData_t& session_data_container_r =
    message_inout->get ();
  Test_I_Stream_SessionData& session_data_r =
    const_cast<Test_I_Stream_SessionData&> (session_data_container_r.get ());
  switch (message_inout->type ())
  {
    case STREAM_SESSION_BEGIN:
    {
      // sanity check(s)
      ACE_ASSERT (inherited::configuration_->socketConfiguration);

//      uno::Reference<uno::XInterface> interface_p;
//      uno::Reference<registry::XSimpleRegistry> registry_p (::cppu::createSimpleRegistry ());
      uno::Reference<uno::XComponentContext> component_context_p; // local context
      uno::Reference<uno::XComponentContext> component_context_2; // remote context
      uno::Reference<lang::XMultiComponentFactory> multi_component_factory_p; // local
      uno::Reference<lang::XMultiComponentFactory> multi_component_factory_2; // remote
      std::string connection_string = ACE_TEXT_ALWAYS_CHAR ("uno:socket,host=");
      std::ostringstream converter;
      ::rtl::OUString connection_string_2;
      uno::Reference<bridge::XUnoUrlResolver> url_resolver_p;
      uno::Reference<frame::XDesktop2> desktop_p;
      uno::Reference<beans::XPropertySet> property_set_p;
      ::rtl::OUString filename, working_directory, filename_url, absolute_filename_url;
      // --> create new frame (see below)
      ::rtl::OUString target_frame_name (RTL_CONSTASCII_USTRINGPARAM (STREAM_DOCUMENT_LIBREOFFICE_FRAME_BLANK));
      oslProcessError result_2 = osl_Process_E_InvalidError;
      ::osl::FileBase::RC result_3 = ::osl::FileBase::RC::E_invalidError;
      bool result_4 = false;
      const char* result_p = NULL;
      sal_Int32 search_flags = frame::FrameSearchFlag::AUTO;
      uno::Sequence<beans::PropertyValue> properties;

      result_2 = osl_getProcessWorkingDir (&working_directory.pData);
      ACE_ASSERT (result_2 == osl_Process_E_None);

      // *TODO*: ::cppu::defaultBootstrap_InitialComponentContext () appears to
      //         work but segfaults in XComponentLoader::loadComponentFromURL()
      //         later on (why ?)
      //      result_4 = component_context_p.set (::cppu::bootstrap (),
      //                                          uno::UNO_QUERY);

      // *NOTE*: strip suffix from .ini/.rc file (see
      //         also: http://api.libreoffice.org/docs/cpp/ref/a00365.html#a296238ca64cb9898a6b8303f12877faa)
      std::string::size_type position =
          inherited::configuration_->libreOfficeRc.find_last_of (ACE_TEXT_ALWAYS_CHAR (STREAM_DOCUMENT_LIBREOFFICE_BOOTSTRAP_FILE_SUFFIX),
                                                                 std::string::npos,
                                                                 ACE_OS::strlen (ACE_TEXT_ALWAYS_CHAR (STREAM_DOCUMENT_LIBREOFFICE_BOOTSTRAP_FILE_SUFFIX)));
      ACE_ASSERT (position != std::string::npos);
      std::string filename_string =
          inherited::configuration_->libreOfficeRc.substr (0, position);
      filename =
          ::rtl::OUString::createFromAscii (filename_string.c_str ());
                 //        ::rtl::OUString::createFromAscii (ACE_TEXT_ALWAYS_CHAR ("/usr/lib/ure/share/misc/types.rdb"));
      result_3 = ::osl::FileBase::getFileURLFromSystemPath (filename,
                                                            filename_url);
      ACE_ASSERT (result_3 == ::osl::FileBase::RC::E_None);
      result_3 = ::osl::FileBase::getAbsoluteFileURL (working_directory,
                                                      filename_url,
                                                      absolute_filename_url);
      ACE_ASSERT (result_3 == ::osl::FileBase::RC::E_None);
//      registry_p->open (absolute_filename_url,
//                        sal_True,
//                        sal_False);
//      ::rtl::Bootstrap::set (::rtl::OUString::createFromAscii (ACE_TEXT_ALWAYS_CHAR ("URE_MORE_TYPES")),
//                             ::rtl::Bootstrap::encode (absolute_filename_url));
      rtl_bootstrap_setIniFileName (absolute_filename_url.pData);
      result_4 =
        component_context_p.set (::cppu::defaultBootstrap_InitialComponentContext (),
//          component_context_p.set (::cppu::bootstrap_InitialComponentContext (registry_p),
                                 uno::UNO_QUERY);
      ACE_ASSERT (result_4);
      result_4 =
        multi_component_factory_p.set (component_context_p->getServiceManager (),
                                       uno::UNO_QUERY);
      ACE_ASSERT (result_4);

      // debug info
      ACE_TCHAR buffer[BUFSIZ];
      ACE_OS::memset (buffer, 0, sizeof (buffer));
      result =
          inherited::configuration_->libreOfficeHost.addr_to_string (buffer,
                                                                     sizeof (buffer));
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ACE_INET_Addr::addr_to_string(): \"%m\", aborting\n"),
                    inherited::mod_->name ()));
        goto error;
      } // end IF

      ACE_TCHAR host_address[BUFSIZ];
      ACE_OS::memset (host_address, 0, sizeof (host_address));
      result_p =
        inherited::configuration_->libreOfficeHost.get_host_addr (host_address,
                                                                  sizeof (host_address));
      if (!result_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ACE_INET_Addr::get_host_addr(\"%s\"): \"%m\", aborting\n"),
                    inherited::mod_->name (),
                    buffer));
        goto error;
      } // end IF
      connection_string += ACE_TEXT_ALWAYS_CHAR (host_address);
      connection_string += ACE_TEXT_ALWAYS_CHAR (",port=");
      converter << inherited::configuration_->libreOfficeHost.get_port_number ();
      connection_string += converter.str ();
      connection_string +=
        ACE_TEXT_ALWAYS_CHAR (";urp;StarOffice.ServiceManager");
      connection_string_2 =
        ::rtl::OUString::createFromAscii (ACE_TEXT_ALWAYS_CHAR (connection_string.c_str ()));
      result_4 =
        url_resolver_p.set (multi_component_factory_p->createInstanceWithContext (::rtl::OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.bridge.UnoUrlResolver")),
                                                                                  component_context_p),
                            uno::UNO_QUERY);
      ACE_ASSERT (result_4);
      try {
        result_4 =
          property_set_p.set (url_resolver_p->resolve (connection_string_2),
                              uno::UNO_QUERY);
      } catch (uno::Exception& exception_in) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: caught exception in XUnoUrlResolver::resolve(\"%s\"): \"%s\", aborting\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (::rtl::OUStringToOString (connection_string_2,
                                                        RTL_TEXTENCODING_ASCII_US,
                                                        OUSTRING_TO_OSTRING_CVTFLAGS).getStr ()),
                    ACE_TEXT (::rtl::OUStringToOString (exception_in.Message,
                                                        RTL_TEXTENCODING_ASCII_US,
                                                        OUSTRING_TO_OSTRING_CVTFLAGS).getStr ())));
        goto error;
      }
      ACE_ASSERT (result_4);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: opened LibreOffice connection (was: \"%s\")...\n"),
                  inherited::mod_->name (),
                  ACE_TEXT (::rtl::OUStringToOString (connection_string_2,
                                                      RTL_TEXTENCODING_ASCII_US,
                                                      OUSTRING_TO_OSTRING_CVTFLAGS).getStr ())));
      property_set_p->getPropertyValue ("DefaultContext") >>=
        component_context_2;
      result_4 =
        multi_component_factory_2.set (component_context_2->getServiceManager ());
      ACE_ASSERT (result_4);
      //result_4 =
      //  desktop_p.set (frame::Desktop::create (component_context_2),
      //                 uno::UNO_QUERY);
      ACE_ASSERT (result_4);
        desktop_p.set (multi_component_factory_2->createInstanceWithContext (::rtl::OUString (RTL_CONSTASCII_USTRINGPARAM ("com.sun.star.frame.Desktop")),
                                                                             component_context_2),
                       uno::UNO_QUERY);
      ACE_ASSERT (result_4);

      // generate document filename URL
      filename =
        ::rtl::OUString::createFromAscii (inherited::configuration_->targetFileName.c_str ());
      result_3 = ::osl::FileBase::getFileURLFromSystemPath (filename,
                                                            filename_url);
      ACE_ASSERT (result_3 == ::osl::FileBase::RC::E_None);
      result_3 = ::osl::FileBase::getAbsoluteFileURL (working_directory,
                                                      filename_url,
                                                      absolute_filename_url);
      ACE_ASSERT (result_3 == ::osl::FileBase::RC::E_None);
      try {
        result_4 =
            inherited::document_.set (desktop_p->loadComponentFromURL (absolute_filename_url, // URL
                                                                       target_frame_name,     // target frame name
                                                                       search_flags,          // search flags
                                                                       properties),           // properties
                                      uno::UNO_QUERY);
      } catch (uno::Exception& exception_in) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: caught exception in XComponentLoader::loadComponentFromURL(\"%s\"): \"%s\", aborting\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (::rtl::OUStringToOString (absolute_filename_url,
                                                        RTL_TEXTENCODING_ASCII_US,
                                                        OUSTRING_TO_OSTRING_CVTFLAGS).getStr ()),
                    ACE_TEXT (::rtl::OUStringToOString (exception_in.Message,
                                                        RTL_TEXTENCODING_ASCII_US,
                                                        OUSTRING_TO_OSTRING_CVTFLAGS).getStr ())));
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: caught exception in XComponentLoader::loadComponentFromURL(\"%s\"), aborting\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (::rtl::OUStringToOString (absolute_filename_url,
                                                        RTL_TEXTENCODING_ASCII_US,
                                                        OUSTRING_TO_OSTRING_CVTFLAGS).getStr ())));
        goto error;
      }
      ACE_ASSERT (result_4);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: loaded LibreOffice spreadsheet document (was: \"%s\")...\n"),
                  inherited::mod_->name (),
                  ACE_TEXT (::rtl::OUStringToOString (absolute_filename_url,
                                                      RTL_TEXTENCODING_ASCII_US,
                                                      OUSTRING_TO_OSTRING_CVTFLAGS).getStr ())));

      // clean up
      uno::Reference<lang::XComponent>::query (multi_component_factory_p)->dispose ();

      break;

error:
      session_data_r.aborted = true;

      return;
    }
    case STREAM_SESSION_END:
    {
      for (Test_I_PortfolioIterator_t iterator = session_data_r.data.begin ();
           iterator != session_data_r.data.end ();
           ++iterator)
      {

      } // end FOR

      break;
    }
    default:
      break;
  } // end SWITCH
}
