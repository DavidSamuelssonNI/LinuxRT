/*
// This example reads all the frames on the bus and displays them in table
// format. This is used to demonstrate a frame input stream session.
// For more information about this type of session, please consult the NI-XNET
// manual.
// Please make sure that the bus is properly terminated as this example does not
// enable the on-board termination. Also ensure that the transceivers are
// externally powered when using C Series modules.
*/
#include <stdlib.h>
#include <stdio.h>
#include "nixnet.h"
#include <unistd.h>

#define NUM_FRAMES 2

static nxSessionRef_t m_SessionRef = 0;


// void DisplayErrorAndExit(nxStatus_t Status, char *Source);


int main(void) {
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
   u64 l_BaudRate = 125000;
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
    printf("error");
      //DisplayErrorAndExit(l_Status, "nxCreateSession");
   }

int c;
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
         sleep(0.001); // Wait 1 ms
      }
      else
      {
         printf("error");
      }
   }
   while (1);

   printf("Data aquisition stopped.\n");

    // Clear the XNET session
   l_Status = nxClear(m_SessionRef);
   if (nxSuccess == l_Status)
   {
      printf("\nSession cleared successfully!\n");
   }
   else
   {
       printf("error");
      //DisplayErrorAndExit(l_Status, "nxClear");
   }
return 0;
}

