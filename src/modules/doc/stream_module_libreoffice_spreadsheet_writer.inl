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

#include "ace/Log_Msg.h"

#include "cppuhelper/bootstrap.hxx"

#include "osl/file.hxx"
#include "osl/process.h"
#include "rtl/process.h"

#include "com/sun/star/beans/XPropertySet.hpp"
#include "com/sun/star/bridge/XUnoUrlResolver.hpp"
#include "com/sun/star/frame/Desktop.hpp"
#include "com/sun/star/frame/XComponentLoader.hpp"
#include "com/sun/star/lang/XMultiComponentFactory.hpp"

#include "stream_macros.h"

template <typename SessionMessageType,
          typename MessageType,
          typename ModuleHandlerConfigurationType,
          typename SessionDataType>
Stream_Module_LibreOffice_Spreadsheet_Writer_T<SessionMessageType,
                                               MessageType,
                                               ModuleHandlerConfigurationType,
                                               SessionDataType>::Stream_Module_LibreOffice_Spreadsheet_Writer_T ()
 : inherited ()
 , configuration_ (NULL)
 , document_ ()
 , isInitialized_ (false)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_LibreOffice_Spreadsheet_Writer_T::Stream_Module_LibreOffice_Spreadsheet_Writer_T"));

}

template <typename SessionMessageType,
          typename MessageType,
          typename ModuleHandlerConfigurationType,
          typename SessionDataType>
Stream_Module_LibreOffice_Spreadsheet_Writer_T<SessionMessageType,
                                               MessageType,
                                               ModuleHandlerConfigurationType,
                                               SessionDataType>::~Stream_Module_LibreOffice_Spreadsheet_Writer_T ()
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_LibreOffice_Spreadsheet_Writer_T::~Stream_Module_LibreOffice_Spreadsheet_Writer_T"));

}

//template <typename SessionMessageType,
//          typename MessageType,
//          typename ModuleHandlerConfigurationType,
//          typename SessionDataType>
//void
//Stream_Module_LibreOffice_Spreadsheet_Writer_T<SessionMessageType,
//                            MessageType,
//                            ModuleHandlerConfigurationType,
//                            SessionDataType>::handleDataMessage (MessageType*& message_inout,
//                                                                 bool& passMessageDownstream_out)
//{
//  STREAM_TRACE (ACE_TEXT ("Stream_Module_LibreOffice_Spreadsheet_Writer_T::handleDataMessage"));
//
//  ssize_t bytes_written = -1;
//
//  // don't care (implies yes per default, if part of a stream)
//  ACE_UNUSED_ARG (passMessageDownstream_out);
//
//  // sanity check(s)
//  if (!connection_)
//  {
////    ACE_DEBUG ((LM_ERROR,
////                ACE_TEXT ("failed to open db connection, returning\n")));
//    return;
//  } // end IF
//}

template <typename SessionMessageType,
          typename MessageType,
          typename ModuleHandlerConfigurationType,
          typename SessionDataType>
void
Stream_Module_LibreOffice_Spreadsheet_Writer_T<SessionMessageType,
                                               MessageType,
                                               ModuleHandlerConfigurationType,
                                               SessionDataType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                                       bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_LibreOffice_Spreadsheet_Writer_T::handleSessionMessage"));

  int result = -1;

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (configuration_);

  const typename SessionMessageType::DATA_T& session_data_container_r =
      message_inout->get ();
  SessionDataType& session_data_r =
      const_cast<SessionDataType&> (session_data_container_r.get ());
  switch (message_inout->type ())
  {
    case STREAM_SESSION_BEGIN:
    {
      // sanity check(s)
      ACE_ASSERT (configuration_->socketConfiguration);

      uno::Reference<uno::XInterface> interface_p;
      uno::Reference<uno::XComponentContext> component_context_p;
      uno::Reference<lang::XMultiComponentFactory> client_p;
      std::string connection_string = ACE_TEXT_ALWAYS_CHAR ("uno:socket,host=");
      std::ostringstream converter;
      ::rtl::OUString connection_string_2;
      uno::Reference<bridge::XUnoUrlResolver> resolver_p;
      uno::Reference<frame::XDesktop2> component_loader_p;
      uno::Reference<lang::XMultiComponentFactory> server_p;
      uno::Reference<beans::XPropertySet> property_set_p;
      ::rtl::OUString filename, working_directory, filename_url, absolute_filename_url;
      oslProcessError result_2 = osl_Process_E_InvalidError;
      ::osl::FileBase::RC result_3 = ::osl::FileBase::RC::E_invalidError;
      bool result_4 = false;
      const char* result_p = NULL;

      // debug info
      ACE_TCHAR buffer[BUFSIZ];
      ACE_OS::memset (buffer, 0, sizeof (buffer));
      result =
        configuration_->socketConfiguration->address.addr_to_string (buffer,
                                                                     sizeof (buffer));
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_INET_Addr::addr_to_string(): \"%m\", aborting\n")));
        goto error;
      } // end IF

      ACE_TCHAR host_address[BUFSIZ];
      ACE_OS::memset (host_address, 0, sizeof (host_address));
      result_p =
        configuration_->socketConfiguration->address.get_host_addr (host_address,
                                                                    sizeof (host_address));
      if (!result_p || (result_p != host_address))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_INET_Addr::get_host_addr(\"%s\"): \"%m\", aborting\n"),
                    ACE_TEXT (buffer)));
        goto error;
      } // end IF

      connection_string += ACE_TEXT_ALWAYS_CHAR (host_address);
      connection_string += ACE_TEXT_ALWAYS_CHAR (",port=");
      converter <<
        configuration_->socketConfiguration->address.get_port_number ();
      connection_string += converter.str ();
      connection_string += ACE_TEXT_ALWAYS_CHAR (";urp;StarOffice.ServiceManager");
      connection_string_2 =
        ::rtl::OUString::createFromAscii (ACE_TEXT_ALWAYS_CHAR (connection_string.c_str ()));

      result_4 =
        component_context_p.set (::cppu::defaultBootstrap_InitialComponentContext ());
      ACE_ASSERT (result_4);
      result_4 = client_p.set (component_context_p->getServiceManager ());
      ACE_ASSERT (result_4);
      result_4 =
        interface_p.set (client_p->createInstanceWithContext (ACE_TEXT_ALWAYS_CHAR ("com.sun.star.bridge.UnoUrlResolver"),
                                                              component_context_p));
      ACE_ASSERT (result_4);
      uno::Reference<lang::XComponent>::query (client_p)->dispose ();
      result_4 = resolver_p.set (interface_p,
                                 uno::UNO_QUERY);
      ACE_ASSERT (result_4);
      try
      {
        result_4 = interface_p.set (resolver_p->resolve (connection_string_2),
                                    uno::UNO_QUERY);
      }
      catch (uno::Exception& exception_in)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to establish a connection (was: \"%s\"): \"%s\"\n"),
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
                  ACE_TEXT ("opened LibreOffice connection (was: \"%s\")...\n"),
                  ACE_TEXT (::rtl::OUStringToOString (connection_string_2,
                                                      RTL_TEXTENCODING_ASCII_US,
                                                      OUSTRING_TO_OSTRING_CVTFLAGS).getStr ())));

      result_4 = property_set_p.set (interface_p,
                                     uno::UNO_QUERY);
      ACE_ASSERT (result_4);
      property_set_p->getPropertyValue ("DefaultContext") >>=
        component_context_p;
      //result_4 = server_p.set (component_context_p->getServiceManager ());
      //ACE_ASSERT (result_4);

      result_4 =
        component_loader_p.set (frame::Desktop::create (component_context_p));
      ACE_ASSERT (result_4);

      // generate document filename URL
      filename =
        ::rtl::OUString::createFromAscii (configuration_->targetFileName.c_str ());
      result_2 = osl_getProcessWorkingDir (&working_directory.pData);
      ACE_ASSERT (result_2 == osl_Process_E_None);
      result_3 = ::osl::FileBase::getFileURLFromSystemPath (filename,
                                                            filename_url);
      ACE_ASSERT (result_3 == ::osl::FileBase::RC::E_None);
      result_3 = ::osl::FileBase::getAbsoluteFileURL (working_directory,
                                                      filename_url,
                                                      absolute_filename_url);
      ACE_ASSERT (result_3 == ::osl::FileBase::RC::E_None);
      result_4 =
        document_.set (component_loader_p->loadComponentFromURL (absolute_filename_url,
                                                                 ::rtl::OUString ("_blank"),
                                                                 0,
                                                                 uno::Sequence<beans::PropertyValue> ()));
      ACE_ASSERT (result_4);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("loaded LibreOffice spreadsheet document (was: \"%s\")...\n"),
                  ACE_TEXT (::rtl::OUStringToOString (absolute_filename_url,
                                                      RTL_TEXTENCODING_ASCII_US,
                                                      OUSTRING_TO_OSTRING_CVTFLAGS).getStr ())));

      break;

error:
      session_data_r.aborted = true;

      return;
    }
    case STREAM_SESSION_END:
    {
      // sanity check(s)
//      if (!state_)
//        return; // nothing to do
//
//      std::string query_string = ACE_TEXT_ALWAYS_CHAR ("INSERT INTO ");
//      query_string += configuration_.dataBaseTable
//                   += ACE_TEXT_ALWAYS_CHAR ("VALUES (");
////      for (Test_I_PageDataIterator_t iterator = session_data_r.data.pageData.begin ();
////           iterator != session_data_r.data.pageData.end ();
////           ++iterator)
////      {
////        query_string += ACE_TEXT_ALWAYS_CHAR (",");
//
////        query_string += ACE_TEXT_ALWAYS_CHAR ("),(");
////      } // end FOR
//      query_string += ACE_TEXT_ALWAYS_CHAR (")");
//      my_ulonglong result_2 = 0;
//
//      result = mysql_real_query (state_,
//                                 query_string.c_str (),
//                                 query_string.size ());
//      if (result)
//      {
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("failed to mysql_real_query(\"%s\"): \"%s\", aborting\n"),
//                    ACE_TEXT (query_string.c_str ()),
//                    ACE_TEXT (mysql_error (state_))));
//
//        session_data_r.aborted = true;
//
//        goto close;
//      } // end IF
//      result_2 = mysql_affected_rows (state_);
//      if (result_2 != session_data_r.data.pageData.size ())
//      {
//        ACE_DEBUG ((LM_WARNING,
//                    ACE_TEXT ("failed to store %u data record(s) (result was: %u), continuing\n"),
//                    session_data_r.data.pageData.size (), result_2));
//        goto commit;
//      } // end IF
//      ACE_DEBUG ((LM_DEBUG,
//                  ACE_TEXT ("stored %u data record(s)...\n"),
//                  session_data_r.data.pageData.size (), result_2));
//
//commit:
//    //my_bool result_3 = mysql_commit (state_);
//    //if (result_3)
//    //{
//    //  ACE_DEBUG ((LM_ERROR,
//    //              ACE_TEXT ("failed to mysql_commit(): \"%s\", aborting\n"),
//    //              ACE_TEXT (mysql_error (state_))));
//
//    //  session_data_r.aborted = true;
//
//    //  goto close;
//    //} // end IF
//    //ACE_DEBUG ((LM_DEBUG,
//    //            ACE_TEXT ("committed %u data record(s)...\n"),
//    //            session_data_r.data.pageData.size (), result_2));
//
//close:
//      mysql_close (state_);
//      state_ = NULL;
//      ACE_DEBUG ((LM_DEBUG,
//                  ACE_TEXT ("closed db connection...\n")));

      break;
    }
    default:
      break;
  } // end SWITCH
}

template <typename SessionMessageType,
          typename MessageType,
          typename ModuleHandlerConfigurationType,
          typename SessionDataType>
bool
Stream_Module_LibreOffice_Spreadsheet_Writer_T<SessionMessageType,
                                               MessageType,
                                               ModuleHandlerConfigurationType,
                                               SessionDataType>::initialize (const ModuleHandlerConfigurationType& configuration_in)
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_LibreOffice_Spreadsheet_Writer_T::initialize"));

  configuration_ =
    &const_cast<ModuleHandlerConfigurationType&> (configuration_in);

  if (isInitialized_)
  {
    isInitialized_ = false;
  } // end IF

  isInitialized_ = true;

  return true;
}
template <typename SessionMessageType,
          typename MessageType,
          typename ModuleHandlerConfigurationType,
          typename SessionDataType>
const ModuleHandlerConfigurationType&
Stream_Module_LibreOffice_Spreadsheet_Writer_T<SessionMessageType,
                                               MessageType,
                                               ModuleHandlerConfigurationType,
                                               SessionDataType>::get () const
{
  STREAM_TRACE (ACE_TEXT ("Stream_Module_LibreOffice_Spreadsheet_Writer_T::get"));

  // sanity check(s)
  ACE_ASSERT (configuration_);
  ACE_ASSERT (isInitialized_);


  return *configuration_;
}
