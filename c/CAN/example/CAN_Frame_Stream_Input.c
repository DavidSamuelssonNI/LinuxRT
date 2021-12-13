/*
// This example reads all the frames on the bus and displays them in table
// format. This is used to demonstrate a frame input stream session.
// For more information about this type of session, please consult the NI-XNET
// manual.
// Please make sure that the bus is properly terminated as this example does not
// enable the on-board termination. Also ensure that the transceivers are
// externally powered when using C Series modules.
*/

#include <stdlib.h>     // Include file for various functions
#include <stdio.h>      // Include file for printf
#include <unistd.h>     // Include Sleep function for Linux
//#include <conio.h>      // Include file for _getch/kbhit WINDOWS ONLY
//#include <windows.h>    // Include file for Win32 time functions WINDOWS ONLY
#include "nixnet.h"     // Include file for NI-XNET functions and constants

//=============================================================================
// Static global variables
//=============================================================================
static nxSessionRef_t m_SessionRef = 0;

//=============================================================================
// Global functions declarations
//=============================================================================
//void DisplayErrorAndExit(nxStatus_t Status, char *Source);
//int PrintTimestamp(nxTimestamp_t *Time);

//=============================================================================
// Main function
//=============================================================================
int main(void)
{
   // Declare all variables for the function
   unsigned int i = 0;
   int l_TypedChar = 0;
   char *l_pSelectedInterface = "CAN2";
   char *l_pSelectedDatabase = "NIXNET_example";
   char *l_pSelectedCluster = "CAN_Cluster";
   char *l_pSelectedFrameList = "CANCyclicFrame1";

   u8 l_Buffer[250 * sizeof(nxFrameCAN_t)]; // Use a large buffer for stream input
   nxFrameVar_t *l_pFrame = NULL;
   u32 l_NumBytes = 0;
   u64 l_BaudRate = 500000;
   nxStatus_t l_Status = 0;

   // Display parameters that will be used for the example.
   printf("Interface: %s\nDatabase: %s\nBaudrate: %llu\n", l_pSelectedInterface,
      l_pSelectedDatabase, l_BaudRate);

   // Create an XNET session in FrameInStream mode
   l_Status = nxCreateSession(l_pSelectedDatabase, l_pSelectedCluster,
      l_pSelectedFrameList, l_pSelectedInterface, nxMode_FrameInStream,
      &m_SessionRef);
   if (nxSuccess == l_Status)
   {
      printf("Session created successfully.\n");
   }
   else
   {
      //DisplayErrorAndExit(l_Status, "nxCreateSession");
   }

   // We are not using a predefined database, so we need to set the Baud Rate
   l_Status = nxSetProperty(m_SessionRef, nxPropSession_IntfBaudRate64,
      sizeof(u64), &l_BaudRate);
   if (nxSuccess == l_Status)
   {
      printf("Properties set successfully.\n");
   }
   else
   {
      //DisplayErrorAndExit(l_Status, "nxSetProperty");
   }

   printf("Logging all received frames. Press q to quit\n");
   printf("Timestamp\t\tID\tData\n");
   do
   {
      // Read the received frames into the buffer
      l_Status = nxReadFrame(m_SessionRef, l_Buffer, sizeof(l_Buffer),
         nxTimeout_None, &l_NumBytes);
      if (nxSuccess == l_Status)
      {
         l_pFrame = (nxFrameVar_t *)l_Buffer;

         // l_pFrame iterates to each frame. When it increments
         // past the number of bytes returned, we reached the end.
         while ((u8 *)l_pFrame < (u8 *)l_Buffer + l_NumBytes)
         {
            // Print timestamp, ID and payload
            //PrintTimestamp(&l_pFrame->Timestamp);
            printf("\t%d\t", l_pFrame->Identifier);
            for (i = 0; i < l_pFrame->PayloadLength; ++i)
            {
               printf("%02X ", l_pFrame->Payload[i]);
            }
            printf("\n");

            // Go to next variable-payload frame.
            l_pFrame = nxFrameIterate(l_pFrame);
         }
         sleep(1); // Wait 1 ms
      }
      else
      {
         //DisplayErrorAndExit(l_Status, "nxReadFrame");
      }

      // if (_kbhit())
      // {
      //    l_TypedChar = _getch();
      // }
   }
   while ('q' != tolower(l_TypedChar));

   printf("Data aquisition stopped.\n");

   // Clear the XNET session
   l_Status = nxClear(m_SessionRef);
   if (nxSuccess == l_Status)
   {
      printf("\nSession cleared successfully!\n");
   }
   else
   {
      //DisplayErrorAndExit(l_Status, "nxClear");
   }

   return 0;
}

//=============================================================================
// Display Error Function
//=============================================================================
// void DisplayErrorAndExit(nxStatus_t Status, char *Source)
// {
//    char l_StatusString[1024];
//    nxStatusToString(Status, sizeof(l_StatusString), l_StatusString);

//    printf("\n\nERROR at %s!\n%s\n", Source, l_StatusString);
//    printf("\nExecution stopped.\nPress any key to quit\n");

//    nxClear(m_SessionRef);

//    _getch();
//    exit(1);
// }

//=============================================================================
// Prints a timestamp to stdout
//=============================================================================
// int PrintTimestamp(nxTimestamp_t *Time)
// {
//    SYSTEMTIME l_STime;
//    FILETIME l_LocalFTime;
//    /* This Win32 function converts from UTC (international) time
//    to the local time zone.  The NI-XNET card keeps time in UTC
//    format (refer to the description of nxTimestamp_t in
//    the NI-XNET reference for Read functions). */
//    FileTimeToLocalFileTime((FILETIME *)Time, &l_LocalFTime);

//    /* This Win32 function converts an absolute time (FILETIME)
//    into SYSTEMTIME, a structure with fields for year, month, day,
//    and so on. */
//    FileTimeToSystemTime(&l_LocalFTime, &l_STime);

//    return printf("%02d/%02d/%04d %02d:%02d:%02d.%03d", l_STime.wMonth, l_STime.wDay,
//       l_STime.wYear, l_STime.wHour, l_STime.wMinute, l_STime.wSecond,
//       l_STime.wMilliseconds);
// }
